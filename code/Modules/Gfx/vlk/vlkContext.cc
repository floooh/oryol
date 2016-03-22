//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  vlkContext.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlk_impl.h"
#include "vlkContext.h"
#include "vlkTypes.h"

// extension function pointers
#define INST_FUNC_PTR(inst, func) \
{ \
    fp##func = (PFN_vk##func)vkGetInstanceProcAddr(inst, "vk" #func);\
    o_assert(fp##func);\
}

static PFN_vkCreateDebugReportCallbackEXT fpCreateDebugReportCallbackEXT = nullptr;
static PFN_vkDestroyDebugReportCallbackEXT fpDestroyDebugReportCallbackEXT = nullptr;
static PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
static PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR = nullptr;
static PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR = nullptr;
static PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR = nullptr;

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkContext::~vlkContext() {
    o_assert(nullptr == this->Instance);
    o_assert(nullptr == this->PhysicalDevice);
    o_assert(nullptr == this->Device);
    o_assert(0 == this->Surface);
}

//------------------------------------------------------------------------------
void
vlkContext::setup(const GfxSetup& setup, const Array<const char*>& requestedInstanceExtensions) {
    o_assert(nullptr == this->Instance);
    o_assert(nullptr == this->PhysicalDevice);
    o_assert(nullptr == this->Device);

    #if ORYOL_DEBUG
    Array<const char*> requestedLayers({
        "VK_LAYER_LUNARG_api_dump",
        "VK_LAYER_LUNARG_device_limits",
        "VK_LAYER_LUNARG_draw_state",
        "VK_LAYER_LUNARG_image",
        "VK_LAYER_LUNARG_mem_tracker",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_param_checker",
        "VK_LAYER_LUNARG_swapchain",
        "VK_LAYER_LUNARG_threading",
        "VK_LAYER_GOOGLE_unique_objects",
    });
    #else
    Array<const char*> requestedLayers;
    #endif
    Array<const char*> requestedDeviceExtensions({
        "VK_KHR_swapchain"
    });

    this->setupInstanceLayers(requestedLayers);
    this->setupInstanceExtensions(requestedInstanceExtensions);
    this->setupInstance(setup);
    #if ORYOL_DEBUG
    this->setupErrorReporting();
    #endif
    this->setupPhysicalDevice(setup);
    this->setupDeviceLayers(requestedLayers);
    this->setupDeviceExtensions(requestedDeviceExtensions);
    this->setupQueueFamilies();
}

//------------------------------------------------------------------------------
void
vlkContext::discard() {
    this->discardDepthBuffer();
    this->discardSwapchain(false);
    this->discardCommandPoolAndBuffers();
    this->discardSurfaceFormats();
    this->discardDeviceAndSurface();
    this->discardQueueFamilies();
    this->discardDeviceExtensions();
    this->discardDeviceLayers();
    this->discardPhysicalDevice();
    #if ORYOL_DEBUG
    this->discardErrorReporting();
    #endif
    this->discardInstance();
    this->discardInstanceExtensions();
    this->discardInstanceLayers();
}

//------------------------------------------------------------------------------
int
vlkContext::findLayer(const char* name, const Array<VkLayerProperties>& layers) {
    o_assert_dbg(name);
    for (int i = 0; i < layers.Size(); i++) {
        if (0 == std::strcmp(name, layers[i].layerName)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
void
vlkContext::dumpLayerInfo(const char* title, const Array<VkLayerProperties>& layerProps) {
    o_assert_dbg(title);
    Log::Info(">>> %s:\n", title);
    for (const auto& prop : layerProps) {
        Log::Info("  Name: %s\n    spec version: %x\n    impl version: %x\n    desc: %s\n\n",
            prop.layerName, prop.specVersion, prop.implementationVersion, prop.description);
    }
}

//------------------------------------------------------------------------------
Array<const char*>
vlkContext::selectLayers(const Array<const char*>& reqLayers, const Array<VkLayerProperties>& availLayers) {
    Array<const char*> result;
    result.Reserve(reqLayers.Size());
    for (int i = 0; i < reqLayers.Size(); i++) {
        int availLayerIndex = findLayer(reqLayers[i], availLayers);
        if (InvalidIndex != availLayerIndex) {
            result.Add(availLayers[availLayerIndex].layerName);
        }
        else {
            o_warn("vlkContext: requested instance layer '%s' not found!\n", reqLayers[i]);
        }
    }
    return std::move(result);
}

//------------------------------------------------------------------------------
int
vlkContext::findExtension(const char* name, const Array<VkExtensionProperties>& exts) {
    o_assert_dbg(name);
    for (int i = 0; i < exts.Size(); i++) {
        if (0 == std::strcmp(name, exts[i].extensionName)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
void
vlkContext::dumpExtensionInfo(const char* title, const Array<VkExtensionProperties>& exts) {
    o_assert_dbg(title);
    Log::Info(">>> %s:\n", title);
    for (const auto& ext : exts) {
        Log::Info("  %s (ver %x)\n", ext.extensionName, ext.specVersion);
    }
}

//------------------------------------------------------------------------------
Array<const char*>
vlkContext::selectExtensions(const Array<const char*>& reqExts, const Array<VkExtensionProperties>& availExts) {
    Array<const char*> result;
    result.Reserve(reqExts.Size());
    for (int i = 0; i < reqExts.Size(); i++) {
        int availExtIndex = findExtension(reqExts[i], availExts);
        if (InvalidIndex != availExtIndex) {
            result.Add(availExts[availExtIndex].extensionName);
        }
        else {
            o_warn("vlkContext: requested instance extension '%s' not found!\n", reqExts[i]);
        }
    }
    return std::move(result);
}

//------------------------------------------------------------------------------
int
vlkContext::findMemoryType(uint32 typeBits, VkFlags reqMask) {
    for (int i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
        if (typeBits & (1<<i)) {
            VkMemoryPropertyFlags propMask = this->memoryProps.memoryTypes[i].propertyFlags;
            if ((propMask & reqMask) == reqMask) {
                return i;
            }
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstanceLayers(const Array<const char*>& requestedLayers) {
    o_assert(this->instLayers.Empty());
    o_assert(this->selInstLayers.Empty());

    uint32_t num = 0;
    VkResult err = vkEnumerateInstanceLayerProperties(&num, nullptr);
    o_assert(!err);
    if (num > 0) {
        this->instLayers.Resize(num);
        err = vkEnumerateInstanceLayerProperties(&num, this->instLayers.Data());
    }
    this->dumpLayerInfo("Instance Layers", this->instLayers);
    this->selInstLayers = selectLayers(requestedLayers, this->instLayers);
}

//------------------------------------------------------------------------------
void
vlkContext::discardInstanceLayers() {
    this->instLayers.Clear();
    this->selInstLayers.Clear();
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstanceExtensions(const Array<const char*>& requestedExts) {
    o_assert(this->instExtensions.Empty());
    o_assert(this->selInstExtensions.Empty());

    uint32_t num = 0;
    VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &num, nullptr);
    o_assert(!err);
    if (num > 0) {
        this->instExtensions.Resize(num);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &num, this->instExtensions.Data());
    }
    dumpExtensionInfo("Instance Extensions", this->instExtensions); 
    #if ORYOL_DEBUG
        Array<const char*> dbgExts(requestedExts);
        dbgExts.Add(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        this->selInstExtensions = selectExtensions(dbgExts, this->instExtensions);
    #else
        this->selInstExtensions = selectExtensions(requestedExts, this->instExtensions);
    #endif
}

//------------------------------------------------------------------------------
void
vlkContext::discardInstanceExtensions() {
    this->instExtensions.Clear();
    this->selInstExtensions.Clear();
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstance(const GfxSetup& setup) {
    o_assert(nullptr == this->Instance);

    VkApplicationInfo appInfo = { };
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ORYOL_APP";    // FIXME
    appInfo.applicationVersion = 0;
    appInfo.pEngineName = "ORYOL";
    appInfo.engineVersion = 0;
    appInfo.apiVersion = VK_API_VERSION;

    VkInstanceCreateInfo instInfo = { };
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledLayerCount = this->selInstLayers.Size();;
    instInfo.ppEnabledLayerNames = this->selInstLayers.Data();
    instInfo.enabledExtensionCount = this->selInstExtensions.Size();
    instInfo.ppEnabledExtensionNames = this->selInstExtensions.Data();
    
    VkResult err = vkCreateInstance(&instInfo, nullptr, &this->Instance);
    o_assert(!err); // FIXME: better error messages
    o_assert(this->Instance);
}

//------------------------------------------------------------------------------
void
vlkContext::discardInstance() {
    o_assert(this->Instance);
    vkDestroyInstance(this->Instance, nullptr);
    this->Instance = nullptr;
}

//------------------------------------------------------------------------------
#if ORYOL_DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL
errorReportingCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                       uint64_t srcObject, size_t location, int32_t msgCode, 
                       const char* pLayerPrefix, const char* pMsg, void* pUserData)
{
    const char* type = "UNKNOWN";
    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        type = "ERROR";
    }
    else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        type = "WARNING";
    }
    else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        type = "PERFORMANCE";
    }
    else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        type = "INFO";
    }
    else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        type = "DEBUG";
    }
    Log::Info("VULKAN [%s] [%s] [%d]: %s\n", type, pLayerPrefix, msgCode, pMsg);
    /*
    * false indicates that layer should not bail-out of an
    * API call that had validation failures. This may mean that the
    * app dies inside the driver due to invalid parameter(s).
    * That's what would happen without validation layers, so we'll
    * keep that behavior here.
    */
    return false;
}
#endif

//------------------------------------------------------------------------------
#if ORYOL_DEBUG
void
vlkContext::setupErrorReporting() {
    o_assert(this->Instance);
    o_assert(nullptr == this->debugReportCallback);

    INST_FUNC_PTR(this->Instance, CreateDebugReportCallbackEXT);
    VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = { };
    dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbgCreateInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                          VK_DEBUG_REPORT_ERROR_BIT_EXT |
                          VK_DEBUG_REPORT_WARNING_BIT_EXT |
                          VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                          VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    dbgCreateInfo.pfnCallback = errorReportingCallback;
    VkResult err = fpCreateDebugReportCallbackEXT(this->Instance, &dbgCreateInfo, nullptr, &this->debugReportCallback);
    o_assert(!err);
}
#endif

//------------------------------------------------------------------------------
#ifdef ORYOL_DEBUG
void
vlkContext::discardErrorReporting() {
    o_assert(this->Instance);
    o_assert(this->debugReportCallback);

    INST_FUNC_PTR(this->Instance, DestroyDebugReportCallbackEXT);
    fpDestroyDebugReportCallbackEXT(this->Instance, this->debugReportCallback, nullptr);
    this->debugReportCallback = nullptr;
}
#endif

//------------------------------------------------------------------------------
void
vlkContext::setupPhysicalDevice(const GfxSetup& setup) {
    o_assert(nullptr == this->PhysicalDevice);
    o_assert(this->physDevices.Empty());
    o_assert(this->Instance);

    // get number of physical devices
    uint32_t num = 0;
    VkResult err = vkEnumeratePhysicalDevices(this->Instance, &num, nullptr);
    o_assert(!err);
    if (num > 0) {
        this->physDevices.Resize(num);
        err = vkEnumeratePhysicalDevices(this->Instance, &num, this->physDevices.Data());
        o_assert(!err);
    }
    else {
        o_error("No Vulkan-enabled GPUs in system!\n");
    }

    // dump device info, and also pick first dedicated GPU
    int firstDiscreteGPU = InvalidIndex;
    VkPhysicalDeviceProperties props = { };
    for (int i = 0; i < this->physDevices.Size(); i++) {
        vkGetPhysicalDeviceProperties(this->physDevices[i], &props);
        Log::Info("GPU %d: %s (type: %s)\n", i, props.deviceName, vlkTypes::physicalDeviceTypeAsString(props.deviceType));
        if ((InvalidIndex == firstDiscreteGPU) &&
            (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == props.deviceType)) {
            firstDiscreteGPU = i;
        }
    }
    
    // choose the 'best' physical device
    // FIXME: might want to have more control over this
    if (InvalidIndex != firstDiscreteGPU) {
        this->PhysicalDevice = this->physDevices[firstDiscreteGPU];
    }
    else {
        this->PhysicalDevice = this->physDevices[0];
    }
    vkGetPhysicalDeviceMemoryProperties(this->PhysicalDevice, &this->memoryProps);
}

//------------------------------------------------------------------------------
void
vlkContext::discardPhysicalDevice() {
    o_assert(this->PhysicalDevice);
    this->physDevices.Clear();
    this->PhysicalDevice = nullptr;
}

//------------------------------------------------------------------------------
void
vlkContext::setupDeviceLayers(const Array<const char*>& requestedLayers) {
    o_assert(this->PhysicalDevice);
    o_assert(this->devLayers.Empty());
    o_assert(this->selDevLayers.Empty());

    uint32_t num = 0;
    VkResult err = vkEnumerateDeviceLayerProperties(this->PhysicalDevice, &num, nullptr);
    o_assert(!err);
    if (num > 0) {
        this->devLayers.Resize(num);
        err = vkEnumerateDeviceLayerProperties(this->PhysicalDevice, &num, this->devLayers.Data());
    }
    dumpLayerInfo("Device Layers", this->devLayers);
    this->selDevLayers = selectLayers(requestedLayers, this->devLayers);
}

//------------------------------------------------------------------------------
void
vlkContext::discardDeviceLayers() {
    this->devLayers.Clear();
    this->selDevLayers.Clear();
}

//------------------------------------------------------------------------------
void
vlkContext::setupDeviceExtensions(const Array<const char*>& requestedExts) {
    o_assert(this->devExtensions.Empty());
    o_assert(this->selDevExtensions.Empty());

    uint32_t num = 0;
    VkResult err = vkEnumerateDeviceExtensionProperties(this->PhysicalDevice, nullptr, &num, nullptr);
    o_assert(!err);
    if (num > 0) {
        this->devExtensions.Resize(num);
        err = vkEnumerateDeviceExtensionProperties(this->PhysicalDevice, nullptr, &num, this->devExtensions.Data());
    }
    dumpExtensionInfo("Device Extensions", this->devExtensions);
    this->selDevExtensions = selectExtensions(requestedExts, this->devExtensions);
}

//------------------------------------------------------------------------------
void
vlkContext::discardDeviceExtensions() {
    this->devExtensions.Clear();
    this->selDevExtensions.Clear();
}

//------------------------------------------------------------------------------
void
vlkContext::setupQueueFamilies() {
    o_assert(this->PhysicalDevice);
    o_assert(this->queueProps.Empty());

    uint32_t num = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(this->PhysicalDevice, &num, nullptr);
    o_assert(num >= 1);
    this->queueProps.Resize(num);
    vkGetPhysicalDeviceQueueFamilyProperties(this->PhysicalDevice, &num, this->queueProps.Data());
}

//------------------------------------------------------------------------------
void
vlkContext::discardQueueFamilies() {
    this->queueProps.Clear();
}

//------------------------------------------------------------------------------
void
vlkContext::initQueueIndices(VkSurfaceKHR surf) {
    o_assert(this->PhysicalDevice);
    o_assert(this->Surface);
    o_assert(InvalidIndex == this->graphicsQueueIndex);
    o_assert(InvalidIndex == this->presentQueueIndex);

    // lookup required function pointers
    INST_FUNC_PTR(this->Instance, GetPhysicalDeviceSurfaceSupportKHR);

    // find a graphics and present queue, prefer one queue that supports both
    for (int queueIndex = 0; queueIndex < this->queueProps.Size(); queueIndex++) {
        const auto& cur = this->queueProps[queueIndex];
        if (cur.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            if (InvalidIndex == this->graphicsQueueIndex) {
                this->graphicsQueueIndex = queueIndex;
            }
            VkBool32 supportsPresent = VK_FALSE;
            fpGetPhysicalDeviceSurfaceSupportKHR(this->PhysicalDevice, queueIndex, this->Surface, &supportsPresent);
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
        for (int queueIndex = 0; queueIndex < this->queueProps.Size(); queueIndex++) {
            VkBool32 supportsPresent = VK_FALSE;
            fpGetPhysicalDeviceSurfaceSupportKHR(this->PhysicalDevice, queueIndex, this->Surface, &supportsPresent);
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
}

//------------------------------------------------------------------------------
void
vlkContext::setupDevice() {
    o_assert(this->PhysicalDevice);
    o_assert(this->Surface);
    o_assert(nullptr == this->Device);

    float queuePrios[] = { 1.0 };
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = this->graphicsQueueIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePrios;

    VkDeviceCreateInfo devCreateInfo = {};
    devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreateInfo.queueCreateInfoCount = 1;
    devCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    #if ORYOL_DEBUG
    devCreateInfo.enabledLayerCount = this->selDevLayers.Size();
    devCreateInfo.ppEnabledLayerNames = this->selDevLayers.Data();
    #endif
    devCreateInfo.enabledExtensionCount = this->selDevExtensions.Size();
    devCreateInfo.ppEnabledExtensionNames = this->selDevExtensions.Data();
    devCreateInfo.pEnabledFeatures = nullptr;   // FIXME: enable optional features

    VkResult createDeviceError = vkCreateDevice(this->PhysicalDevice, &devCreateInfo, nullptr, &this->Device);
    o_assert(!createDeviceError);
    o_assert(this->Device);
}

//------------------------------------------------------------------------------
void
vlkContext::discardDeviceAndSurface() {
    o_assert(this->Device);
    o_assert(this->Surface);
    vkDestroyDevice(this->Device, nullptr);
    this->Device = nullptr;
    vkDestroySurfaceKHR(this->Instance, this->Surface, nullptr);
    this->Surface = 0;
}

//------------------------------------------------------------------------------
void
vlkContext::setupSurfaceFormats(const GfxSetup& setup) {
    o_assert(this->Instance);
    o_assert(this->PhysicalDevice);
    o_assert(this->Surface);
    o_assert(this->surfaceFormats.Empty());
    o_assert(VK_FORMAT_MAX_ENUM == this->format);
    o_assert(VK_COLORSPACE_MAX_ENUM == this->colorSpace);

    // lookup required function pointers
    INST_FUNC_PTR(this->Instance, GetPhysicalDeviceSurfaceFormatsKHR);

    // enumerate surface formats
    uint32_t num = 0;
    VkResult err = fpGetPhysicalDeviceSurfaceFormatsKHR(this->PhysicalDevice, this->Surface, &num, nullptr);
    o_assert(!err && (num > 0));
    this->surfaceFormats.Resize(num);
    err = fpGetPhysicalDeviceSurfaceFormatsKHR(this->PhysicalDevice, this->Surface, &num, this->surfaceFormats.Data());
    o_assert(!err);

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if ((1 == this->surfaceFormats.Size()) && (VK_FORMAT_UNDEFINED == this->surfaceFormats[0].format)) {
        this->format = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else {
        this->format = this->surfaceFormats[0].format;
    }
    this->colorSpace = this->surfaceFormats[0].colorSpace;
}

//------------------------------------------------------------------------------
void
vlkContext::discardSurfaceFormats() {
    this->surfaceFormats.Clear();
}

//------------------------------------------------------------------------------
void
vlkContext::setupCommandPoolAndBuffers() {
    o_assert(this->Device);
    o_assert(nullptr == this->cmdPool);
    o_assert(nullptr == this->cmdBuffers[0]);

    VkCommandPoolCreateInfo poolInfo = { };
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = this->graphicsQueueIndex;
    VkResult err = vkCreateCommandPool(this->Device, &poolInfo, nullptr, &this->cmdPool);
    o_assert(!err && this->cmdPool);

    VkCommandBufferAllocateInfo bufInfo = { };
    bufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    bufInfo.commandPool = this->cmdPool;
    bufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    bufInfo.commandBufferCount = vlkConfig::NumFrames;
    err = vkAllocateCommandBuffers(this->Device, &bufInfo, this->cmdBuffers);
    o_assert(!err);
}

//------------------------------------------------------------------------------
void
vlkContext::discardCommandPoolAndBuffers() {
    o_assert(this->Device);
    o_assert(this->cmdPool);
    o_assert(this->cmdBuffers[0]);
    
    vkFreeCommandBuffers(this->Device, this->cmdPool, vlkConfig::NumFrames, this->cmdBuffers);
    for (int i = 0 ; i < vlkConfig::NumFrames; i++) {
        this->cmdBuffers[i] = nullptr;
    }
    vkDestroyCommandPool(this->Device, this->cmdPool, nullptr);
    this->cmdPool = nullptr;
}

//------------------------------------------------------------------------------
void
vlkContext::beginCmdBuffer() {
    VkCommandBufferInheritanceInfo inhInfo = {};
    inhInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    VkCommandBufferBeginInfo bgnInfo = {};
    bgnInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bgnInfo.pInheritanceInfo = &inhInfo;
    VkCommandBuffer cmdBuf = this->curCmdBuffer();
    VkResult err = vkBeginCommandBuffer(cmdBuf, &bgnInfo);
    o_assert(!err);
}

//------------------------------------------------------------------------------
void
vlkContext::submitCmdBuffer() {
    VkCommandBuffer cmdBuf = this->curCmdBuffer();
    VkResult err = vkEndCommandBuffer(cmdBuf);
    o_assert(!err);

    // FIXME: need a fence here?
    const VkCommandBuffer cmdBufs[] = { cmdBuf };
    VkFence nullFence = { VK_NULL_HANDLE };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = cmdBufs;
    err = vkQueueSubmit(this->Queue, 1, &submitInfo, nullFence);
    o_assert(!err);
}

//------------------------------------------------------------------------------
void
vlkContext::transitionImageLayout(VkImage img, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout) {
    o_assert(this->Device);
    o_assert(this->cmdPool);
    o_assert(img);

    VkImageMemoryBarrier imgMemBarrier = { };
    imgMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imgMemBarrier.oldLayout = oldLayout;
    imgMemBarrier.newLayout = newLayout;
    imgMemBarrier.image = img;
    imgMemBarrier.subresourceRange = { aspectMask, 0, 1, 0, 1 };
    switch (newLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: 
            imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT; 
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imgMemBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imgMemBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT|VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
            break;
    }
    const VkImageMemoryBarrier* pImgMemBarriers = &imgMemBarrier;
    VkCommandBuffer cmdBuf = this->curCmdBuffer();
    vkCmdPipelineBarrier(cmdBuf,                                // commandBuffer
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,     // srcStageMask
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,     // dstStageMask
                         0,                                     // dependencyFLags
                         0,                                     // memoryBarrierCount
                         nullptr,                               // pMemoryBarriers
                         0,                                     // bufferMemoryBarrierCount
                         nullptr,                               // pBufferMemoryBarriers
                         1,                                     // imageMemoryBarrierCount
                         pImgMemBarriers);                      // pImageMemoryBarriers
}

//------------------------------------------------------------------------------
DisplayAttrs
vlkContext::setupSwapchain(const GfxSetup& setup, const DisplayAttrs& attrs) {

    // NOTE: this will also destroy the previous swap chain (if one exists)
    // do NOT call discardSwapChain() during a resize operation, only
    // on final cleanup
    o_assert(this->Instance);
    o_assert(this->PhysicalDevice);
    o_assert(this->Device);
    o_assert(this->Surface);

    // lookup required function pointers
    INST_FUNC_PTR(this->Instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    INST_FUNC_PTR(this->Instance, GetPhysicalDeviceSurfacePresentModesKHR);
    INST_FUNC_PTR(this->Instance, CreateSwapchainKHR);
    INST_FUNC_PTR(this->Instance, DestroySwapchainKHR);
    INST_FUNC_PTR(this->Instance, GetSwapchainImagesKHR);

    VkSurfaceCapabilitiesKHR surfCaps = { };
    VkResult err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(this->PhysicalDevice, this->Surface, &surfCaps);
    o_assert(!err);
    o_assert(surfCaps.currentExtent.width != (uint32)-1);
    o_assert(surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
    o_assert(surfCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);

    // FIXME: this doesn't seem to have any purpose except supressing a validation layer error(?)
    static const int maxPresentModeCount = 8;
    uint32 presentModeCount = 0;
    err = fpGetPhysicalDeviceSurfacePresentModesKHR(this->PhysicalDevice, this->Surface, &presentModeCount, nullptr);
    o_assert(presentModeCount < maxPresentModeCount);
    VkPresentModeKHR presentModes[maxPresentModeCount] = { };
    err = fpGetPhysicalDeviceSurfacePresentModesKHR(this->PhysicalDevice, this->Surface, &presentModeCount, presentModes);
    o_assert(presentModeCount < maxPresentModeCount);

    // Determine the number of VkImage's to use in the swap chain (we desire to 
    // own only 1 image at a time, besides the images being displayed and
    // queued for display):
    uint32 desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
    if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount)) {
        // Application must settle for fewer images than desired:
        desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
    }
    o_assert(desiredNumberOfSwapchainImages < MaxNumSwapChainBuffers);

    VkSwapchainKHR oldSwapChain = this->SwapChain;
    this->SwapChain = nullptr;

    VkSwapchainCreateInfoKHR swapChainInfo = { };
    swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainInfo.surface = this->Surface;
    swapChainInfo.minImageCount = desiredNumberOfSwapchainImages;
    swapChainInfo.imageFormat = this->format;
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

    err = fpGetSwapchainImagesKHR(this->Device, this->SwapChain, &this->numSwapChainBuffers, nullptr);
    o_assert(!err && (this->numSwapChainBuffers < MaxNumSwapChainBuffers));
    VkImage swapChainImages[MaxNumSwapChainBuffers];
    err = fpGetSwapchainImagesKHR(this->Device, this->SwapChain, &this->numSwapChainBuffers, swapChainImages);
    o_assert(!err);
    for (uint32 i = 0; i < this->numSwapChainBuffers; i++) {
        this->swapChainBuffers[i].image = swapChainImages[i];

        // Render loop will expect image to have been used before and in
        // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        // layout and will change to COLOR_ATTACHMENT_OPTIMAL, so init the image
        // to that state
        this->transitionImageLayout(swapChainImages[i], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VkImageViewCreateInfo imgViewCreateInfo = { };
        imgViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgViewCreateInfo.format = this->format;
        imgViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        imgViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        imgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imgViewCreateInfo.flags = 0;
        imgViewCreateInfo.image = swapChainImages[i];
        err = vkCreateImageView(this->Device, &imgViewCreateInfo, nullptr, &this->swapChainBuffers[i].view);
        o_assert(!err && this->swapChainBuffers[i].view);
    }
    DisplayAttrs result = attrs;
    result.FramebufferWidth = surfCaps.currentExtent.width;
    result.FramebufferHeight = surfCaps.currentExtent.height;
    return result;
}

//------------------------------------------------------------------------------
void
vlkContext::discardSwapchain(bool forResize) {
    o_assert(this->Device);
    o_assert(fpDestroySwapchainKHR);
    for (uint32 i = 0; i < this->numSwapChainBuffers; i++) {
        o_assert(this->swapChainBuffers[i].view);
        vkDestroyImageView(this->Device, this->swapChainBuffers[i].view, nullptr);
        this->swapChainBuffers[i].image = nullptr;
        this->swapChainBuffers[i].view = nullptr;
    }
    if (!forResize) {
        o_assert(this->SwapChain);
        fpDestroySwapchainKHR(this->Device, this->SwapChain, nullptr);
        this->SwapChain = nullptr;
    }
}

//------------------------------------------------------------------------------
void
vlkContext::setupDepthBuffer(const GfxSetup& setup, const DisplayAttrs& attrs) {
    o_assert(this->Device);
    o_assert(nullptr == this->depthBuffer.image);
    o_assert(nullptr == this->depthBuffer.mem);
    o_assert(nullptr == this->depthBuffer.view);
    
    if (PixelFormat::None == attrs.DepthPixelFormat) {
        // no depth buffer requested
        return;
    }

    this->depthBuffer.format = vlkTypes::asRenderTargetFormat(attrs.DepthPixelFormat);
    
    // create image objct
    VkImageCreateInfo imgInfo = { };
    imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
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
    VkResult err = vkCreateImage(this->Device, &imgInfo, nullptr, &this->depthBuffer.image);
    o_assert(!err && this->depthBuffer.image);

    // allocate memory
    VkMemoryRequirements memReqs = { };
    vkGetImageMemoryRequirements(this->Device, this->depthBuffer.image, &memReqs);
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = this->findMemoryType(memReqs.memoryTypeBits, 0);
    o_assert(InvalidIndex != allocInfo.memoryTypeIndex);
    err = vkAllocateMemory(this->Device, &allocInfo, nullptr, &this->depthBuffer.mem);
    o_assert(!err);

    // bind memory
    err = vkBindImageMemory(this->Device, this->depthBuffer.image, this->depthBuffer.mem, 0);
    o_assert(!err);

    // transition to initial state
    this->transitionImageLayout(this->depthBuffer.image, 
        VK_IMAGE_ASPECT_DEPTH_BIT, 
        VK_IMAGE_LAYOUT_UNDEFINED, 
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    // create image view object
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.format = this->depthBuffer.format;
    viewInfo.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.image = this->depthBuffer.image;
    err = vkCreateImageView(this->Device, &viewInfo, nullptr, &this->depthBuffer.view);
    assert(!err);
}

//------------------------------------------------------------------------------
void
vlkContext::discardDepthBuffer() {
    o_assert(this->Device);
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
}

//------------------------------------------------------------------------------
DisplayAttrs
vlkContext::setupDeviceAndSwapChain(const GfxSetup& setup, const DisplayAttrs& attrs, VkSurfaceKHR surf) {
    o_assert(this->PhysicalDevice);
    o_assert(nullptr == this->Surface);
    o_assert(surf);
    
    this->Surface = surf;
    this->initQueueIndices(surf);
    this->setupDevice();
    vkGetDeviceQueue(this->Device, this->graphicsQueueIndex, 0, &this->Queue);
    this->setupSurfaceFormats(setup);
    this->setupCommandPoolAndBuffers();

    this->beginCmdBuffer();
    DisplayAttrs outAttrs = this->setupSwapchain(setup, attrs);
    this->setupDepthBuffer(setup, outAttrs);
    this->submitCmdBuffer();
    vkQueueWaitIdle(this->Queue);

    return outAttrs;
}

} // namespace _priv
} // namespace Oryol
