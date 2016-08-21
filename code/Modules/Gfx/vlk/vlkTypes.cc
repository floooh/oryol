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
        case PixelFormat::RGBA8:        return VK_FORMAT_B8G8R8A8_UNORM;
        case PixelFormat::RGBA32F:      return VK_FORMAT_R32G32B32A32_SFLOAT;
        case PixelFormat::RGBA16F:      return VK_FORMAT_R16G16B16A16_SFLOAT;
        case PixelFormat::DEPTH:        return VK_FORMAT_D16_UNORM;
        case PixelFormat::DEPTHSTENCIL: return VK_FORMAT_D24_UNORM_S8_UINT;
        default:
            o_error("vlkTypes::asRenderTargetFormat(): invalid pixel format!\n");
            return VK_FORMAT_UNDEFINED;
    }
}

//------------------------------------------------------------------------------
VkFormat
vlkTypes::asVertexFormat(VertexFormat::Code vf) {
    switch (vf) {
        case VertexFormat::Float:   return VK_FORMAT_R32_SFLOAT;
        case VertexFormat::Float2:  return VK_FORMAT_R32G32_SFLOAT;
        case VertexFormat::Float3:  return VK_FORMAT_R32G32B32_SFLOAT;
        case VertexFormat::Float4:  return VK_FORMAT_R32G32B32A32_SFLOAT;
        case VertexFormat::Byte4:   return VK_FORMAT_R8G8B8A8_SINT;
        case VertexFormat::Byte4N:  return VK_FORMAT_R8G8B8A8_SNORM;
        case VertexFormat::UByte4:  return VK_FORMAT_R8G8B8A8_UINT;
        case VertexFormat::UByte4N: return VK_FORMAT_R8G8B8A8_UNORM;
        case VertexFormat::Short2:  return VK_FORMAT_R16G16_SINT; 
        case VertexFormat::Short2N: return VK_FORMAT_R16G16_SNORM;
        case VertexFormat::Short4:  return VK_FORMAT_R16G16B16A16_SINT;
        case VertexFormat::Short4N: return VK_FORMAT_R16G16B16A16_SNORM;
        default:
            o_error("vlkTypes::asVertexFormat(): invalid format!\n");
            return VK_FORMAT_UNDEFINED;
    }
}

//------------------------------------------------------------------------------
VkPrimitiveTopology
vlkTypes::asPrimitiveTopology(PrimitiveType::Code primType) {
    switch (primType) {
        case PrimitiveType::Points:         return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case PrimitiveType::Lines:          return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case PrimitiveType::LineStrip:      return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case PrimitiveType::Triangles:      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case PrimitiveType::TriangleStrip:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        default:
            o_error("vlkTypes::asPrimitiveTopology(): invalid primitive type");
            return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
    }
}

//------------------------------------------------------------------------------
VkCullModeFlags
vlkTypes::asCullMode(bool cullEnabled, Face::Code face) {
    if (cullEnabled) {
        if (Face::Front == face) {
            return VK_CULL_MODE_FRONT_BIT;
        }
        else {
            return VK_CULL_MODE_BACK_BIT;
        }
    }
    else {
        return VK_CULL_MODE_NONE;
    }
}

//------------------------------------------------------------------------------
VkCompareOp
vlkTypes::asCompareOp(CompareFunc::Code cmp) {
    switch (cmp) {
        case CompareFunc::Never:         return VK_COMPARE_OP_NEVER;
        case CompareFunc::Less:          return VK_COMPARE_OP_LESS;
        case CompareFunc::Equal:         return VK_COMPARE_OP_EQUAL;
        case CompareFunc::LessEqual:     return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareFunc::Greater:       return VK_COMPARE_OP_GREATER;
        case CompareFunc::NotEqual:      return VK_COMPARE_OP_NOT_EQUAL;
        case CompareFunc::GreaterEqual:  return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareFunc::Always:        return VK_COMPARE_OP_ALWAYS;
        default:
            o_error("vlkTypes::asCompareOp: invalid compare func value!");
            return VK_COMPARE_OP_MAX_ENUM;
    }
}

//------------------------------------------------------------------------------
VkStencilOp
vlkTypes::asStencilOp(StencilOp::Code op) {
    switch (op) {
        case StencilOp::Keep:       return VK_STENCIL_OP_KEEP;
        case StencilOp::Zero:       return VK_STENCIL_OP_ZERO;
        case StencilOp::Replace:    return VK_STENCIL_OP_REPLACE;
        case StencilOp::IncrClamp:  return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case StencilOp::DecrClamp:  return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case StencilOp::Invert:     return VK_STENCIL_OP_INVERT;
        case StencilOp::IncrWrap:   return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case StencilOp::DecrWrap:   return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            o_error("vlkTypes::asStencilOp: invalid stencil op value!");
            return VK_STENCIL_OP_MAX_ENUM;
    }
}

} // namespace _priv
} // namespace Oryol
