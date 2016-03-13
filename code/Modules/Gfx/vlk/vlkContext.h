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
    VkPhysicalDevice Gpu = nullptr;
    VkDevice Device = nullptr;

private:
    /// enumerate available instance layers, and find requested layers
    void setupInstanceLayers(const char** layers, int numLayers);
    /// discard instance layer list
    void discardInstanceLayers();
    /// find instance layer index, or InvalidIndex if not supported
    int findInstanceLayer(const char* name) const;
    /// enumerate available instance extensions, and find requested extensions
    void setupInstanceExtensions(const char** extensions, int numExtensions);
    /// discard instance extension list
    void discardInstanceExtensions();
    /// find instance extension index or InvalidIndex if not supported
    int findInstanceExtension(const char* name) const;
    /// setup the Vulkan instance
    void setupInstance(const GfxSetup& setup);
    /// discard the Vulkan instance
    void discardInstance();

    uint32 numInstLayers = 0;
    VkLayerProperties* instLayers = nullptr;
    uint32 numInstExtensions = 0;
    VkExtensionProperties* instExtensions = nullptr;
    static const int maxReqInstLayers = 16;
    int numReqInstLayers = 0;
    const char* reqInstLayers[maxReqInstLayers] = { };
    int numReqInstExtensions = 0;
    static const int maxReqInstExtensions = 16;
    const char* reqInstExtensions[maxReqInstExtensions] = { };
};

} // namespace _priv
} // namespace Oryol

