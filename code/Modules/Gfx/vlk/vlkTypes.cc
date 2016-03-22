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

//------------------------------------------------------------------------------
VkFormat
vlkTypes::asRenderTargetFormat(PixelFormat::Code pf) {
    switch (pf) {
    case PixelFormat::RGBA8:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case PixelFormat::RGBA32F:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case PixelFormat::RGBA16F:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case PixelFormat::DEPTH:
        return VK_FORMAT_D16_UNORM;
    case PixelFormat::DEPTHSTENCIL:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    default:
        o_error("vlkTypes::asRenderTargetFormat(): invalid pixel format!\n");
        return VK_FORMAT_UNDEFINED;
    }
}

} // namespace _priv
} // namespace Oryol
