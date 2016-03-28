#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkTypes
    @brief Vulkan type conversions
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "vlk_impl.h"

namespace Oryol {
namespace _priv {

class vlkTypes {
public:
    /// convert VkPhysicalDeviceType to static string
    static const char* physicalDeviceTypeAsString(VkPhysicalDeviceType t);
    /// convert PixelFormat to a renderable Vulkan format
    static VkFormat asRenderTargetFormat(PixelFormat::Code pixelFormat);
};

} // namespace _priv
} // namespace Oryol
