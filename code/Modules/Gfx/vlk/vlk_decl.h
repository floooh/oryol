#pragma once
//------------------------------------------------------------------------------
/**
    @file vlk_decl.h
    @brief Vulkan forward declarations
*/

#ifndef __vulkan_h_
#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;
#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;
#else
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#endif

VK_DEFINE_HANDLE(VkInstance)
VK_DEFINE_HANDLE(VkPhysicalDevice)
VK_DEFINE_HANDLE(VkDevice)
VK_DEFINE_HANDLE(VkQueue)

typedef void VkLayerProperties;
typedef void VkExtensionProperties;

typedef struct VkAllocationCallbacks {
    void*                                   pUserData;
    void (*pfnAllocation)();
    void (*pfnReallocation)();
    void (*pfnFree)();
    void (*pfnInternalAllocation)();
    void (*pfnInternalFree)();
} VkAllocationCallbacks;
#endif // __vulkan_h_