#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkResAllocator
    @brief create and destroy Vulkan resources
    
    Also see d3d12ResAllocator class!
*/
#include "Core/Types.h"
#include "Core/Containers/Queue.h"
#include "Gfx/vlk/vlk_decl.h"

namespace Oryol {
namespace _priv {

class vlkResAllocator {
public:
    /// a bufffer object handle with associated memory
    struct BufferItem {
        VkBuffer buffer = nullptr;
        VkDeviceMemory memory = nullptr;
    };

    /// destructor
    ~vlkResAllocator();

    /// setup the resource allocator
    void setup(VkPhysicalDevice gpu);
    /// discard the resource allocator
    void discard(VkDevice dev, VkCommandPool cmdPool);
    /// garbage-collect released resources when safe (no longer used by GPU)
    void garbageCollect(VkDevice dev, VkCommandPool cmdPool, uint64 frameIndex);
    /// find matching memory type index from memory properties, return InvalidIndex if no match
    int findMemoryType(uint32 typeBits, VkFlags requirementsMask);

    /// allocate a new command buffer
    VkCommandBuffer allocCommandBuffer(VkDevice dev, VkCommandPool cmdPool);
    /// defer-release command buffer
    void releaseCommandBuffer(uint64 frameIndex, VkCommandBuffer cmdBuf);

    /// allocate a generic buffer object in device memory (transfer-dst usage bit will be added)
    BufferItem allocDeviceBuffer(VkDevice dev, VkBufferUsageFlags usage, uint32 size);
    /// allocate a staging buffer object and optionally copy data into it
    BufferItem allocStagingBuffer(VkDevice dev, uint32 size);
    /// allocate a buffer object and optionally fill it with data
    BufferItem allocStaticBuffer(VkDevice dev, VkCommandBuffer cmdBuf, uint64 frameIndex, VkBufferUsageFlags usage, const void* data, uint32 size);
    /// release a buffer object
    void releaseBuffer(uint64 frameIndex, const BufferItem& item);

    /// copy data into device-memory buffer using a staging buffer
    void copyBufferData(VkDevice dev, VkCommandBuffer cmdBuf, uint64 frameIndex, VkBufferUsageFlags dstUsage, VkBuffer dstBuf, VkBuffer optStagingBuf, VkDeviceMemory optStagingMem, const void* data, uint32 size);

    /// convert buffer usage into access flags
    static VkAccessFlags bufferUsageToAccess(VkBufferUsageFlags usage);
    /// do an buffer access state transition
    static void transitionBufferAccess(VkCommandBuffer cmdBuf, VkBuffer buf, VkAccessFlags oldAccess, VkAccessFlags newAccess);
    /// do an image layout state transition
    static void transitionImageLayout(VkCommandBuffer cmdBuf, VkImage img, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout);

private:
    struct freeItem {            
        uint64 frameIndex;
        uint64 res;
        VkDeviceMemory mem;     // optional
        VkFence fence;          // optional
        enum type_t {
            CommandBuffer,
            Buffer,
            InvalidType,
        } type;
        
        freeItem() : frameIndex(), res(0), mem(nullptr), fence(nullptr), type(InvalidType) { };
        freeItem(uint64 frameIndex_, uint64 res_, type_t type_) :
            frameIndex(frameIndex_), res(res_), mem(nullptr), fence(nullptr), type(type_) { };
        freeItem(uint64 frameIndex_, uint64 res_, VkFence fence_, type_t type_) :
            frameIndex(frameIndex_), res(res_), mem(nullptr), fence(fence_), type(type_) { };
        freeItem(uint64 frameIndex_, uint64 res_, VkDeviceMemory mem_, type_t type_) :
            frameIndex(frameIndex_), res(res_), mem(mem_), fence(nullptr), type(type_) { };
    };
    /// generic call to destroy a resource
    void destroy(VkDevice dev, VkCommandPool cmdPool, const freeItem& item);
    /// destroy a VkCommandBuffer object
    void destroyCommandBuffer(VkDevice dev, VkCommandPool cmdPool, VkCommandBuffer cmdBuf);
    /// destroy a VkBuffer object and associated memory
    void destroyBuffer(VkDevice dev, VkBuffer buf, VkDeviceMemory mem);

    bool valid = false;
    VkPhysicalDeviceMemoryProperties memoryProps = { };
    Queue<freeItem> releaseQueue;
};

} // namespace _priv
} // namespace Oryol
