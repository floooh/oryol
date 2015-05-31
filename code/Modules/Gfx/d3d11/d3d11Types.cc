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
        case PixelFormat::RGB8:
        case PixelFormat::RGBA8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        default:
            o_error("d3d11Types::asSwapChainFormat: not a valid D3D11 display pixel format\n");\
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d11Types::asTextureFormat(PixelFormat::Code pf) {
    switch(pf) {
        case PixelFormat::RGBA8:    return DXGI_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::RGBA32F:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case PixelFormat::RGBA16F:  return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case PixelFormat::L8:       return DXGI_FORMAT_R8_UNORM;
        case PixelFormat::DXT1:     return DXGI_FORMAT_BC1_UNORM;
        case PixelFormat::DXT3:     return DXGI_FORMAT_BC2_UNORM;  
        case PixelFormat::DXT5:     return DXGI_FORMAT_BC3_UNORM;
        case PixelFormat::D16:      return DXGI_FORMAT_D16_UNORM;
        case PixelFormat::D32:      return DXGI_FORMAT_D32_FLOAT;   // FIXME???
        case PixelFormat::D24S8:    return DXGI_FORMAT_D24_UNORM_S8_UINT;
        default:
            // NOTE: a number of common pixel formats are not supported by D3D10, or D3D11 until Windows8:
            // RGB8 (not at all), RGBA4, R5G6B5, R5G5B5A1 (no until D3D11 on Win8) 
            o_error("d3d11Types::asTextureFormat: invalid pixel format for D3D11 texture!\n");
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
D3D11_USAGE
d3d11Types::asBufferUsage(Usage::Code usage) {
    switch (usage) {
        case Usage::Immutable:  return D3D11_USAGE_IMMUTABLE;
        case Usage::Static:     return D3D11_USAGE_DEFAULT;
        case Usage::Dynamic:    return D3D11_USAGE_DYNAMIC;
        case Usage::Stream:     return D3D11_USAGE_DYNAMIC;
        default:
            o_error("invalid usage\n");
            return D3D11_USAGE_IMMUTABLE;
    }
}

//------------------------------------------------------------------------------
uint32
d3d11Types::asBufferCPUAccessFlag(Usage::Code usage) {
    switch (usage) {
        case Usage::Immutable:  return 0;
        case Usage::Static:     return D3D11_CPU_ACCESS_WRITE;
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
    switch (attr) {
        case VertexAttr::Position:  
            return "POSITION";
        case VertexAttr::Normal:   
            return "NORMAL";
        case VertexAttr::TexCoord0:
        case VertexAttr::TexCoord1:
        case VertexAttr::TexCoord2:
        case VertexAttr::TexCoord3:
            return "TEXCOORD";
        case VertexAttr::Tangent:
            return "TANGENT";
        case VertexAttr::Binormal:
            return "BINORMAL";
        case VertexAttr::Weights:
            return "BLENDWEIGHT";
        case VertexAttr::Indices:
            return "BLENDINDICES";
        case VertexAttr::Color0:
        case VertexAttr::Color1:
            return "COLOR";
        case VertexAttr::Instance0:
        case VertexAttr::Instance1:
        case VertexAttr::Instance2:
        case VertexAttr::Instance3:
            return "INSTANCE";
        default:
            o_error("d3d11Types::asSemanticName: invalid vertex attr!\n");
            return nullptr;
    }
}

//------------------------------------------------------------------------------
uint32 
d3d11Types::asSemanticIndex(VertexAttr::Code attr) {
    switch (attr) {
    case VertexAttr::TexCoord1:
    case VertexAttr::Color1:
    case VertexAttr::Instance1:
        return 1;
    case VertexAttr::TexCoord2:
    case VertexAttr::Instance2:
        return 2;
    case VertexAttr::TexCoord3:
    case VertexAttr::Instance3:
        return 3;
    default:
        return 0;
    }
}

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d11Types::asInputElementFormat(VertexFormat::Code fmt) {
    switch (fmt) {
        case VertexFormat::Float:   return DXGI_FORMAT_R32_FLOAT;
        case VertexFormat::Float2:  return DXGI_FORMAT_R32G32_FLOAT;
        case VertexFormat::Float3:  return DXGI_FORMAT_R32G32B32_FLOAT;
        case VertexFormat::Float4:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case VertexFormat::Byte4:   return DXGI_FORMAT_R8G8B8A8_SINT;
        case VertexFormat::Byte4N:  return DXGI_FORMAT_R8G8B8A8_SNORM;
        case VertexFormat::UByte4:  return DXGI_FORMAT_R8G8B8A8_UINT;
        case VertexFormat::UByte4N: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case VertexFormat::Short2:  return DXGI_FORMAT_R16G16_SINT;
        case VertexFormat::Short2N: return DXGI_FORMAT_R16G16_SNORM;
        case VertexFormat::Short4:  return DXGI_FORMAT_R16G16B16A16_SINT;
        case VertexFormat::Short4N: return DXGI_FORMAT_R16G16B16A16_SNORM;
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
        // FIXME FIXME FIXME:
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
uint8
d3d11Types::asColorWriteMask(PixelChannel::Mask mask) {
    uint8 res = 0;
    if (mask & PixelChannel::Red)   res |= D3D11_COLOR_WRITE_ENABLE_RED;
    if (mask & PixelChannel::Green) res |= D3D11_COLOR_WRITE_ENABLE_GREEN;
    if (mask & PixelChannel::Blue)  res |= D3D11_COLOR_WRITE_ENABLE_BLUE;
    if (mask & PixelChannel::Alpha) res |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
    return res;
}

} // namespace _priv
} // namespace Oryol
