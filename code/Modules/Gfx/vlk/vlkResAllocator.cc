//------------------------------------------------------------------------------
//  vlkResAllocator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlk_impl.h"
#include "vlkResAllocator.h"
#include "vlkConfig.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkResAllocator::~vlkResAllocator() {
    o_assert(!this->valid);
    o_assert(this->releaseQueue.Empty());
}

//------------------------------------------------------------------------------
void
vlkResAllocator::setup(VkPhysicalDevice gpu) {
    o_assert(!this->valid);
    o_assert(gpu);
    this->valid = true;
    vkGetPhysicalDeviceMemoryProperties(gpu, &this->memoryProps);
}

//------------------------------------------------------------------------------
void
vlkResAllocator::discard(VkDevice dev, VkCommandPool cmdPool) {
    o_assert(this->valid);
    freeItem item;
    while (!this->releaseQueue.Empty()) {
        this->releaseQueue.Dequeue(item);
        this->destroy(dev, cmdPool, item);
    }
    this->valid = false;
}

//------------------------------------------------------------------------------
int
vlkResAllocator::findMemoryType(uint32 typeBits, VkFlags reqMask) {
    o_assert_dbg(this->valid);
    for (int i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
        if (typeBits & (1 << i)) {
            VkMemoryPropertyFlags propMask = this->memoryProps.memoryTypes[i].propertyFlags;
            if ((propMask & reqMask) == reqMask) {
                return i;
            }
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
void
vlkResAllocator::destroy(VkDevice dev, VkCommandPool cmdPool, const freeItem& item) {
    o_assert_dbg(this->valid);
    o_assert_dbg(item.res);
    switch (item.type) {
        case freeItem::Buffer:
            this->destroyBuffer(dev, (VkBuffer)item.res, item.mem);
            break;
        default:
            o_assert(false);
            break;
    }
}

//------------------------------------------------------------------------------
void
vlkResAllocator::garbageCollect(VkDevice dev, VkCommandPool cmdPool, uint64 frameIndex) {
    o_assert_dbg(this->valid);

    // release all resources from longer then NumFrames befores,
    // these are definitely no longer accessed by the GPU
    const uint64 safeNumFrames = vlkConfig::NumFrames + 2;
    if (frameIndex > safeNumFrames) {
        const uint64 minReleaseFrame = frameIndex - safeNumFrames;
        freeItem item;
        while (!this->releaseQueue.Empty() && (this->releaseQueue.Front().frameIndex < minReleaseFrame)) {
            this->releaseQueue.Dequeue(item);
            this->destroy(dev, cmdPool, item);
        }
    }
}

//------------------------------------------------------------------------------
vlkResAllocator::BufferItem
vlkResAllocator::allocDeviceBuffer(VkDevice dev, VkBufferUsageFlags usage, uint32 size) {
    o_assert_dbg(this->valid);
    o_assert_dbg(dev);
    o_assert_dbg(size >= 0);
    BufferItem result;

    // allocate buffer object
    VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufInfo.size = size;
    bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkResult err = vkCreateBuffer(dev, &bufInfo, nullptr, &result.buffer);
    o_assert(!err && result.buffer);

    // allocate in device-local memory
    VkMemoryRequirements memReqs = { };
    vkGetBufferMemoryRequirements(dev, result.buffer, &memReqs);
    VkMemoryAllocateInfo memAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = this->findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    o_assert(InvalidIndex != memAlloc.memoryTypeIndex);
    err = vkAllocateMemory(dev, &memAlloc, nullptr, &result.memory);
    o_assert(!err && result.memory);

    // bind memory to buffer
    err = vkBindBufferMemory(dev, result.buffer, result.memory, 0);
    o_assert(!err);

    return result;
}

//------------------------------------------------------------------------------
vlkResAllocator::BufferItem
vlkResAllocator::allocStagingBuffer(VkDevice dev, VkBufferUsageFlags usage, uint32 size) {
    o_assert_dbg(this->valid);
    o_assert_dbg(dev);
    o_assert_dbg(size >= 0);
    BufferItem result;

    // allocate buffer object
    VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufInfo.size = size;
    bufInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkResult err = vkCreateBuffer(dev, &bufInfo, nullptr, &result.buffer);
    o_assert(!err && result.buffer);

    // allocate in host-visible memory
    VkMemoryRequirements memReqs = { };
    vkGetBufferMemoryRequirements(dev, result.buffer, &memReqs);
    VkMemoryAllocateInfo memAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = this->findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    o_assert(InvalidIndex != memAlloc.memoryTypeIndex);
    err = vkAllocateMemory(dev, &memAlloc, nullptr, &result.memory);
    o_assert(!err && result.memory);

    // bind memory to buffer
    err = vkBindBufferMemory(dev, result.buffer, result.memory, 0);
    o_assert(!err);

    return result;
}

//------------------------------------------------------------------------------
vlkResAllocator::BufferItem
vlkResAllocator::allocStaticBuffer(VkDevice dev, VkCommandBuffer cmdBuf, uint64 frameIndex, VkBufferUsageFlags usage, const void* data, uint32 size) {
    o_assert_dbg(this->valid);
    o_assert_dbg(dev && cmdBuf);
    BufferItem deviceBuffer = this->allocDeviceBuffer(dev, usage, size);
    if (data) {
        this->copyBufferData(dev, cmdBuf, frameIndex, usage, deviceBuffer.buffer, nullptr, nullptr, data, size);
    }
    return deviceBuffer;
}

//------------------------------------------------------------------------------
void
vlkResAllocator::releaseBuffer(uint64 frameIndex, const BufferItem& item) {
    o_assert_dbg(this->valid);
    o_assert_dbg(item.buffer && item.memory);
    this->releaseQueue.Enqueue(freeItem(frameIndex, uint64(item.buffer), item.memory, freeItem::Buffer));
}

//------------------------------------------------------------------------------
void
vlkResAllocator::destroyBuffer(VkDevice dev, VkBuffer buf, VkDeviceMemory mem) {
    o_assert_dbg(this->valid);
    o_assert_dbg(dev && buf && mem);
    vkFreeMemory(dev, mem, nullptr);
    vkDestroyBuffer(dev, buf, nullptr);
}

//------------------------------------------------------------------------------
VkAccessFlags
vlkResAllocator::bufferUsageToAccess(VkBufferUsageFlags usage) {
    if (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
        return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    }
    else if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        return VK_ACCESS_INDEX_READ_BIT;
    }
    else if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        return VK_ACCESS_UNIFORM_READ_BIT;
    }
    else {
        o_error("Unknown buffer usage!\n");
        return 0;
    }
}

//------------------------------------------------------------------------------
void
vlkResAllocator::copyBufferData(VkDevice dev, VkCommandBuffer cmdBuf, uint64 frameIndex, VkBufferUsageFlags dstUsage, VkBuffer dstBuf, VkBuffer optStagingBuf, VkDeviceMemory optStagingMem, const void* data, uint32 size) {
    o_assert(this->valid);
    o_assert(dev && cmdBuf && dstBuf);
    o_assert(data && (size > 0));

    // transition dst buffer into transfer-dest state
    VkAccessFlags dstAccess = bufferUsageToAccess(dstUsage);
    if (dstBuf) {
        this->transitionBufferAccess(cmdBuf, dstBuf, dstAccess, VK_ACCESS_TRANSFER_WRITE_BIT);
    }

    // create temporary staging buffer with upload data if none is provided
    BufferItem stagingItem;
    stagingItem.buffer = optStagingBuf;
    stagingItem.memory = optStagingMem;
    if (!stagingItem.buffer) {
        stagingItem = this->allocStagingBuffer(dev, 0, size);
    }

    // copy data into staging buffer
    if (data) {
        o_assert(stagingItem.memory);
        void* dstPtr = nullptr;
        VkResult err = vkMapMemory(dev, stagingItem.memory, 0, size, 0, &dstPtr);
        o_assert(!err && dstPtr);
        Memory::Copy(data, dstPtr, size);
        vkUnmapMemory(dev, stagingItem.memory);
    }

    // add upload command to command list and transition back into usage state
    if (dstBuf) {
        VkBufferCopy region = { };
        region.size = size;
        vkCmdCopyBuffer(cmdBuf, stagingItem.buffer, dstBuf, 1, &region);
        this->transitionBufferAccess(cmdBuf, dstBuf, VK_ACCESS_TRANSFER_WRITE_BIT, dstAccess);
    }

    // if an upload buffer was created, release it
    if (!optStagingBuf) {
        this->releaseBuffer(frameIndex, stagingItem);
    }
}

//------------------------------------------------------------------------------
void
vlkResAllocator::transitionBufferAccess(VkCommandBuffer cmdBuf, VkBuffer buf, VkAccessFlags oldAccess, VkAccessFlags newAccess) {
    o_assert_dbg(cmdBuf && buf);

    VkBufferMemoryBarrier bufMemBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
    bufMemBarrier.srcAccessMask = oldAccess;
    bufMemBarrier.dstAccessMask = newAccess;
    bufMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufMemBarrier.buffer = buf;
    bufMemBarrier.size = VK_WHOLE_SIZE;
    const VkBufferMemoryBarrier* pBufMemBarriers = &bufMemBarrier;
    vkCmdPipelineBarrier(cmdBuf,                // commandBuffer
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,      // srcStageMask
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,      // dstStageMask
        0,                                      // dependencyFlags
        0,                                      // memoryBarrierCount
        nullptr,                                // pMemoryBarriers
        1,                                      // bufferMemoryBarrierCount
        &bufMemBarrier,                         // pBufferMemoryBarriers
        0,                                      // imageMemoryBarrierCount
        nullptr);                               // pImageMemoryBarriers
}

//------------------------------------------------------------------------------
void
vlkResAllocator::transitionImageLayout(VkCommandBuffer cmdBuf, VkImage img, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout) {
    o_assert(cmdBuf && img);

    VkImageMemoryBarrier imgMemBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    imgMemBarrier.oldLayout = oldLayout;
    imgMemBarrier.newLayout = newLayout;
    imgMemBarrier.image = img;
    imgMemBarrier.subresourceRange = { aspectMask, 0, 1, 0, 1 };
    switch (newLayout) {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        imgMemBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        imgMemBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        break;
    }
    const VkImageMemoryBarrier* pImgMemBarriers = &imgMemBarrier;
    vkCmdPipelineBarrier(cmdBuf,                                // commandBuffer
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,     // srcStageMask
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,     // dstStageMask
        0,                                     // dependencyFlags
        0,                                     // memoryBarrierCount
        nullptr,                               // pMemoryBarriers
        0,                                     // bufferMemoryBarrierCount
        nullptr,                               // pBufferMemoryBarriers
        1,                                     // imageMemoryBarrierCount
        pImgMemBarriers);                      // pImageMemoryBarriers
}

} // namespace _priv
} // namespace Oryol
