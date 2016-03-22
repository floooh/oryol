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
VK_DEFINE_HANDLE(VkCommandBuffer)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkImage)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkImageView)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSurfaceKHR)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkCommandPool)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDebugReportCallbackEXT)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSwapchainKHR)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDeviceMemory)

typedef uint32_t VkFlags;
typedef VkFlags VkImageAspectFlags;
typedef enum VkImageLayout vkImageLayout;

typedef void VkLayerProperties;
typedef void VkExtensionProperties;
typedef void VkQueueFamilyProperties;

typedef enum VkPhysicalDeviceType VkPhysicalDeviceType;
typedef struct VkSurfaceFormatKHR VkSurfaceFormatKHR;
typedef enum VkFormat {
    VK_FORMAT_MAX_ENUM = 0x7FFFFFFF
} VkFormat;
typedef enum VkColorSpaceKHR {
    VK_COLORSPACE_MAX_ENUM = 0x7FFFFFFF
} VkColorSpaceKHR;

typedef struct VkAllocationCallbacks {
    void*                                   pUserData;
    void (*pfnAllocation)();
    void (*pfnReallocation)();
    void (*pfnFree)();
    void (*pfnInternalAllocation)();
    void (*pfnInternalFree)();
} VkAllocationCallbacks;

#endif // __vulkan_h_
