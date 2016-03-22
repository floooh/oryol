#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkContext
    @brief Vulkan instance, device, swap-chain helpers
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/vlk/vlk_impl.h"
#include "Gfx/vlk/vlkConfig.h"

namespace Oryol {
namespace _priv {

class vlkContext {
public:
    /// destructor
    ~vlkContext();
    /// setup instance and device
    void setup(const GfxSetup& setup, const Array<const char*>& reqestedInstanceExtensions);
    /// setup the swap chain
    DisplayAttrs setupDeviceAndSwapChain(const GfxSetup& setup, const DisplayAttrs& attrs, VkSurfaceKHR surf);
    /// discard everything
    void discard();

    VkInstance Instance = nullptr;
    VkPhysicalDevice PhysicalDevice = nullptr;
    VkDevice Device = nullptr;
    VkSurfaceKHR Surface = nullptr;
    VkQueue Queue = nullptr; 
    VkSwapchainKHR SwapChain = nullptr;
    VkRenderPass RenderPass = nullptr;
    static const int MaxNumBuffers = 4;
    uint32 NumBuffers = 0;
    VkFramebuffer Framebuffers[MaxNumBuffers];

private:
    /// enumerate available instance layers, and find requested layers
    void setupInstanceLayers(const Array<const char*>& requestedLayers);
    /// discard instance layer list
    void discardInstanceLayers();
    /// enumerate available instance extensions, and find requested extensions
    void setupInstanceExtensions(const Array<const char*>& requestedExtensions);
    /// discard instance extension list
    void discardInstanceExtensions();
    /// setup the error reporting callback
    #if ORYOL_DEBUG
    void setupErrorReporting();
    #endif
    /// discard the error reporting callback
    #if ORYOL_DEBUG
    void discardErrorReporting();
    #endif
    /// setup the Vulkan instance
    void setupInstance(const GfxSetup& setup);
    /// discard the Vulkan instance
    void discardInstance();
    /// enumerate available physical devices, and pick the 'best'
    void setupPhysicalDevice(const GfxSetup& setup);
    /// discard physics devices
    void discardPhysicalDevice();
    /// enumerate available device layers, and select requested layers
    void setupDeviceLayers(const Array<const char*>& requestedLayers);
    /// discard device layers list
    void discardDeviceLayers();
    /// enumerate available device extensions, and selected requested extensions
    void setupDeviceExtensions(const Array<const char*>& requestedExtensions);
    /// discard device extension list
    void discardDeviceExtensions();
    /// setup device queue family list
    void setupQueueFamilies();
    /// discard device queue family list
    void discardQueueFamilies();
    /// initialize the graphics and present queue indices
    void initQueueIndices(VkSurfaceKHR surf);
    /// setup the logical device
    void setupDevice();
    /// discard the logical device and KHR surface
    void discardDeviceAndSurface();
    /// setup the supported surface formats
    void setupSurfaceFormats(const GfxSetup& setup);
    /// discard the surface format list
    void discardSurfaceFormats();
    /// setup the command pool and command buffers
    void setupCommandPoolAndBuffers();
    /// discard the command pool and command buffers
    void discardCommandPoolAndBuffers();
    /// setup the actual swap chain
    DisplayAttrs setupSwapchain(const GfxSetup& setup, const DisplayAttrs& attrs);
    /// discard the swap chain, will not destroy actual swap chain if this is a resize
    void discardSwapchain(bool forResize);
    /// setup the optional depth/stencil buffer
    void setupDepthBuffer(const GfxSetup& setup, const DisplayAttrs& attrs);
    /// discard the depth/stencil buffer
    void discardDepthBuffer();
    /// setup default render pass
    void setupRenderPass(const GfxSetup& setup);
    /// discard default render pass
    void discardRenderPass();
    /// setup framebuffers
    void setupFramebuffers(const DisplayAttrs& attrs);
    /// discard framebuffers
    void discardFramebuffers();

    /// find instance or device layer index, return InvalidIndex if not supported
    static int findLayer(const char* name, const Array<VkLayerProperties>& layers);
    /// dump layer information to console
    static void dumpLayerInfo(const char* title, const Array<VkLayerProperties>& layers);
    /// select requested layers from available layers
    static Array<const char*> selectLayers(const Array<const char*>& reqLayers, const Array<VkLayerProperties>& availLayers);

    /// find instance or device extension index, return InvalidIndex if not supported
    static int findExtension(const char* name, const Array<VkExtensionProperties>& exts);
    /// dump extension information to console
    static void dumpExtensionInfo(const char* title, const Array<VkExtensionProperties>& exts);
    /// select requested extensions from available extensions
    static Array<const char*> selectExtensions(const Array<const char*>& reqExts, const Array<VkExtensionProperties>& availExts);

    /// find matching memory type index from memory properties, return InvalidIndex if no match
    int findMemoryType(uint32 typeBits, VkFlags requirementsMask);

    /// transition image layout from old to new state
    void transitionImageLayout(VkImage img, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout);

    /// begin writing commands to current command buffer
    void beginCmdBuffer();
    /// submit commands in current command buffer
    void submitCmdBuffer();
    /// get current command buffer for this frame
    VkCommandBuffer curCmdBuffer() {
        return this->cmdBuffers[this->curFrameRotateIndex];
    };

    Array<VkLayerProperties> instLayers;
    Array<VkExtensionProperties> instExtensions;
    Array<VkPhysicalDevice> physDevices;
    VkPhysicalDeviceMemoryProperties memoryProps = { };
    Array<VkLayerProperties> devLayers;
    Array<VkExtensionProperties> devExtensions;
    Array<VkQueueFamilyProperties> queueProps;
    int graphicsQueueIndex = InvalidIndex;
    int presentQueueIndex = InvalidIndex;
    Array<VkSurfaceFormatKHR> surfaceFormats;
    VkFormat format = VK_FORMAT_MAX_ENUM;
    VkColorSpaceKHR colorSpace = VK_COLORSPACE_MAX_ENUM;
    VkCommandPool cmdPool = nullptr;
    VkCommandBuffer cmdBuffers[vlkConfig::NumFrames] = { };
    int curFrameRotateIndex = 0;
    struct SwapChainBuffer {
        VkImage image = nullptr;
        VkImageView view = nullptr;
    };
    SwapChainBuffer swapChainBuffers[MaxNumBuffers];
    VkFramebuffer frameBuffers[MaxNumBuffers] = { };
    struct DepthBuffer {
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkImage image = nullptr;
        VkDeviceMemory mem = nullptr;
        VkImageView view= nullptr;
    };
    DepthBuffer depthBuffer;

    Array<const char*> selInstLayers;
    Array<const char*> selDevLayers;
    Array<const char*> selInstExtensions;
    Array<const char*> selDevExtensions;

    #ifdef ORYOL_DEBUG
    VkDebugReportCallbackEXT debugReportCallback = nullptr;
    #endif
};

} // namespace _priv
} // namespace Oryol

