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
    o_assert(this->releaseQueue.Empty());
}

//------------------------------------------------------------------------------
void
vlkResAllocator::destroyAll(VkDevice dev, VkCommandPool cmdPool) {
    freeItem item;
    while (!this->releaseQueue.Empty()) {
        this->releaseQueue.Dequeue(item);
        this->destroy(dev, cmdPool, item);
    }
}

//------------------------------------------------------------------------------
void
vlkResAllocator::destroy(VkDevice dev, VkCommandPool cmdPool, const freeItem& item) {
    o_assert_dbg(item.res);
    switch (item.type) {
        case freeItem::CommandBuffer:
            this->destroyCommandBuffer(dev, cmdPool, (VkCommandBuffer)item.res);
            break;
        default:
            o_assert(false);
            break;
    }
}

//------------------------------------------------------------------------------
void
vlkResAllocator::garbageCollect(VkDevice dev, VkCommandPool cmdPool, uint64 frameIndex) {

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
VkCommandBuffer
vlkResAllocator::allocCommandBuffer(VkDevice dev, VkCommandPool cmdPool) {
    o_assert_dbg(dev && cmdPool);
    VkCommandBufferAllocateInfo bufInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    bufInfo.commandPool = cmdPool;
    bufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    bufInfo.commandBufferCount = 1;
    VkCommandBuffer cmdBuf = nullptr;
    VkResult err = vkAllocateCommandBuffers(dev, &bufInfo, &cmdBuf);
    o_assert(!err && cmdBuf);
    return cmdBuf;
}

//------------------------------------------------------------------------------
void
vlkResAllocator::destroyCommandBuffer(VkDevice dev, VkCommandPool cmdPool, VkCommandBuffer cmdBuf) {
    o_assert_dbg(dev && cmdPool && cmdBuf);
    vkFreeCommandBuffers(dev, cmdPool, 1, &cmdBuf);
}

//------------------------------------------------------------------------------
void
vlkResAllocator::releaseCommandBuffer(uint64 frameIndex, VkCommandBuffer cmdBuf) {
    o_assert_dbg(cmdBuf);
    this->releaseQueue.Enqueue(freeItem(frameIndex, uint64(cmdBuf), freeItem::CommandBuffer));
}

//------------------------------------------------------------------------------
void
vlkResAllocator::transitionImageLayout(VkDevice dev, VkCommandBuffer cmdBuf, VkImage img, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout) {
    o_assert(dev && cmdBuf && img);

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
        0,                                     // dependencyFLags
        0,                                     // memoryBarrierCount
        nullptr,                               // pMemoryBarriers
        0,                                     // bufferMemoryBarrierCount
        nullptr,                               // pBufferMemoryBarriers
        1,                                     // imageMemoryBarrierCount
        pImgMemBarriers);                      // pImageMemoryBarriers
}

} // namespace _priv
} // namespace Oryol
