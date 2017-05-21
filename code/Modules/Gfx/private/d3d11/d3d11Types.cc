//------------------------------------------------------------------------------
//  d3d11Types.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Types.h"
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d11Types::asSwapChainFormat(PixelFormat::Code pf) {
    switch (pf) {
        case PixelFormat::RGBA8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        default:
            o_error("d3d11Types::asSwapChainFormat: not a valid D3D11 display pixel format\n");
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d11Types::asRenderTargetFormat(PixelFormat::Code pf) {
    switch (pf) {
        case PixelFormat::RGBA8:        return DXGI_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::RGBA32F:      return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case PixelFormat::RGBA16F:      return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case PixelFormat::R10G10B10A2:  return DXGI_FORMAT_R10G10B10A2_UNORM;
        case PixelFormat::DEPTH:        return DXGI_FORMAT_D16_UNORM;
        case PixelFormat::DEPTHSTENCIL: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        default:
            o_error("d3d11Types::asRenderTargetFormat(): invalid pixel format!\n");
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d11Types::asTextureFormat(PixelFormat::Code pf) {
    switch(pf) {
        case PixelFormat::RGBA8:        return DXGI_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::RGBA4:        return DXGI_FORMAT_B4G4R4A4_UNORM;
        case PixelFormat::R5G6B5:       return DXGI_FORMAT_B5G6R5_UNORM;
        case PixelFormat::R5G5B5A1:     return DXGI_FORMAT_B5G5R5A1_UNORM;
        case PixelFormat::RGBA32F:      return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case PixelFormat::RGBA16F:      return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case PixelFormat::R10G10B10A2:  return DXGI_FORMAT_R10G10B10A2_UNORM;
        case PixelFormat::L8:           return DXGI_FORMAT_R8_UNORM;
        case PixelFormat::DXT1:         return DXGI_FORMAT_BC1_UNORM;
        case PixelFormat::DXT3:         return DXGI_FORMAT_BC2_UNORM;  
        case PixelFormat::DXT5:         return DXGI_FORMAT_BC3_UNORM;
        case PixelFormat::DEPTH:        return DXGI_FORMAT_D16_UNORM;
        case PixelFormat::DEPTHSTENCIL: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        default:
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
D3D11_USAGE
d3d11Types::asResourceUsage(Usage::Code usage) {
    switch (usage) {
        case Usage::Immutable:  return D3D11_USAGE_IMMUTABLE;
        case Usage::Dynamic:    return D3D11_USAGE_DYNAMIC;
        case Usage::Stream:     return D3D11_USAGE_DYNAMIC;
        default:
            o_error("invalid usage\n");
            return D3D11_USAGE_IMMUTABLE;
    }
}

//------------------------------------------------------------------------------
uint32_t
d3d11Types::asResourceCPUAccessFlag(Usage::Code usage) {
    switch (usage) {
        case Usage::Immutable:  return 0;
        case Usage::Dynamic:    return D3D11_CPU_ACCESS_WRITE;
        case Usage::Stream:     return D3D11_CPU_ACCESS_WRITE;
        default:
            o_error("invalid usage\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
const char*
d3d11Types::asSemanticName(VertexAttr::Code attr) {
    // not a bug, SPIRV-Cross uses TEXCOORD as universal semantic name
    return "TEXCOORD";
}

//------------------------------------------------------------------------------
uint32_t
d3d11Types::asSemanticIndex(VertexAttr::Code attr) {
    // not a bug, see oryol-shdc tool
    return (uint32_t) attr;
}

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d11Types::asInputElementFormat(VertexFormat::Code fmt) {
    switch (fmt) {
        case VertexFormat::Short2:      return DXGI_FORMAT_R16G16_SINT;
        case VertexFormat::Float:       return DXGI_FORMAT_R32_FLOAT;
        case VertexFormat::Float2:      return DXGI_FORMAT_R32G32_FLOAT;
        case VertexFormat::Float3:      return DXGI_FORMAT_R32G32B32_FLOAT;
        case VertexFormat::Float4:      return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case VertexFormat::Byte4:       return DXGI_FORMAT_R8G8B8A8_SINT;
        case VertexFormat::Byte4N:      return DXGI_FORMAT_R8G8B8A8_SNORM;
        case VertexFormat::UByte4:      return DXGI_FORMAT_R8G8B8A8_UINT;
        case VertexFormat::UByte4N:     return DXGI_FORMAT_R8G8B8A8_UNORM;
        case VertexFormat::Short2N:     return DXGI_FORMAT_R16G16_SNORM;
        case VertexFormat::Short4:      return DXGI_FORMAT_R16G16B16A16_SINT;
        case VertexFormat::Short4N:     return DXGI_FORMAT_R16G16B16A16_SNORM;
        default:
            o_error("d3d11Types::asInputElementFormat: invalid vertex format!\n");
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
D3D11_CULL_MODE
d3d11Types::asCullMode(bool cullEnabled, Face::Code face) {
    if (cullEnabled) {
        if (Face::Front == face) {
            return D3D11_CULL_FRONT;
        }
        else {
            return D3D11_CULL_BACK;
        }
    }
    else {
        return D3D11_CULL_NONE;
    }
}

//------------------------------------------------------------------------------
D3D11_COMPARISON_FUNC
d3d11Types::asComparisonFunc(CompareFunc::Code func) {
    switch (func) {
        case CompareFunc::Never:        return D3D11_COMPARISON_NEVER;    
        case CompareFunc::Less:         return D3D11_COMPARISON_LESS;
        case CompareFunc::Equal:        return D3D11_COMPARISON_EQUAL;
        case CompareFunc::LessEqual:    return D3D11_COMPARISON_LESS_EQUAL;  
        case CompareFunc::Greater:      return D3D11_COMPARISON_GREATER;
        case CompareFunc::NotEqual:     return D3D11_COMPARISON_NOT_EQUAL;
        case CompareFunc::GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
        default:                        return D3D11_COMPARISON_ALWAYS;
    }
}

//------------------------------------------------------------------------------
D3D11_STENCIL_OP
d3d11Types::asStencilOp(StencilOp::Code op) {
    switch (op) {
        case StencilOp::Keep:       return D3D11_STENCIL_OP_KEEP;   
        case StencilOp::Zero:       return D3D11_STENCIL_OP_ZERO;
        case StencilOp::Replace:    return D3D11_STENCIL_OP_REPLACE;
        case StencilOp::IncrClamp:  return D3D11_STENCIL_OP_INCR_SAT;
        case StencilOp::DecrClamp:  return D3D11_STENCIL_OP_DECR_SAT;
        case StencilOp::Invert:     return D3D11_STENCIL_OP_INVERT;
        case StencilOp::IncrWrap:   return D3D11_STENCIL_OP_INCR;
        default:                    return D3D11_STENCIL_OP_DECR;
    }
}

//------------------------------------------------------------------------------
D3D11_BLEND
d3d11Types::asBlendFactor(BlendFactor::Code b) {
    switch (b) {
        case BlendFactor::Zero:                 return D3D11_BLEND_ZERO;
        case BlendFactor::One:                  return D3D11_BLEND_ONE;
        case BlendFactor::SrcColor:             return D3D11_BLEND_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:     return D3D11_BLEND_INV_SRC_COLOR;
        case BlendFactor::SrcAlpha:             return D3D11_BLEND_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:     return D3D11_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DstColor:             return D3D11_BLEND_DEST_COLOR;
        case BlendFactor::OneMinusDstColor:     return D3D11_BLEND_INV_DEST_COLOR;
        case BlendFactor::DstAlpha:             return D3D11_BLEND_DEST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:     return D3D11_BLEND_INV_DEST_ALPHA;
        case BlendFactor::SrcAlphaSaturated:    return D3D11_BLEND_SRC_ALPHA_SAT;
        case BlendFactor::BlendColor:           return D3D11_BLEND_BLEND_FACTOR;
        case BlendFactor::OneMinusBlendColor:   return D3D11_BLEND_INV_BLEND_FACTOR;
        case BlendFactor::BlendAlpha:           return D3D11_BLEND_BLEND_FACTOR;
        case BlendFactor::OneMinusBlendAlpha:   return D3D11_BLEND_INV_BLEND_FACTOR;
        default: return D3D11_BLEND_ONE;
    }
}

//------------------------------------------------------------------------------
D3D11_BLEND_OP
d3d11Types::asBlendOp(BlendOperation::Code op) {
    // FIXME: D3D11 also has MIN and MAX
    switch (op) {
        case BlendOperation::Add:               return D3D11_BLEND_OP_ADD;   
        case BlendOperation::Subtract:          return D3D11_BLEND_OP_SUBTRACT;
        case BlendOperation::ReverseSubtract:   return D3D11_BLEND_OP_REV_SUBTRACT;
        default: return D3D11_BLEND_OP_ADD;
    }
}

//------------------------------------------------------------------------------
uint8_t
d3d11Types::asColorWriteMask(PixelChannel::Mask mask) {
    uint8_t res = 0;
    if (mask & PixelChannel::Red)   res |= D3D11_COLOR_WRITE_ENABLE_RED;
    if (mask & PixelChannel::Green) res |= D3D11_COLOR_WRITE_ENABLE_GREEN;
    if (mask & PixelChannel::Blue)  res |= D3D11_COLOR_WRITE_ENABLE_BLUE;
    if (mask & PixelChannel::Alpha) res |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
    return res;
}

//------------------------------------------------------------------------------
D3D11_INPUT_CLASSIFICATION
d3d11Types::asInputClassification(VertexStepFunction::Code func) {
    return func == VertexStepFunction::PerVertex ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
}

//------------------------------------------------------------------------------
D3D11_FILTER
d3d11Types::asSamplerFilter(TextureFilterMode::Code magFilter, TextureFilterMode::Code minFilter) {
    if (TextureFilterMode::Nearest == magFilter) {
        switch (minFilter) {
            case TextureFilterMode::Nearest:
            case TextureFilterMode::NearestMipmapNearest:
                return D3D11_FILTER_MIN_MAG_MIP_POINT;
            case TextureFilterMode::Linear:
            case TextureFilterMode::LinearMipmapNearest:
                return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            case TextureFilterMode::NearestMipmapLinear:
                return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
            case TextureFilterMode::LinearMipmapLinear:
                return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            default:
                break;
        }
    }
    else if (TextureFilterMode::Linear == magFilter) {
        switch (magFilter) {
            case TextureFilterMode::Nearest:
            case TextureFilterMode::NearestMipmapNearest:
                return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            case TextureFilterMode::Linear:
            case TextureFilterMode::LinearMipmapNearest:
                return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            case TextureFilterMode::NearestMipmapLinear:
                return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            case TextureFilterMode::LinearMipmapLinear:
                return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            default:
                break;
        }
    }
    // fallthrough: invalid filter combination
    o_error("d3d11Types::asFilter(): invalid filter combination!\n");
    return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

//------------------------------------------------------------------------------
D3D11_TEXTURE_ADDRESS_MODE
d3d11Types::asTextureAddressMode(TextureWrapMode::Code mode) {
    switch (mode) {
        case TextureWrapMode::ClampToEdge:   
            return D3D11_TEXTURE_ADDRESS_CLAMP;
        case TextureWrapMode::Repeat:        
            return D3D11_TEXTURE_ADDRESS_WRAP;
        case TextureWrapMode::MirroredRepeat:
            return D3D11_TEXTURE_ADDRESS_MIRROR;
        default:
            return D3D11_TEXTURE_ADDRESS_CLAMP;
    }
}

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d11Types::asIndexType(IndexType::Code c) {
    switch (c) {
        case IndexType::None:       return DXGI_FORMAT_UNKNOWN; // this is a valid return type!   
        case IndexType::Index16:    return DXGI_FORMAT_R16_UINT;
        case IndexType::Index32:    return DXGI_FORMAT_R32_UINT;
        default:
            o_error("d3d11Types::asIndexType(): invalid value!\n");
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
D3D11_PRIMITIVE_TOPOLOGY
d3d11Types::asPrimitiveTopology(PrimitiveType::Code c) {
    switch (c) {
        case PrimitiveType::Points:         return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case PrimitiveType::Lines:          return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case PrimitiveType::LineStrip:      return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case PrimitiveType::Triangles:      return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case PrimitiveType::TriangleStrip:  return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default:
            o_error("d3d11Types::asPrimitiveTopology(): invalid value!\n");
            return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    }
}

} // namespace _priv
} // namespace Oryol
