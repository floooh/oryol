//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  vlkContext.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlk_impl.h"
#include "vlkContext.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkContext::~vlkContext() {
    o_assert(nullptr == this->Instance);
    o_assert(nullptr == this->Gpu);
    o_assert(nullptr == this->Device);
    o_assert(nullptr == this->instLayers);
    o_assert(nullptr == this->instExtensions);
}

//------------------------------------------------------------------------------
void
vlkContext::setup(const GfxSetup& setup, const char** requiredExtensions, int numRequiredExtensions) {
    o_assert(nullptr == this->Instance);
    o_assert(nullptr == this->Gpu);
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
    #endif

    this->setupInstanceLayers(requestedLayers, int(std::size(requestedLayers)));
    this->setupInstanceExtensions(requiredExtensions, numRequiredExtensions);
    this->setupInstance(setup);
}

//------------------------------------------------------------------------------
void
vlkContext::discard() {
    this->discardInstance();
    this->discardInstanceExtensions();
    this->discardInstanceLayers();
}

//------------------------------------------------------------------------------
int
vlkContext::findInstanceLayer(const char* name) const {
    o_assert_dbg(name);
    for (uint32 i = 0; i < this->numInstLayers; i++) {
        if (0 == std::strcmp(name, this->instLayers[i].layerName)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstanceLayers(const char** layers, int numLayers) {
    o_assert(nullptr == this->instLayers);
    o_assert(0 == this->numInstLayers);
    o_assert(0 == this->numReqInstLayers);

    // first get number of instance layers, allocate room, and get layer properties
    VkResult err = vkEnumerateInstanceLayerProperties(&this->numInstLayers, nullptr);
    o_assert(!err);
    if (this->numInstLayers > 0) {
        const int size = this->numInstLayers * sizeof(VkLayerProperties);
        this->instLayers = (VkLayerProperties*) Memory::Alloc(size);
        err = vkEnumerateInstanceLayerProperties(&this->numInstLayers, this->instLayers);
    }

    // dump out all instance layers
    Log::Info(">>> Vulkan Instance Layers:\n");
    for (uint32 i = 0; i < this->numInstLayers; i++) {
        const VkLayerProperties& cur = this->instLayers[i];
        Log::Info("  Name: %s\n    spec version: %x\n    impl version: %x\n    desc: %s\n\n",
            cur.layerName, cur.specVersion, cur.implementationVersion, cur.description);
    }

    // filter requested layers against available layers
    if (layers) {
        o_assert(numLayers <= maxReqInstLayers);
        for (int i = 0; i < numLayers; i++) {
            o_assert_dbg(layers[i]);
            int layerIndex = this->findInstanceLayer(layers[i]);
            if (InvalidIndex != layerIndex) {
                this->reqInstLayers[this->numReqInstLayers++] = this->instLayers[layerIndex].layerName;
            }
            else {
                o_warn("vlkContext: requested instance layer '%s' not found!\n", layers[i]);
            }
        }
    }
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
int
vlkContext::findInstanceExtension(const char* name) const {
    o_assert_dbg(name);
    for (uint32 i = 0; i < this->numInstExtensions; i++) {
        if (0 == std::strcmp(name, this->instExtensions[i].extensionName)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstanceExtensions(const char** extensions, int numExtensions) {
    o_assert(nullptr == this->instExtensions);
    o_assert(0 == this->numInstExtensions);
    o_assert(0 == this->numReqInstExtensions);

    // enumerate available extensions
    VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &this->numInstExtensions, nullptr);
    o_assert(!err);
    if (this->numInstExtensions > 0) {
        const int size = this->numInstExtensions * sizeof(VkExtensionProperties);
        this->instExtensions = (VkExtensionProperties*) Memory::Alloc(size);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &this->numInstExtensions, this->instExtensions);
    }

    // dump extension info
    Log::Info(">>> Vulkan Instance Extensions:\n");
    for (uint32 i = 0; i < this->numInstExtensions; i++) {
        const VkExtensionProperties& cur = this->instExtensions[i];
        Log::Info("  %s (ver %x)\n", cur.extensionName, cur.specVersion);
    }

    // filter requested extensions against available extensions
    // FIXME: need to distinguish between optional and required extensions!
    if (extensions) {
        o_assert(numExtensions < maxReqInstExtensions);
        for (int i = 0; i < numExtensions; i++) {
            o_assert_dbg(extensions[i]);
            int extIndex = this->findInstanceExtension(extensions[i]);
            if (InvalidIndex != extIndex) {
                this->reqInstExtensions[this->numReqInstExtensions++] = this->instExtensions[extIndex].extensionName;
            }
            else {
                o_warn("vlkContext: requested instance extension '%s' not found!\n", extensions[i]);
            }
        }
    }
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
    instInfo.enabledLayerCount = this->numReqInstLayers;
    instInfo.ppEnabledLayerNames = this->reqInstLayers;
    instInfo.enabledExtensionCount = this->numReqInstExtensions;
    instInfo.ppEnabledExtensionNames = this->reqInstExtensions;
    
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

} // namespace _priv
} // namespace Oryol
