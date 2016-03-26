//------------------------------------------------------------------------------
//  vlkSyncPool.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "vlk_impl.h"
#include "vlkSyncPool.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkSyncPool::~vlkSyncPool() {
    o_assert(!this->valid);
}

//------------------------------------------------------------------------------
void
vlkSyncPool::setup(VkDevice dev) {
    o_assert(!this->valid);
    this->valid = true;

    VkResult err;
    VkSemaphoreCreateInfo semInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    this->semaphores.Resize(MaxNumSemaphores, nullptr);
    for (int i = 0; i < MaxNumSemaphores; i++) {
        err = vkCreateSemaphore(dev, &semInfo, nullptr, &this->semaphores[i]);
        o_assert(!err && this->semaphores[i]);
    }
    VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    this->fences.Resize(MaxNumFences, nullptr);
    for (int i = 0; i < MaxNumFences; i++) {
        err = vkCreateFence(dev, &fenceInfo, nullptr, &this->fences[i]);
        o_assert(!err && this->fences[i]);
    }
}

//------------------------------------------------------------------------------
void
vlkSyncPool::discard(VkDevice dev) {
    o_assert(this->valid);
    this->valid = false;
    for (VkSemaphore sem : this->semaphores) {
        vkDestroySemaphore(dev, sem, nullptr);
    }
    this->semaphores.Clear();
    for (VkFence fence : this->fences) {
        vkDestroyFence(dev, fence, nullptr);
    }
    this->fences.Clear();
}

//------------------------------------------------------------------------------
VkSemaphore
vlkSyncPool::allocSemaphore() {
    return this->semaphores.PopBack();
}

//------------------------------------------------------------------------------
void
vlkSyncPool::freeSemaphore(VkSemaphore sem) {
    this->semaphores.Add(sem);
}

//------------------------------------------------------------------------------
VkFence
vlkSyncPool::allocFence() {
    return this->fences.PopBack();
}

//------------------------------------------------------------------------------
void
vlkSyncPool::freeFence(VkFence fence) {
    this->fences.Add(fence);
}

} // namespace _priv
} // namespace Oryol
