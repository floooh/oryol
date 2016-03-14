//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  vlkContext.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlk_impl.h"
#include "vlkContext.h"
#include "vlkTypes.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkContext::~vlkContext() {
    o_assert(nullptr == this->Instance);
    o_assert(nullptr == this->PhysicalDevice);
    o_assert(nullptr == this->Device);
    o_assert(nullptr == this->instLayers);
    o_assert(nullptr == this->instExtensions);
    o_assert(nullptr == this->physDevices);
    o_assert(nullptr == this->devLayers);
    o_assert(nullptr == this->devExtensions);
    o_assert(nullptr == this->queueFamilies);
}

//------------------------------------------------------------------------------
void
vlkContext::setup(const GfxSetup& setup, const char** requiredInstanceExtensions, int numRequiredInstanceExtensions) {
    o_assert(nullptr == this->Instance);
    o_assert(nullptr == this->PhysicalDevice);
    o_assert(nullptr == this->Device);

    #if ORYOL_DEBUG
    const char* requestedLayers[] = {
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
    };
    const int numRequestedLayers = int(std::size(requestedLayers));
    #else
    const char** requestedLayers = nullptr;
    int numRequestedLayers = 0;
    #endif
    const char* requiredDeviceExtensions[] = {
        "VK_KHR_swapchain"
    };
    const int numRequiredDeviceExtensions = int(std::size(requiredDeviceExtensions));

    this->setupInstanceLayers(requestedLayers, numRequestedLayers);
    this->setupInstanceExtensions(requiredInstanceExtensions, numRequiredInstanceExtensions);
    this->setupInstance(setup);
    this->setupPhysicalDevice(setup);
    this->setupDeviceLayers(requestedLayers, numRequestedLayers);
    this->setupDeviceExtensions(requiredDeviceExtensions, numRequiredDeviceExtensions);
    this->setupQueueFamilies();
}

//------------------------------------------------------------------------------
void
vlkContext::discard() {
    this->discardQueueFamilies();
    this->discardDeviceExtensions();
    this->discardDeviceLayers();
    this->discardPhysicalDevice();
    this->discardInstance();
    this->discardInstanceExtensions();
    this->discardInstanceLayers();
}

//------------------------------------------------------------------------------
int
vlkContext::findLayer(const char* name, const VkLayerProperties* layers, int numLayers) {
    o_assert_dbg(name && layers && (numLayers >= 0));
    for (int i = 0; i < numLayers; i++) {
        if (0 == std::strcmp(name, layers[i].layerName)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
void
vlkContext::dumpLayerInfo(const char* title, const VkLayerProperties* layerProps, int numLayers) {
    o_assert_dbg(title && layerProps && (numLayers >= 0));
    Log::Info(">>> %s:\n", title);
    for (int i = 0; i < numLayers; i++) {
        const VkLayerProperties& cur = layerProps[i];
        Log::Info("  Name: %s\n    spec version: %x\n    impl version: %x\n    desc: %s\n\n",
            cur.layerName, cur.specVersion, cur.implementationVersion, cur.description);
    }
}

//------------------------------------------------------------------------------
void
vlkContext::selectLayers(const char** reqLayers, int numReqLayers, 
                         const VkLayerProperties* availLayers, int numAvailLayers,
                         const char** outSelLayers, int& outNumSelLayers) {

    outNumSelLayers = 0;
    if (reqLayers) {
        o_assert(numReqLayers <= maxSelLayers);
        for (int i = 0; i < numReqLayers; i++) {
            o_assert_dbg(reqLayers[i]);
            int availLayerIndex = findLayer(reqLayers[i], availLayers, numAvailLayers);
            if (InvalidIndex != availLayerIndex) {
                outSelLayers[outNumSelLayers++] = availLayers[availLayerIndex].layerName;
            }
            else {
                o_warn("vlkContext: requested instance layer '%s' not found!\n", reqLayers[i]);
            }
        }
    }
}

//------------------------------------------------------------------------------
int
vlkContext::findExtension(const char* name, const VkExtensionProperties* exts, int numExts) {
    o_assert_dbg(name && exts && (numExts >= 0));
    for (int i = 0; i < numExts; i++) {
        if (0 == std::strcmp(name, exts[i].extensionName)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
void
vlkContext::dumpExtensionInfo(const char* title, const VkExtensionProperties* exts, int numExts) {
    o_assert_dbg(title && exts && (numExts >= 0));
    Log::Info(">>> %s:\n", title);
    for (int i = 0; i < numExts; i++) {
        const VkExtensionProperties& cur = exts[i];
        Log::Info("  %s (ver %x)\n", cur.extensionName, cur.specVersion);
    }
}

//------------------------------------------------------------------------------
void
vlkContext::selectExtensions(const char** reqExts, int numReqExts, 
                             const VkExtensionProperties* availExts, int numAvailExts,
                             const char** outSelExts, int& outNumSelExts) {

    outNumSelExts = 0;
    if (reqExts) {
        o_assert(numReqExts < maxSelExtensions);
        for (int i = 0; i < numReqExts; i++) {
            o_assert_dbg(reqExts[i]);
            int availExtIndex = findExtension(reqExts[i], availExts, numAvailExts);
            if (InvalidIndex != availExtIndex) {
                outSelExts[outNumSelExts++] = availExts[availExtIndex].extensionName;
            }
            else {
                o_warn("vlkContext: requested instance extension '%s' not found!\n", reqExts[i]);
            }
        }
    }
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstanceLayers(const char** layers, int numLayers) {
    o_assert(nullptr == this->instLayers);
    o_assert(0 == this->numInstLayers);
    o_assert(0 == this->numSelInstLayers);

    // first get number of instance layers, allocate room, and get layer properties
    VkResult err = vkEnumerateInstanceLayerProperties(&this->numInstLayers, nullptr);
    o_assert(!err);
    if (this->numInstLayers > 0) {
        const int size = this->numInstLayers * sizeof(VkLayerProperties);
        this->instLayers = (VkLayerProperties*) Memory::Alloc(size);
        err = vkEnumerateInstanceLayerProperties(&this->numInstLayers, this->instLayers);
    }

    // dump out all instance layers
    this->dumpLayerInfo("Instance Layers", this->instLayers, this->numInstLayers);

    // select requested layers
    this->selectLayers(layers, numLayers, this->instLayers, this->numInstLayers, this->selInstLayers, this->numSelInstLayers);
}

//------------------------------------------------------------------------------
void
vlkContext::discardInstanceLayers() {
    if (this->instLayers) {
        Memory::Free(this->instLayers);
        this->instLayers = nullptr;
    }
    this->numInstLayers = 0;
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstanceExtensions(const char** exts, int numExts) {
    o_assert(nullptr == this->instExtensions);
    o_assert(0 == this->numInstExtensions);
    o_assert(0 == this->numSelInstExtensions);

    // enumerate available extensions
    VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &this->numInstExtensions, nullptr);
    o_assert(!err);
    if (this->numInstExtensions > 0) {
        const int size = this->numInstExtensions * sizeof(VkExtensionProperties);
        this->instExtensions = (VkExtensionProperties*) Memory::Alloc(size);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &this->numInstExtensions, this->instExtensions);
    }

    // dump extension info
    dumpExtensionInfo("Instance Extensions", this->instExtensions, this->numInstExtensions); 

    // filter requested extensions against available extensions
    // FIXME: need to distinguish between optional and required extensions!
    selectExtensions(exts, numExts, this->instExtensions, this->numInstExtensions, this->selInstExtensions, this->numSelInstExtensions);
}

//------------------------------------------------------------------------------
void
vlkContext::discardInstanceExtensions() {
    if (this->instExtensions) {
        Memory::Free(this->instExtensions);
        this->instExtensions = nullptr;
    }
    this->numInstExtensions = 0;
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstance(const GfxSetup& setup) {
    o_assert(nullptr == this->Instance);

    VkApplicationInfo appInfo = { };
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "ORYOL_APP";    // FIXME
    appInfo.applicationVersion = 0;
    appInfo.pEngineName = "ORYOL";
    appInfo.engineVersion = 0;
    appInfo.apiVersion = VK_API_VERSION;

    VkInstanceCreateInfo instInfo = { };
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = nullptr;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledLayerCount = this->numSelInstLayers;
    instInfo.ppEnabledLayerNames = this->selInstLayers;
    instInfo.enabledExtensionCount = this->numSelInstExtensions;
    instInfo.ppEnabledExtensionNames = this->selInstExtensions;
    
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
void
vlkContext::setupPhysicalDevice(const GfxSetup& setup) {
    o_assert(nullptr == this->PhysicalDevice);
    o_assert(nullptr == this->physDevices);
    o_assert(0 == this->numPhysDevices);
    o_assert(this->Instance);

    // get number of physical devices
    VkResult err = vkEnumeratePhysicalDevices(this->Instance, &this->numPhysDevices, nullptr);
    o_assert(!err);
    if (this->numPhysDevices <= 0) {
        o_error("No Vulkan-enabled GPUs in system!\n");
    }
    this->physDevices = (VkPhysicalDevice*) Memory::Alloc(this->numPhysDevices * sizeof(VkPhysicalDevice));
    err = vkEnumeratePhysicalDevices(this->Instance, &this->numPhysDevices, this->physDevices);
    o_assert(!err);

    // dump device info, and also pick first dedicated GPU
    int firstDiscreteGPU = InvalidIndex;
    VkPhysicalDeviceProperties props = { };
    for (uint32 i = 0; i < this->numPhysDevices; i++) {
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
}

//------------------------------------------------------------------------------
void
vlkContext::discardPhysicalDevice() {
    o_assert(this->PhysicalDevice);
    o_assert(this->physDevices);

    Memory::Free(this->physDevices);
    this->physDevices = nullptr;
    this->PhysicalDevice = nullptr;
}

//------------------------------------------------------------------------------
void
vlkContext::setupDeviceLayers(const char** layers, int numLayers) {
    o_assert(this->PhysicalDevice);
    o_assert(0 == this->numDevLayers);
    o_assert(nullptr == this->devLayers);

    VkResult err = vkEnumerateDeviceLayerProperties(this->PhysicalDevice, &this->numDevLayers, nullptr);
    o_assert(!err);
    if (this->numDevLayers > 0) {
        const int size = this->numDevLayers * sizeof(VkLayerProperties);
        this->devLayers = (VkLayerProperties*) Memory::Alloc(size);
        err = vkEnumerateDeviceLayerProperties(this->PhysicalDevice, &this->numDevLayers, this->devLayers);
    }
    dumpLayerInfo("Device Layers", this->devLayers, this->numDevLayers);
    selectLayers(layers, numLayers, this->devLayers, this->numDevLayers, this->selDevLayers, this->numSelDevLayers);
}

//------------------------------------------------------------------------------
void
vlkContext::discardDeviceLayers() {
    if (this->devLayers) {
        Memory::Free(this->devLayers);
        this->devLayers = nullptr;
    }
    this->numDevLayers = 0;
}

//------------------------------------------------------------------------------
void
vlkContext::setupDeviceExtensions(const char** exts, int numExts) {
    o_assert(nullptr == this->devExtensions);
    o_assert(0 == this->numDevExtensions);
    o_assert(0 == this->numSelDevExtensions);

    // enumerate available device extensions
    VkResult err = vkEnumerateDeviceExtensionProperties(this->PhysicalDevice, nullptr, &this->numDevExtensions, nullptr);
    o_assert(!err);
    if (this->numDevExtensions > 0) {
        const int size = this->numDevExtensions * sizeof(VkExtensionProperties);
        this->devExtensions = (VkExtensionProperties*)Memory::Alloc(size);
        err = vkEnumerateDeviceExtensionProperties(this->PhysicalDevice, nullptr, &this->numDevExtensions, this->devExtensions);
    }

    // dump device extension info
    dumpExtensionInfo("Device Extensions", this->devExtensions, this->numDevExtensions);

    // filter requested extensions against available extensions
    // FIXME: need to distinguish between optional and required extensions!
    selectExtensions(exts, numExts, this->devExtensions, this->numDevExtensions, this->selDevExtensions, this->numSelDevExtensions);
}

//------------------------------------------------------------------------------
void
vlkContext::discardDeviceExtensions() {
    if (this->devExtensions) {
        Memory::Free(this->devExtensions);
        this->devExtensions = nullptr;
    }
    this->numDevExtensions = 0;
}

//------------------------------------------------------------------------------
void
vlkContext::setupQueueFamilies() {
    o_assert(this->PhysicalDevice);
    o_assert(nullptr == this->queueFamilies);
    o_assert(0 == this->numQueueFamilies);

    vkGetPhysicalDeviceQueueFamilyProperties(this->PhysicalDevice, &this->numQueueFamilies, nullptr);
    o_assert(this->numQueueFamilies >= 1);
    const int size = sizeof(VkQueueFamilyProperties) * this->numQueueFamilies;
    this->queueFamilies = (VkQueueFamilyProperties*) Memory::Alloc(size);
    vkGetPhysicalDeviceQueueFamilyProperties(this->PhysicalDevice, &this->numQueueFamilies, this->queueFamilies);
}

//------------------------------------------------------------------------------
void
vlkContext::discardQueueFamilies() {
    if (this->queueFamilies) {
        Memory::Free(this->queueFamilies);
        this->queueFamilies = nullptr;
    }
    this->numQueueFamilies = 0;
}

} // namespace _priv
} // namespace Oryol
