#pragma once
//------------------------------------------------------------------------------
/**
    @class vlkSyncPool
    @brief a pool for vulkan semaphores and fences
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"
#include "Gfx/vlk/vlk_impl.h"

namespace Oryol {
namespace _priv {

class vlkSyncPool {
public:
    /// destructor
    ~vlkSyncPool();

    /// setup the pool (pre-allocates fences and semaphores)
    void setup(VkDevice dev);
    /// discard the pool
    void discard(VkDevice dev);
    
    /// allocate a semaphore from the pool
    VkSemaphore allocSemaphore();
    /// put semaphore back into pool
    void freeSemaphore(VkSemaphore);

    /// allocate a fence from the pool
    VkFence allocFence();
    /// put a fence back into the pool
    void freeFence(VkFence);

private:
    static const int MaxNumSemaphores = 16;
    static const int MaxNumFences = 16;
    Array<VkSemaphore> semaphores;
    Array<VkFence> fences;
    bool valid = false;
};

} // namespace _priv
} // namespace Oryol
