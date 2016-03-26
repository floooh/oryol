//------------------------------------------------------------------------------
//  vlkExt.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkExt.h"

static PFN_vkCreateDebugReportCallbackEXT fpCreateDebugReportCallbackEXT = nullptr;
static PFN_vkDestroyDebugReportCallbackEXT fpDestroyDebugReportCallbackEXT = nullptr;

namespace Oryol {
namespace _priv {

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
vlkExt::~vlkExt() {
    #if ORYOL_DEBUG
    o_assert(nullptr == this->debugReportCallback);
    #endif
}

//------------------------------------------------------------------------------
#if ORYOL_DEBUG
void
vlkExt::setupDebugReporting(VkInstance inst) {
    o_assert_dbg(inst);
    o_assert_dbg(nullptr == this->debugReportCallback);

    INST_FUNC_PTR(inst, CreateDebugReportCallbackEXT);
    INST_FUNC_PTR(inst, DestroyDebugReportCallbackEXT);

    VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
    dbgCreateInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    dbgCreateInfo.pfnCallback = errorReportingCallback;
    VkResult err = fpCreateDebugReportCallbackEXT(inst, &dbgCreateInfo, nullptr, &this->debugReportCallback);
    o_assert(!err && this->debugReportCallback);
}
#endif

//------------------------------------------------------------------------------
#if ORYOL_DEBUG
void
vlkExt::discardDebugReporting(VkInstance inst) {
    o_assert_dbg(inst);
    o_assert_dbg(this->debugReportCallback);
    o_assert_dbg(fpDestroyDebugReportCallbackEXT);
    fpDestroyDebugReportCallbackEXT(inst, this->debugReportCallback, nullptr);
    this->debugReportCallback = nullptr;
}
#endif

//------------------------------------------------------------------------------
Array<const char*>
vlkExt::queryInstanceLayers(const Array<const char*>& requestedLayers) {
    uint32_t num = 0;
    VkResult err = vkEnumerateInstanceLayerProperties(&num, nullptr);
    o_assert(!err);
    if (num > 0) {
        this->availInstanceLayers.Resize(num);
        err = vkEnumerateInstanceLayerProperties(&num, this->availInstanceLayers.Data());
    }
    Log::Info("Available Instance Layers:\n");
    for (const auto& l : this->availInstanceLayers) {
        Log::Info("  %s\n", l.layerName);
    }
    return selectLayers(requestedLayers, this->availInstanceLayers);
}

//------------------------------------------------------------------------------
Array<const char*>
vlkExt::queryInstanceExtensions(const Array<const char*>& requestedExtensions) {
    uint32_t num = 0;
    VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &num, nullptr);
    o_assert(!err);
    if (num > 0) {
        this->availInstanceExtensions.Resize(num);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &num, this->availInstanceExtensions.Data());
    }
    Log::Info("Available Instance Extensions:\n");
    for (const auto& ext : this->availInstanceExtensions) {
        Log::Info("  %s\n", ext.extensionName);
    }
    return selectExtensions(requestedExtensions, this->availInstanceExtensions);
}

//------------------------------------------------------------------------------
Array<const char*>
vlkExt::queryDeviceLayers(VkPhysicalDevice gpu, const Array<const char*>& requestedLayers) {
    o_assert_dbg(gpu);
    uint32_t num = 0;
    VkResult err = vkEnumerateDeviceLayerProperties(gpu, &num, nullptr);
    o_assert(!err);
    if (num > 0) {
        this->availDeviceLayers.Resize(num);
        err = vkEnumerateDeviceLayerProperties(gpu, &num, this->availDeviceLayers.Data());
    }
    Log::Info("Device Layers:\n");
    for (const auto& l : this->availDeviceLayers) {
        Log::Info("  %s\n", l.layerName);
    }
    return selectLayers(requestedLayers, this->availDeviceLayers);
}

//------------------------------------------------------------------------------
Array<const char*>
vlkExt::queryDeviceExtensions(VkPhysicalDevice gpu, const Array<const char*>& requestedExtensions) {
    o_assert_dbg(gpu);
    uint32_t num = 0;
    VkResult err = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &num, nullptr);
    o_assert(!err);
    if (num > 0) {
        this->availDeviceExtensions.Resize(num);
        err = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &num, this->availDeviceExtensions.Data());
    }
    Log::Info("Device Extensions:\n");
    for (const auto& ext : this->availDeviceExtensions) {
        Log::Info("  %s\n", ext.extensionName);
    }
    return selectExtensions(requestedExtensions, this->availDeviceExtensions);
}

//------------------------------------------------------------------------------
int
vlkExt::findLayer(const char* name, const Array<VkLayerProperties>& layers) {
    o_assert_dbg(name);
    for (int i = 0; i < layers.Size(); i++) {
        if (0 == std::strcmp(name, layers[i].layerName)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
Array<const char*>
vlkExt::selectLayers(const Array<const char*>& reqLayers, const Array<VkLayerProperties>& availLayers) {
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
    return result;
}

//------------------------------------------------------------------------------
int
vlkExt::findExtension(const char* name, const Array<VkExtensionProperties>& exts) {
    o_assert_dbg(name);
    for (int i = 0; i < exts.Size(); i++) {
        if (0 == std::strcmp(name, exts[i].extensionName)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
Array<const char*>
vlkExt::selectExtensions(const Array<const char*>& reqExts, const Array<VkExtensionProperties>& availExts) {
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
    return result;
}

} // namespace _priv
} // namespace Oryol
