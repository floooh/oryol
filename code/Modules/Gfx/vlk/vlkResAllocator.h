#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkResAllocator
    @brief create and destroy Vulkan resources
*/
#include "Core/Types.h"
#include "Core/Containers/Queue.h"
#include "Gfx/vlk/vlk_decl.h"

namespace Oryol {
namespace _priv {

class vlkResAllocator {
public:
    /// destructor
    ~vlkResAllocator();

    /// discard resource allocator
    void destroyAll(VkDevice dev, VkCommandPool cmdPool);
    /// garbage-collect released resources when safe (no longer used by GPU)
    void garbageCollect(VkDevice dev, VkCommandPool cmdPool, uint64 frameIndex);

    /// allocate a new command buffer
    VkCommandBuffer allocCommandBuffer(VkDevice dev, VkCommandPool cmdPool);
    /// defer-release command buffer
    void releaseCommandBuffer(uint64 frameIndex, VkCommandBuffer cmdBuf);
    
    /// do an image layout state transition
    static void transitionImageLayout(VkDevice dev, VkCommandBuffer cmdBuf, VkImage img, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout);

private:
    struct freeItem {            
        uint64 frameIndex;
        uint64 res;
        VkFence fence;      // optional
        enum type_t {
            CommandBuffer,
            InvalidType,
        } type;
        
        freeItem() : frameIndex(), res(0), fence(nullptr), type(InvalidType) { };
        freeItem(uint64 frameIndex_, uint64 res_, type_t type_) :
            frameIndex(frameIndex_), res(res_), fence(nullptr), type(type_) { };
        freeItem(uint64 frameIndex_, uint64 res_, VkFence fence_, type_t type_) :
            frameIndex(frameIndex_), res(res_), fence(fence_), type(type_) { };
    };
    /// generic call to destroy a resource
    void destroy(VkDevice dev, VkCommandPool cmdPool, const freeItem& item);
    /// destroy a VkCommandBuffer object
    void destroyCommandBuffer(VkDevice dev, VkCommandPool cmdPool, VkCommandBuffer cmdBuf);

    Queue<freeItem> releaseQueue;
};

} // namespace _priv
} // namespace Oryol
