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
    void setup(const GfxSetup& setup);
    /// setup the swap chain
    void setupSwapChain(const GfxSetup& setup, const DisplayAttrs& attrs);
    /// discard everything
    void discard();

    VkInstance Instance = nullptr;
    VkPhysicalDevice Gpu = nullptr;
    VkDevice Device = nullptr;

private:
    /// enumerate available instance layers
    void setupInstanceLayers();
    /// discard instance layer list
    void discardInstanceLayers();
    /// enumerate available instance extensions
    void setupInstanceExtensions();
    /// discard instance extension list
    void discardInstanceExtensions();

    uint32 numInstanceLayers = 0;
    VkLayerProperties* instanceLayers = nullptr;
    uint32 numInstanceExtensions = 0;
    VkExtensionProperties* instanceExtensions = nullptr;
};

} // namespace _priv
} // namespace Oryol

