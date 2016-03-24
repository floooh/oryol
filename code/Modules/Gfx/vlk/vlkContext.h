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
    /// perform setup before GLFW window has been opened
    void setupBeforeWindow(const Array<const char*>& layers, const Array<const char*>& exts);
    /// perform setup after GLFW window has been opened
    DisplayAttrs setupAfterWindow(const GfxSetup& setup, const DisplayAttrs& attrs, const Array<const char*>& layers, const Array<const char*>& exts, VkSurfaceKHR surf);
    /// discard everything
    void discard();

    /// start a new frame
    void beginFrame();
    /// present the current swap chain image and do frame sync
    void present();
    /// begin writing commands to current command buffer
    VkCommandBuffer beginCmdBuffer();
    /// submit commands in current command buffer
    void submitCmdBuffer();
    /// get current command buffer for this frame
    VkCommandBuffer curCmdBuffer() const;
    /// get current default framebuffer for this frame
    VkFramebuffer curFramebuffer() const;
    /// get current swapchain image
    VkImage curSwapChainImage() const;
    /// get current present complete semaphore
    VkSemaphore curPresentCompleteSemaphore() const;
    /// transition image layout from old to new state
    void transitionImageLayout(VkImage img, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout);

    VkInstance Instance = nullptr;
    VkPhysicalDevice GPU = nullptr;
    VkDevice Device = nullptr;
    VkSurfaceKHR Surface = nullptr;
    VkQueue Queue = nullptr; 
    VkSwapchainKHR SwapChain = nullptr;
    VkRenderPass RenderPass = nullptr;

private:
    /// setup the Vulkan instance
    void setupInstance(const Array<const char*>& layers, const Array<const char*>& exts);
    /// discard the Vulkan instance
    void discardInstance();
    /// setup the physical device
    void setupGPU();
    /// discard the physical device
    void discardGPU();
    /// setup the logical device
    void setupDevice(const Array<const char*>& layers, const Array<const char*>& exts);
    /// discard the logical device
    void discardDevice();
    /// setup the swapchain and default framebuffer
    DisplayAttrs setupSwapchain(const GfxSetup& setup, const DisplayAttrs& attrs);
    /// discard swapchain and default framebuffer
    void discardSwapchain(bool forResize);

    /// find instance or device layer index, return InvalidIndex if not supported
    static int findLayer(const char* name, const Array<VkLayerProperties>& layers);
    /// select requested layers from available layers
    static Array<const char*> selectLayers(const Array<const char*>& reqLayers, const Array<VkLayerProperties>& availLayers);
    /// find instance or device extension index, return InvalidIndex if not supported
    static int findExtension(const char* name, const Array<VkExtensionProperties>& exts);
    /// select requested extensions from available extensions
    static Array<const char*> selectExtensions(const Array<const char*>& reqExts, const Array<VkExtensionProperties>& availExts);
    /// find matching memory type index from memory properties, return InvalidIndex if no match
    int findMemoryType(uint32 typeBits, VkFlags requirementsMask);

    VkPhysicalDeviceMemoryProperties memoryProps = { };
    int graphicsQueueIndex = InvalidIndex;
    int presentQueueIndex = InvalidIndex;
    VkFormat pixelFormat = VK_FORMAT_MAX_ENUM;
    VkColorSpaceKHR colorSpace = VK_COLORSPACE_MAX_ENUM;
    VkCommandPool cmdPool = nullptr;
    VkCommandBuffer cmdBuffers[vlkConfig::NumFrames] = { };
    struct SwapChainBuffer {
        VkFramebuffer framebuffer = nullptr;
        VkImage image = nullptr;
        VkImageView view = nullptr;
        VkSemaphore semaphore = nullptr;
    };
    static const int NumSwapChainBuffers = vlkConfig::NumFrames;
    uint32 curBufferIndex = 0;  // both swapchain and command buffer index
    SwapChainBuffer swapChainBuffers[NumSwapChainBuffers];
    struct DepthBuffer {
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkImage image = nullptr;
        VkDeviceMemory mem = nullptr;
        VkImageView view= nullptr;
    };
    DepthBuffer depthBuffer;
    #ifdef ORYOL_DEBUG
    VkDebugReportCallbackEXT debugReportCallback = nullptr;
    #endif
};

//------------------------------------------------------------------------------
inline VkCommandBuffer 
vlkContext::curCmdBuffer() const {
    return this->cmdBuffers[this->curBufferIndex];
};

//------------------------------------------------------------------------------
inline VkFramebuffer 
vlkContext::curFramebuffer() const {
    return this->swapChainBuffers[this->curBufferIndex].framebuffer;
}

//------------------------------------------------------------------------------
inline VkImage 
vlkContext::curSwapChainImage() const {
    return this->swapChainBuffers[this->curBufferIndex].image;
}

//------------------------------------------------------------------------------
inline VkSemaphore 
vlkContext::curPresentCompleteSemaphore() const {
    return this->swapChainBuffers[this->curBufferIndex].semaphore;
}

} // namespace _priv
} // namespace Oryol

