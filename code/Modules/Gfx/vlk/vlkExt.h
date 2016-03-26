#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkExt
    @brief Vulkan extension and layer helper functions
*/
#include "Core/Containers/Array.h"
#include "Gfx/vlk/vlk_impl.h"

namespace Oryol {
namespace _priv {

class vlkExt {
public:
    /// destructor
    ~vlkExt();

    #if ORYOL_DEBUG
    /// initialize the debug reporting callback extension
    void setupDebugReporting(VkInstance inst);
    /// shutdown debug reporting callback extension
    void discardDebugReporting(VkInstance inst);
    #endif

    /// enumerate instance layers, and match against requested layers
    Array<const char*> queryInstanceLayers(const Array<const char*>& requestedLayers);
    /// enumerate instance extensions, and match against requested extensions
    Array<const char*> queryInstanceExtensions(const Array<const char*>& requestedExtensions);
    /// enumerate device layers, and match against requested layers
    Array<const char*> queryDeviceLayers(VkPhysicalDevice gpu, const Array<const char*>& requestedLayers);
    /// enumerate device extensions, and match against requested extensions
    Array<const char*> queryDeviceExtensions(VkPhysicalDevice gpu, const Array<const char*>& requestedExtensions);

private:
    /// find instance or device layer index, return InvalidIndex if not supported
    static int findLayer(const char* name, const Array<VkLayerProperties>& layers);
    /// select requested layers from available layers
    static Array<const char*> selectLayers(const Array<const char*>& reqLayers, const Array<VkLayerProperties>& availLayers);
    /// find instance or device extension index, return InvalidIndex if not supported
    static int findExtension(const char* name, const Array<VkExtensionProperties>& exts);
    /// select requested extensions from available extensions
    static Array<const char*> selectExtensions(const Array<const char*>& reqExts, const Array<VkExtensionProperties>& availExts);

    #ifdef ORYOL_DEBUG
    VkDebugReportCallbackEXT debugReportCallback = nullptr;
    #endif
    Array<VkLayerProperties> availInstanceLayers;
    Array<VkExtensionProperties> availInstanceExtensions;
    Array<VkLayerProperties> availDeviceLayers;
    Array<VkExtensionProperties> availDeviceExtensions;
};

} // namespace _priv
} // namespace Oryol
