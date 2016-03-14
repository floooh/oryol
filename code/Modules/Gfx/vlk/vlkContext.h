#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkContext
    @brief Vulkan instance, device, swap-chain helpers
*/
#include "Core/Types.h"
#include "Gfx/Setup/GfxSetup.h"
#include "vlk_decl.h"

namespace Oryol {
namespace _priv {

class vlkContext {
public:
    /// destructor
    ~vlkContext();
    /// setup instance and device
    void setup(const GfxSetup& setup, const char** instExtensions, int numInstExtensions);
    /// setup the swap chain
    void setupSwapChain(const GfxSetup& setup, const DisplayAttrs& attrs);
    /// discard everything
    void discard();

    VkInstance Instance = nullptr;
    VkPhysicalDevice PhysicalDevice = nullptr;
    VkDevice Device = nullptr;

private:
    /// enumerate available instance layers, and find requested layers
    void setupInstanceLayers(const char** layers, int numLayers);
    /// discard instance layer list
    void discardInstanceLayers();
    /// enumerate available instance extensions, and find requested extensions
    void setupInstanceExtensions(const char** extensions, int numExtensions);
    /// discard instance extension list
    void discardInstanceExtensions();
    /// setup the Vulkan instance
    void setupInstance(const GfxSetup& setup);
    /// discard the Vulkan instance
    void discardInstance();
    /// enumerate available physical devices, and pick the 'best'
    void setupPhysicalDevice(const GfxSetup& setup);
    /// discard physics devices
    void discardPhysicalDevice();
    /// enumerate available device layers, and select requested layers
    void setupDeviceLayers(const char** layers, int numLayers);
    /// discard device layers list
    void discardDeviceLayers();
    /// enumerate available device extensions, and selected requested extensions
    void setupDeviceExtensions(const char** extensions, int numExtensions);
    /// discard device extension list
    void discardDeviceExtensions();
    /// setup device queue family list
    void setupQueueFamilies();
    /// discard device queue family list
    void discardQueueFamilies();

    /// find instance or device layer index, return InvalidIndex if not supported
    static int findLayer(const char* name, const VkLayerProperties* layers, int numLayers);
    /// dump layer information to console
    static void dumpLayerInfo(const char* title, const VkLayerProperties* layers, int numLayers);
    /// select requested layers from available layers
    static void selectLayers(const char** reqLayers, int numReqLayers, const VkLayerProperties* availLayers, int numAvailLayers, const char** outSelLayers, int& outNumSelLayers);

    /// find instance or device extension index, return InvalidIndex if not supported
    static int findExtension(const char* name, const VkExtensionProperties* exts, int numExts);
    /// dump extension information to console
    static void dumpExtensionInfo(const char* title, const VkExtensionProperties* exts, int numExts);
    /// select requested extensions from available extensions
    static void selectExtensions(const char** reqExts, int numReqExts, const VkExtensionProperties* availExts, int numAvailExts, const char** outSelExts, int& outNumSelExts);

    uint32 numInstLayers = 0;
    VkLayerProperties* instLayers = nullptr;
    uint32 numInstExtensions = 0;
    VkExtensionProperties* instExtensions = nullptr;
    uint32 numPhysDevices = 0;
    VkPhysicalDevice* physDevices = nullptr;
    uint32 numDevLayers = 0;
    VkLayerProperties* devLayers = nullptr;
    uint32 numDevExtensions = 0;
    VkExtensionProperties* devExtensions = nullptr;
    uint32 numQueueFamilies = 0;
    VkQueueFamilyProperties* queueFamilies = nullptr;

    static const int maxSelLayers = 32;
    int numSelInstLayers = 0;
    const char* selInstLayers[maxSelLayers] = { };
    int numSelDevLayers = 0;
    const char* selDevLayers[maxSelLayers] = { };

    static const int maxSelExtensions = 16;
    int numSelInstExtensions = 0;
    const char* selInstExtensions[maxSelExtensions] = { };
    int numSelDevExtensions = 0;
    const char* selDevExtensions[maxSelExtensions] = { };
};

} // namespace _priv
} // namespace Oryol

