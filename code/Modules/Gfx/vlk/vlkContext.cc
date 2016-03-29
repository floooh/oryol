//------------------------------------------------------------------------------
//  vlkContext.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlk_impl.h"
#include "vlkContext.h"
#include "vlkTypes.h"

static PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
static PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR = nullptr;
static PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR = nullptr;
static PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR = nullptr;
static PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR = nullptr;
static PFN_vkQueuePresentKHR fpQueuePresentKHR = nullptr;

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkContext::~vlkContext() {
    o_assert(!this->Instance);
    o_assert(!this->GPU);
    o_assert(!this->Device);
    o_assert(!this->Surface);
    o_assert(!this->Queue);
    o_assert(!this->SwapChain);
    o_assert(!this->RenderPass);
}

//------------------------------------------------------------------------------
void
vlkContext::setupBeforeWindow(const Array<const char*>& layers, const Array<const char*>& exts) {
    o_assert(nullptr == this->Instance);
    o_assert(nullptr == this->GPU);
    this->setupInstance(layers, exts);
    this->setupGPU();
}

//------------------------------------------------------------------------------
DisplayAttrs
vlkContext::setupAfterWindow(const GfxSetup& setup, const DisplayAttrs& attrs, const Array<const char*>& layers, const Array<const char*>& exts, VkSurfaceKHR surf) {
    o_assert(!this->Surface);
    o_assert(surf);

    this->Surface = surf;
    this->setupDevice(layers, exts);
    DisplayAttrs outAttrs = this->setupSwapchain(setup, attrs);

    return outAttrs;
}

//------------------------------------------------------------------------------
void
vlkContext::discard() {
    o_assert(this->Device);
    vkDeviceWaitIdle(this->Device);
    this->discardSwapchain(false);
    this->ResAllocator.discard(this->Device, this->cmdPool);
    this->discardDevice();
    this->discardGPU();
    this->discardInstance();
}

//------------------------------------------------------------------------------
void
vlkContext::beginCmdBuffer() {
    o_assert_dbg(!this->commandBuffer);
    o_assert_dbg(this->Device);
    const auto& frame = this->frameDatas[this->CurFrameRotateIndex];
    this->commandBuffer = frame.cmdBuf;
    o_assert_dbg(this->commandBuffer);
    if (frame.cmdBufDoneFenceSet) {
        vkWaitForFences(this->Device, 1, &frame.cmdBufDoneFence, VK_TRUE, UINT64_MAX);
        vkResetFences(this->Device, 1, &frame.cmdBufDoneFence);
    }
    VkCommandBufferBeginInfo bgnInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    VkResult err = vkBeginCommandBuffer(this->commandBuffer, &bgnInfo);
    o_assert(!err);
}

//------------------------------------------------------------------------------
void
vlkContext::submitCmdBuffer(VkPipelineStageFlags waitDstStageMask, VkSemaphore waitSem, VkSemaphore doneSem) {
    o_assert(this->commandBuffer);

    VkResult err = vkEndCommandBuffer(this->commandBuffer);
    o_assert(!err);
    auto& frame = this->frameDatas[this->CurFrameRotateIndex];
    o_assert_dbg(this->commandBuffer == frame.cmdBuf);

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    if (waitSem) {
        o_assert_dbg(0 != waitDstStageMask);
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSem;
        submitInfo.pWaitDstStageMask = &waitDstStageMask;
    }
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->commandBuffer;
    if (doneSem) {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &doneSem;
    }
    err = vkQueueSubmit(this->Queue, 1, &submitInfo, frame.cmdBufDoneFence);
    o_assert(!err);
    frame.cmdBufDoneFenceSet = true;

    this->commandBuffer = nullptr;
}

//------------------------------------------------------------------------------
VkCommandBuffer
vlkContext::beginFrame() {
    o_assert_dbg(this->Device);
    o_assert_dbg(this->SwapChain);
    o_assert_dbg(fpAcquireNextImageKHR);
    VkResult err;

    // wait on previous frame-rotate-fence for v-sync
    auto& curFrame = this->frameDatas[this->CurFrameRotateIndex];
    if (curFrame.presentFenceSet) {
        vkWaitForFences(this->Device, 1, &curFrame.presentFence, VK_TRUE, UINT64_MAX);
        vkResetFences(this->Device, 1, &curFrame.presentFence);
    }

    // get new semaphores
    VkSemaphore imageAcquiredSemaphore = this->SyncPool.allocSemaphore();
    VkSemaphore renderingFinishedSemaphore = this->SyncPool.allocSemaphore();

    // get next available buffer index from the swap chain
    err = fpAcquireNextImageKHR(this->Device, 
        this->SwapChain,            // swapchain
        UINT64_MAX,                 // timeout
        imageAcquiredSemaphore,     // semaphore (signaled when presentation engine has released ownership of image)
        curFrame.presentFence,      // fence (same as semaphore)
        &this->curSwapChainBufferIndex);
    o_assert(!err && (this->curSwapChainBufferIndex >= 0));
    curFrame.presentFenceSet = true;

    // free old and set new semaphore handles
    auto& curBuf = this->swapChainBuffers[this->curSwapChainBufferIndex];
    if (curBuf.imageAcquiredSemaphore) {
        this->SyncPool.freeSemaphore(curBuf.imageAcquiredSemaphore);
    }
    if (curBuf.renderingFinishedSemaphore) {
        this->SyncPool.freeSemaphore(curBuf.renderingFinishedSemaphore);
    }
    curBuf.imageAcquiredSemaphore = imageAcquiredSemaphore;
    curBuf.renderingFinishedSemaphore = renderingFinishedSemaphore;

    if (!this->commandBuffer) {
        this->beginCmdBuffer();
    }
    return this->commandBuffer;
}

//------------------------------------------------------------------------------
void
vlkContext::present() {
    o_assert_dbg(this->Device);
    o_assert_dbg(this->Queue);
    o_assert_dbg(this->SwapChain);
    o_assert_dbg(this->commandBuffer);

    // submit (and free) the command buffer
    const auto& curBuf = this->swapChainBuffers[this->curSwapChainBufferIndex];
    this->submitCmdBuffer(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, curBuf.imageAcquiredSemaphore, curBuf.renderingFinishedSemaphore);

    // present the frame
    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &curBuf.renderingFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &this->SwapChain;
    presentInfo.pImageIndices = &this->curSwapChainBufferIndex;
    VkResult err = fpQueuePresentKHR(this->Queue, &presentInfo);
    o_assert(!err); // FIXME (swap chain may have been lost)

    // bump frame counters
    this->CurFrameIndex = this->CurFrameIndex + 1;
    this->CurFrameRotateIndex = this->CurFrameIndex % vlkConfig::NumFrames;

    // defer-release expired resources
    this->ResAllocator.garbageCollect(this->Device, this->cmdPool, this->CurFrameIndex);
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstance(const Array<const char*>& reqLayers, const Array<const char*>& reqExts) {
    o_assert(!this->Instance);

    // query instance layers and extensions
    Array<const char*> instanceLayers = this->Ext.queryInstanceLayers(reqLayers);
    Array<const char*> instanceExtensions = this->Ext.queryInstanceExtensions(reqExts);

    // setup instance
    VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName = "ORYOL_APP";    // FIXME
    appInfo.applicationVersion = 0;
    appInfo.pEngineName = "ORYOL";
    appInfo.engineVersion = 0;
    appInfo.apiVersion = VK_API_VERSION;
    VkInstanceCreateInfo instInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledLayerCount = instanceLayers.Size();
    instInfo.ppEnabledLayerNames = instanceLayers.Data();
    instInfo.enabledExtensionCount = instanceExtensions.Size();
    instInfo.ppEnabledExtensionNames = instanceExtensions.Data();    
    VkResult err = vkCreateInstance(&instInfo, nullptr, &this->Instance);
    o_assert(!err && this->Instance);

    // lookup instance functions
    INST_FUNC_PTR(this->Instance, GetPhysicalDeviceSurfaceSupportKHR);
    INST_FUNC_PTR(this->Instance, GetPhysicalDeviceSurfaceFormatsKHR);
    INST_FUNC_PTR(this->Instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    INST_FUNC_PTR(this->Instance, GetPhysicalDeviceSurfacePresentModesKHR);
    INST_FUNC_PTR(this->Instance, CreateSwapchainKHR);
    INST_FUNC_PTR(this->Instance, DestroySwapchainKHR);
    INST_FUNC_PTR(this->Instance, GetSwapchainImagesKHR);
    INST_FUNC_PTR(this->Instance, AcquireNextImageKHR);
    INST_FUNC_PTR(this->Instance, QueuePresentKHR);

    // setup error reporting
    this->Ext.setupDebugReporting(this->Instance);
}

//------------------------------------------------------------------------------
void
vlkContext::discardInstance() {
    o_assert(this->Instance);
    this->Ext.discardDebugReporting(this->Instance);
    vkDestroyInstance(this->Instance, nullptr);
    this->Instance = nullptr;
}

//------------------------------------------------------------------------------
void
vlkContext::setupGPU() {
    o_assert(this->Instance);
    o_assert(!this->GPU);

    // enumerate GPUs in system
    Array<VkPhysicalDevice> gpus;
    uint32_t num;
    VkResult err = vkEnumeratePhysicalDevices(this->Instance, &num, nullptr);
    o_assert(!err);
    if (num > 0) {
        gpus.Resize(num);
        err = vkEnumeratePhysicalDevices(this->Instance, &num, gpus.Data());
    }
    else {
        o_error("No Vulkan-enabled GPUs in system!\n");
    }

    // dump device info, and also pick first dedicated GPU
    int firstDiscreteGPU = InvalidIndex;
    VkPhysicalDeviceProperties props = { };
    for (int i = 0; i < gpus.Size(); i++) {
        vkGetPhysicalDeviceProperties(gpus[i], &props);
        Log::Info("GPU %d: %s (type: %s)\n", i, props.deviceName, vlkTypes::physicalDeviceTypeAsString(props.deviceType));
        if ((InvalidIndex == firstDiscreteGPU) &&
            (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == props.deviceType)) {
            firstDiscreteGPU = i;
        }
    }
    
    // choose the 'best' physical device
    // FIXME: might want to have more control over this
    if (InvalidIndex != firstDiscreteGPU) {
        this->GPU = gpus[firstDiscreteGPU];
    }
    else {
        this->GPU = gpus[0];
    }

    // setup the resource allocator
    this->ResAllocator.setup(this->GPU);
}

//------------------------------------------------------------------------------
void
vlkContext::discardGPU() {
    o_assert(this->GPU);
    this->GPU = nullptr;
}

//------------------------------------------------------------------------------
void
vlkContext::setupDevice(const Array<const char*>& reqLayers, const Array<const char*>& reqExts) {
    o_assert(this->GPU);
    o_assert(this->Surface);
    o_assert(!this->Device);
    o_assert(!this->Queue);
    o_assert(InvalidIndex == this->graphicsQueueIndex);
    o_assert(InvalidIndex == this->presentQueueIndex);
    o_assert(!this->cmdPool);

    // enumerate and select queue families
    uint32_t num = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(this->GPU, &num, nullptr);
    o_assert(num >= 1);
    Array<VkQueueFamilyProperties> queueProps;
    queueProps.Resize(num);
    vkGetPhysicalDeviceQueueFamilyProperties(this->GPU, &num, queueProps.Data());

    // find a graphics and present queue, prefer one queue that supports both
    for (int queueIndex = 0; queueIndex < queueProps.Size(); queueIndex++) {
        const auto& cur = queueProps[queueIndex];
        if (cur.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            if (InvalidIndex == this->graphicsQueueIndex) {
                this->graphicsQueueIndex = queueIndex;
            }
            VkBool32 supportsPresent = VK_FALSE;
            fpGetPhysicalDeviceSurfaceSupportKHR(this->GPU, queueIndex, this->Surface, &supportsPresent);
            if (VK_TRUE == supportsPresent) {
                this->graphicsQueueIndex = queueIndex;
                this->presentQueueIndex = queueIndex;
                break;
            }
        }
    }
    if (InvalidIndex == this->presentQueueIndex) {
        // no queue which can do both graphics and present, find separate present queue
        o_warn("vlkContext: trying to find separate present queue\n");
        for (int queueIndex = 0; queueIndex < queueProps.Size(); queueIndex++) {
            VkBool32 supportsPresent = VK_FALSE;
            fpGetPhysicalDeviceSurfaceSupportKHR(this->GPU, queueIndex, this->Surface, &supportsPresent);
            if (VK_TRUE == supportsPresent) {
                this->presentQueueIndex = queueIndex;
                break;
            }
        }
    }
    if ((InvalidIndex == this->graphicsQueueIndex) || (InvalidIndex == this->presentQueueIndex)) {
        o_error("vlkContext: Could not find graphics or present queue\n");
    }
    // FIXME: handle separate graphics and present queue
    if (this->graphicsQueueIndex != this->presentQueueIndex) {
        o_error("vlkContext: separate graphics and present queue not supported\n");
    }

    // query device layers and extensions
    Array<const char*> devLayers = this->Ext.queryDeviceLayers(this->GPU, reqLayers);
    Array<const char*> devExtensions = this->Ext.queryDeviceExtensions(this->GPU, reqExts);

    // create logical device
    float queuePrios[] = { 1.0 };
    VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueCreateInfo.queueFamilyIndex = this->graphicsQueueIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePrios;
    VkDeviceCreateInfo devCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    devCreateInfo.queueCreateInfoCount = 1;
    devCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    devCreateInfo.enabledLayerCount = devLayers.Size();
    devCreateInfo.ppEnabledLayerNames = devLayers.Data();
    devCreateInfo.enabledExtensionCount = devExtensions.Size();
    devCreateInfo.ppEnabledExtensionNames = devExtensions.Data();
    devCreateInfo.pEnabledFeatures = nullptr;   // FIXME: enable optional features
    VkResult err = vkCreateDevice(this->GPU, &devCreateInfo, nullptr, &this->Device);
    o_assert(!err && this->Device);

    // setup semaphore and fence pool
    this->SyncPool.setup(this->Device);

    // get graphics queue handle
    vkGetDeviceQueue(this->Device, this->graphicsQueueIndex, 0, &this->Queue);
    
    // create command pool
    VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = this->graphicsQueueIndex;
    err = vkCreateCommandPool(this->Device, &poolInfo, nullptr, &this->cmdPool);
    o_assert(!err && this->cmdPool);

    // create per-frame fences and command buffers
    for (auto& frame : this->frameDatas) {
        frame.presentFence = this->SyncPool.allocFence();
        frame.cmdBufDoneFence = this->SyncPool.allocFence();
        VkCommandBufferAllocateInfo bufInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        bufInfo.commandPool = this->cmdPool;
        bufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        bufInfo.commandBufferCount = 1;
        VkResult err = vkAllocateCommandBuffers(this->Device, &bufInfo, &frame.cmdBuf);
        o_assert(!err && frame.cmdBuf);
    }
}

//------------------------------------------------------------------------------
void
vlkContext::discardDevice() {
    o_assert(this->Instance);
    o_assert(this->Device);
    o_assert(this->Surface);
    o_assert(this->cmdPool);
    for (auto& frame : this->frameDatas) {
        this->SyncPool.freeFence(frame.presentFence);
        frame.presentFence = nullptr;
        this->SyncPool.freeFence(frame.cmdBufDoneFence);
        frame.cmdBufDoneFence = nullptr;
        vkFreeCommandBuffers(this->Device, cmdPool, 1, &frame.cmdBuf);
    }
    vkDestroyCommandPool(this->Device, this->cmdPool, nullptr);
    this->cmdPool = nullptr;
    this->SyncPool.discard(this->Device);
    vkDestroyDevice(this->Device, nullptr);
    this->Device = nullptr;
    vkDestroySurfaceKHR(this->Instance, this->Surface, nullptr);
    this->Surface = nullptr;
    this->Queue = nullptr;
}

//------------------------------------------------------------------------------
DisplayAttrs
vlkContext::setupSwapchain(const GfxSetup& setup, const DisplayAttrs& inAttrs) {
    o_assert(this->Instance);
    o_assert(this->GPU);
    o_assert(this->Surface);
    o_assert(VK_FORMAT_MAX_ENUM == this->pixelFormat);
    o_assert(VK_COLORSPACE_MAX_ENUM == this->colorSpace);    
    o_assert(!this->depthBuffer.image);
    o_assert(!this->depthBuffer.mem);
    o_assert(!this->depthBuffer.view);
    o_assert(VK_FORMAT_UNDEFINED == this->depthBuffer.format);
    o_assert(!this->RenderPass);
    
    // start recording commands
    this->beginCmdBuffer();

    // this will be modified with the actual framebuffer size, and returned
    DisplayAttrs attrs = inAttrs;

    // enumerate surface formats
    Array<VkSurfaceFormatKHR> surfaceFormats;
    uint32_t num = 0;
    VkResult err = fpGetPhysicalDeviceSurfaceFormatsKHR(this->GPU, this->Surface, &num, nullptr);
    o_assert(!err && (num > 0));
    surfaceFormats.Resize(num);
    err = fpGetPhysicalDeviceSurfaceFormatsKHR(this->GPU, this->Surface, &num, surfaceFormats.Data());
    o_assert(!err);

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if ((1 == surfaceFormats.Size()) && (VK_FORMAT_UNDEFINED == surfaceFormats[0].format)) {
        this->pixelFormat = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else {
        this->pixelFormat = surfaceFormats[0].format;
    }
    this->colorSpace = surfaceFormats[0].colorSpace;

    // NOTE: this will also destroy the previous swap chain (if one exists)
    // do NOT call discardSwapChain() during a resize operation, only
    // on final cleanup
    VkSurfaceCapabilitiesKHR surfCaps = {};
    err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(this->GPU, this->Surface, &surfCaps);
    o_assert(!err);
    o_assert(surfCaps.currentExtent.width != (uint32)-1);
    o_assert(surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
    o_assert(surfCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
    attrs.FramebufferWidth = surfCaps.currentExtent.width;
    attrs.FramebufferHeight = surfCaps.currentExtent.height;

    // FIXME: this doesn't seem to have any purpose except supressing a validation layer error(?)
    err = fpGetPhysicalDeviceSurfacePresentModesKHR(this->GPU, this->Surface, &num, nullptr);
    o_assert(!err && (num > 0));
    Array<VkPresentModeKHR> presentModes;
    presentModes.Resize(num);
    err = fpGetPhysicalDeviceSurfacePresentModesKHR(this->GPU, this->Surface, &num, presentModes.Data());
    o_assert(!err);

    // Determine the number of VkImage's to use in the swap chain (we desire to 
    // own only 1 image at a time, besides the images being displayed and
    // queued for display):
    uint32 desiredNumberOfSwapchainImages = vlkConfig::NumFrames;
    if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount)) {
        // Application must settle for fewer images than desired:
        desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
    }
    o_assert(desiredNumberOfSwapchainImages == vlkConfig::NumFrames);   // FIXME

    VkSwapchainKHR oldSwapChain = this->SwapChain;
    this->SwapChain = nullptr;

    VkSwapchainCreateInfoKHR swapChainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapChainInfo.surface = this->Surface;
    swapChainInfo.minImageCount = desiredNumberOfSwapchainImages;
    swapChainInfo.imageFormat = this->pixelFormat;
    swapChainInfo.imageColorSpace = this->colorSpace;
    swapChainInfo.imageExtent = surfCaps.currentExtent;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainInfo.queueFamilyIndexCount = 0;
    swapChainInfo.pQueueFamilyIndices = nullptr;
    swapChainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapChainInfo.oldSwapchain = oldSwapChain;
    swapChainInfo.clipped = true;
    err = fpCreateSwapchainKHR(this->Device, &swapChainInfo, nullptr, &this->SwapChain);
    o_assert(!err && this->SwapChain);

    // If we just re-created an existing swapchain, we should destroy the old
    // swapchain at this point.
    // Note: destroying the swapchain also cleans up all its associated
    // presentable images once the platform is done with them.
    if (oldSwapChain) {
        fpDestroySwapchainKHR(this->Device, oldSwapChain, nullptr);
    }

    uint32 numBuffers = 0;
    err = fpGetSwapchainImagesKHR(this->Device, this->SwapChain, &numBuffers, nullptr);
    o_assert(!err && (numBuffers == vlkConfig::NumFrames));
    VkImage swapChainImages[vlkConfig::NumFrames];
    err = fpGetSwapchainImagesKHR(this->Device, this->SwapChain, &numBuffers, swapChainImages);
    o_assert(!err);
    for (uint32 i = 0; i < vlkConfig::NumFrames; i++) {
        auto& curBuf = this->swapChainBuffers[i];

        o_assert(!curBuf.image);
        curBuf.image = swapChainImages[i];

        // Render loop will expect image to have been used before and in
        // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        // layout and will change to COLOR_ATTACHMENT_OPTIMAL, so init the image
        // to that state
        vlkResAllocator::transitionImageLayout( 
            this->commandBuffer, 
            swapChainImages[i], 
            VK_IMAGE_ASPECT_COLOR_BIT, 
            VK_IMAGE_LAYOUT_UNDEFINED, 
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        o_assert(!curBuf.view);
        VkImageViewCreateInfo imgViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        imgViewCreateInfo.format = this->pixelFormat;
        imgViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        imgViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        imgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imgViewCreateInfo.flags = 0;
        imgViewCreateInfo.image = swapChainImages[i];
        err = vkCreateImageView(this->Device, &imgViewCreateInfo, nullptr, &curBuf.view);
        o_assert(!err && curBuf.view);
    }

    // create optional depth buffer
    if (PixelFormat::None != attrs.DepthPixelFormat) {
        this->depthBuffer.format = vlkTypes::asRenderTargetFormat(attrs.DepthPixelFormat);

        // create image objct
        VkImageCreateInfo imgInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imgInfo.imageType = VK_IMAGE_TYPE_2D;
        imgInfo.format = this->depthBuffer.format;
        imgInfo.extent.width = attrs.FramebufferWidth;
        imgInfo.extent.height = attrs.FramebufferHeight;
        imgInfo.extent.depth = 1;
        imgInfo.mipLevels = 1;
        imgInfo.arrayLayers = 1;
        imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imgInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imgInfo.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        VkResult err = vkCreateImage(this->Device, &imgInfo, nullptr, &this->depthBuffer.image);
        o_assert(!err && this->depthBuffer.image);

        // allocate memory
        VkMemoryRequirements memReqs = {};
        vkGetImageMemoryRequirements(this->Device, this->depthBuffer.image, &memReqs);
        VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = this->ResAllocator.findMemoryType(memReqs.memoryTypeBits, 0);
        o_assert(InvalidIndex != allocInfo.memoryTypeIndex);
        err = vkAllocateMemory(this->Device, &allocInfo, nullptr, &this->depthBuffer.mem);
        o_assert(!err);

        // bind memory
        err = vkBindImageMemory(this->Device, this->depthBuffer.image, this->depthBuffer.mem, 0);
        o_assert(!err);

        // create image view object
        VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.format = this->depthBuffer.format;
        viewInfo.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.image = this->depthBuffer.image;
        err = vkCreateImageView(this->Device, &viewInfo, nullptr, &this->depthBuffer.view);
        assert(!err);
    }

    // setup RenderPass
    int numRpAttachments = 1;
    VkAttachmentDescription rpAttachments[2] = {};
    rpAttachments[0].format = this->pixelFormat;
    rpAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    if (setup.ClearHint.Actions & ClearState::ColorBit) {
        rpAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
    else {
        rpAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
    rpAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    rpAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    rpAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    rpAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    rpAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if (VK_FORMAT_UNDEFINED != this->depthBuffer.format) {
        numRpAttachments++;
        rpAttachments[1].format = this->depthBuffer.format;
        rpAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
        if (setup.ClearHint.Actions & ClearState::DepthBit) {
            rpAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
        else {
            rpAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
        rpAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        if (setup.ClearHint.Actions & ClearState::StencilBit) {
            rpAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
        else {
            rpAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
        rpAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        rpAttachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        rpAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
    VkAttachmentReference colorRef = {};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference depthRef = {};
    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    if (VK_FORMAT_UNDEFINED != this->depthBuffer.format) {
        subpass.pDepthStencilAttachment = &depthRef;
    }
    VkRenderPassCreateInfo passInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    passInfo.attachmentCount = numRpAttachments;
    passInfo.pAttachments = rpAttachments;
    passInfo.subpassCount = 1;
    passInfo.pSubpasses = &subpass;
    err = vkCreateRenderPass(this->Device, &passInfo, nullptr, &this->RenderPass);
    o_assert(!err && this->RenderPass);
    
    // create framebuffer objects
    VkImageView fbAttachments[2] = {};
    int fbAttachmentCount = 1;
    if (VK_FORMAT_UNDEFINED != this->depthBuffer.format) {
        fbAttachments[1] = this->depthBuffer.view;
        fbAttachmentCount++;
    }
    VkFramebufferCreateInfo fbInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    fbInfo.renderPass = this->RenderPass;
    fbInfo.attachmentCount = fbAttachmentCount;
    fbInfo.pAttachments = fbAttachments;
    fbInfo.width = attrs.FramebufferWidth;
    fbInfo.height = attrs.FramebufferHeight;
    fbInfo.layers = 1; // ???
    for (uint32 i = 0; i < vlkConfig::NumFrames; i++) {
        o_assert(nullptr == this->swapChainBuffers[i].framebuffer);
        fbAttachments[0] = this->swapChainBuffers[i].view;
        err = vkCreateFramebuffer(this->Device, &fbInfo, nullptr, &(this->swapChainBuffers[i].framebuffer));
        o_assert(!err && this->swapChainBuffers[i].framebuffer);
    }

    this->submitCmdBuffer(0, nullptr, nullptr);
    vkQueueWaitIdle(this->Queue);
    return attrs;
}

//------------------------------------------------------------------------------
void
vlkContext::discardSwapchain(bool forResize) {
    o_assert(this->Device);
    o_assert(fpDestroySwapchainKHR);
    o_assert(this->RenderPass);

    for (int i = 0; i < vlkConfig::NumFrames; i++) {
        if (this->swapChainBuffers[i].framebuffer) {
            vkDestroyFramebuffer(this->Device, this->swapChainBuffers[i].framebuffer, nullptr);
            this->swapChainBuffers[i].framebuffer = nullptr;
        }
    }
    vkDestroyRenderPass(this->Device, this->RenderPass, nullptr);
    this->RenderPass = nullptr;
    if (this->depthBuffer.view) {
        vkDestroyImageView(this->Device, this->depthBuffer.view, nullptr);
        this->depthBuffer.view = nullptr;
    }
    if (this->depthBuffer.image) {
        vkDestroyImage(this->Device, this->depthBuffer.image, nullptr);
        this->depthBuffer.image = nullptr;
    }
    if (this->depthBuffer.mem) {
        vkFreeMemory(this->Device, this->depthBuffer.mem, nullptr);
        this->depthBuffer.mem = nullptr;
    }
    for (auto& curBuf : this->swapChainBuffers) {
        curBuf.image = nullptr;
        o_assert(curBuf.view);
        vkDestroyImageView(this->Device, curBuf.view, nullptr);
        curBuf.view = nullptr;
        o_assert(curBuf.imageAcquiredSemaphore);
        vkDestroySemaphore(this->Device, curBuf.imageAcquiredSemaphore, nullptr);
        curBuf.imageAcquiredSemaphore = nullptr;
        o_assert(curBuf.renderingFinishedSemaphore);
        vkDestroySemaphore(this->Device, curBuf.renderingFinishedSemaphore, nullptr);
        curBuf.renderingFinishedSemaphore = nullptr;
    }
    if (!forResize) {
        o_assert(this->SwapChain);
        fpDestroySwapchainKHR(this->Device, this->SwapChain, nullptr);
        this->SwapChain = nullptr;
    }
}

} // namespace _priv
} // namespace Oryol
