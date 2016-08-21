#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkResAllocator
    @brief create and destroy Vulkan resources
    
    Also see d3d12ResAllocator class!
*/
#include "Core/Types.h"
#include "Core/Containers/Queue.h"
#include "Gfx/vlk/vlk_impl.h"

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
    void garbageCollect(VkDevice dev, VkCommandPool cmdPool, uint64_t frameIndex);
    /// find matching memory type index from memory properties, return InvalidIndex if no match
    int findMemoryType(uint32_t typeBits, VkFlags requirementsMask);

    /// allocate a generic buffer object in device memory (transfer-dst usage bit will be added)
    BufferItem allocDeviceBuffer(VkDevice dev, VkBufferUsageFlags usage, uint32_t size);
    /// allocate a staging buffer object and optionally copy data into it
    BufferItem allocStagingBuffer(VkDevice dev, VkBufferUsageFlags usage, uint32_t size);
    /// allocate a buffer object and optionally fill it with data
    BufferItem allocStaticBuffer(VkDevice dev, VkCommandBuffer cmdBuf, uint64_t frameIndex, VkBufferUsageFlags usage, const void* data, uint32_t size);
    /// release a buffer object
    void releaseBuffer(uint64_t frameIndex, const BufferItem& item);
    /// release a pipeline object
    void releasePipeline(uint64_t frameIndex, VkPipeline pipeline);

    /// copy data into device-memory buffer using a staging buffer
    void copyBufferData(VkDevice dev, VkCommandBuffer cmdBuf, uint64_t frameIndex, VkBufferUsageFlags dstUsage, VkBuffer dstBuf, VkBuffer optStagingBuf, VkDeviceMemory optStagingMem, const void* data, uint32_t size);

    /// convert buffer usage into access flags
    static VkAccessFlags bufferUsageToAccess(VkBufferUsageFlags usage);
    /// do an buffer access state transition
    static void transitionBufferAccess(VkCommandBuffer cmdBuf, VkBuffer buf, VkAccessFlags oldAccess, VkAccessFlags newAccess);
    /// do an image layout state transition
    static void transitionImageLayout(VkCommandBuffer cmdBuf, VkImage img, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout);

private:
    struct freeItem {            
        uint64_t frameIndex;
        uint64_t res;
        VkDeviceMemory mem;     // optional
        VkFence fence;          // optional
        enum type_t {
            Buffer,
            Pipeline,
            InvalidType,
        } type;
        
        freeItem() : frameIndex(), res(0), mem(nullptr), fence(nullptr), type(InvalidType) { };
        freeItem(uint64_t frameIndex_, uint64_t res_, type_t type_) :
            frameIndex(frameIndex_), res(res_), mem(nullptr), fence(nullptr), type(type_) { };
        freeItem(uint64_t frameIndex_, uint64_t res_, VkFence fence_, type_t type_) :
            frameIndex(frameIndex_), res(res_), mem(nullptr), fence(fence_), type(type_) { };
        freeItem(uint64_t frameIndex_, uint64_t res_, VkDeviceMemory mem_, type_t type_) :
            frameIndex(frameIndex_), res(res_), mem(mem_), fence(nullptr), type(type_) { };
    };
    /// generic call to destroy a resource
    void destroy(VkDevice dev, VkCommandPool cmdPool, const freeItem& item);
    /// destroy a VkBuffer object and associated memory
    void destroyBuffer(VkDevice dev, VkBuffer buf, VkDeviceMemory mem);

    bool valid = false;
    VkPhysicalDeviceMemoryProperties memoryProps = { };
    Queue<freeItem> releaseQueue;
};

} // namespace _priv
} // namespace Oryol
