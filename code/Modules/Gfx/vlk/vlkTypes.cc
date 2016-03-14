//------------------------------------------------------------------------------
//  vlkTypes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlk_impl.h"
#include "vlkTypes.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
const char*
vlkTypes::physicalDeviceTypeAsString(VkPhysicalDeviceType t) {
    switch (t) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER: return "OTHER";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "INTEGRATED_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "DISCRETE_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
        default: return "UNKNOWN";
    }
}

} // namespace _priv
} // namespace Oryol
