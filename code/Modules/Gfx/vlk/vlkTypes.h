#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkTypes
    @brief Vulkan type conversions
*/
#include "Core/Types.h"
#include "vlk_decl.h"

namespace Oryol {
namespace _priv {

class vlkTypes {
public:
    /// convert VkPhysicalDeviceType to static string
    static const char* physicalDeviceTypeAsString(VkPhysicalDeviceType t);
};

} // namespace _priv
} // namespace Oryol
