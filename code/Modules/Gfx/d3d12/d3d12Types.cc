//------------------------------------------------------------------------------
//  d3d12Types.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Types.h"
#include "d3d12_impl.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d12Types::asSwapChainFormat(PixelFormat::Code pixelFormat) {
    switch (pixelFormat) {
        case PixelFormat::RGB8:
        case PixelFormat::RGBA8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        default:
            o_error("d3d12Types::asSwapChainFormat: not a valid DXGI display pixel format\n");
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d12Types::asRenderTargetFormat(PixelFormat::Code pf) {
    switch (pf) {
        case PixelFormat::RGB8:
        case PixelFormat::RGBA8:    
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::RGBA32F:  
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case PixelFormat::RGBA16F:  
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case PixelFormat::D16:      
            return DXGI_FORMAT_D16_UNORM;
        case PixelFormat::D32:      
            return DXGI_FORMAT_D32_FLOAT;
        case PixelFormat::D24S8:    
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        default:
            o_error("d3d12Types::asRenderTargetFormat(): invalid pixel format!\n");
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
D3D12_BLEND
d3d12Types::asBlendFactor(BlendFactor::Code b) {
    switch (b) {
        case BlendFactor::One:                  return D3D12_BLEND_ONE;
        case BlendFactor::Zero:                 return D3D12_BLEND_ZERO;
        case BlendFactor::SrcColor:             return D3D12_BLEND_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:     return D3D12_BLEND_INV_SRC_COLOR;
        case BlendFactor::SrcAlpha:             return D3D12_BLEND_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:     return D3D12_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DstColor:             return D3D12_BLEND_DEST_COLOR;
        case BlendFactor::OneMinusDstColor:     return D3D12_BLEND_INV_DEST_COLOR;
        case BlendFactor::DstAlpha:             return D3D12_BLEND_DEST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:     return D3D12_BLEND_INV_DEST_ALPHA;
        case BlendFactor::SrcAlphaSaturated:    return D3D12_BLEND_SRC_ALPHA_SAT;
        case BlendFactor::BlendColor:           return D3D12_BLEND_BLEND_FACTOR;
        case BlendFactor::OneMinusBlendColor:   return D3D12_BLEND_INV_BLEND_FACTOR;
            // FIXME FIXME FIXME:
        case BlendFactor::BlendAlpha:           return D3D12_BLEND_BLEND_FACTOR;
        case BlendFactor::OneMinusBlendAlpha:   return D3D12_BLEND_INV_BLEND_FACTOR;
        default: return D3D12_BLEND_ONE;
    }
}

//------------------------------------------------------------------------------
D3D12_BLEND_OP
d3d12Types::asBlendOp(BlendOperation::Code op) {
    // FIXME: D3D12 also has MIN and MAX
    switch (op) {
        case BlendOperation::Add:               return D3D12_BLEND_OP_ADD;
        case BlendOperation::Subtract:          return D3D12_BLEND_OP_SUBTRACT;
        case BlendOperation::ReverseSubtract:   return D3D12_BLEND_OP_REV_SUBTRACT;
        default: return D3D12_BLEND_OP_ADD;
    }
}

//------------------------------------------------------------------------------
uint8
d3d12Types::asColorWriteMask(PixelChannel::Mask mask) {
    uint8 res = 0;
    if (mask & PixelChannel::Red)   res |= D3D12_COLOR_WRITE_ENABLE_RED;
    if (mask & PixelChannel::Green) res |= D3D12_COLOR_WRITE_ENABLE_GREEN;
    if (mask & PixelChannel::Blue)  res |= D3D12_COLOR_WRITE_ENABLE_BLUE;
    if (mask & PixelChannel::Alpha) res |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
    return res;
}

//------------------------------------------------------------------------------
D3D12_CULL_MODE
d3d12Types::asCullMode(bool cullEnabled, Face::Code face) {
    if (cullEnabled) {
        if (Face::Front == face) {
            return D3D12_CULL_MODE_FRONT;
        }
        else {
            return D3D12_CULL_MODE_BACK;
        }
    }
    else {
        return D3D12_CULL_MODE_NONE;
    }
}

//------------------------------------------------------------------------------
D3D12_COMPARISON_FUNC
d3d12Types::asComparisonFunc(CompareFunc::Code func) {
    switch (func) {
        case CompareFunc::Never:        return D3D12_COMPARISON_FUNC_NEVER;
        case CompareFunc::Less:         return D3D12_COMPARISON_FUNC_LESS;
        case CompareFunc::Equal:        return D3D12_COMPARISON_FUNC_EQUAL;
        case CompareFunc::LessEqual:    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case CompareFunc::Greater:      return D3D12_COMPARISON_FUNC_GREATER;
        case CompareFunc::NotEqual:     return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case CompareFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        default:                        return D3D12_COMPARISON_FUNC_ALWAYS;
    }
}

//------------------------------------------------------------------------------
D3D12_STENCIL_OP
d3d12Types::asStencilOp(StencilOp::Code op) {
    switch (op) {
        case StencilOp::Keep:       return D3D12_STENCIL_OP_KEEP;
        case StencilOp::Zero:       return D3D12_STENCIL_OP_ZERO;
        case StencilOp::Replace:    return D3D12_STENCIL_OP_REPLACE;
        case StencilOp::IncrClamp:  return D3D12_STENCIL_OP_INCR_SAT;
        case StencilOp::DecrClamp:  return D3D12_STENCIL_OP_DECR_SAT;
        case StencilOp::Invert:     return D3D12_STENCIL_OP_INVERT;
        case StencilOp::IncrWrap:   return D3D12_STENCIL_OP_INCR;
        default:                    return D3D12_STENCIL_OP_DECR;
    }
}

//------------------------------------------------------------------------------
D3D12_PRIMITIVE_TOPOLOGY
d3d12Types::asPrimitiveTopology(PrimitiveType::Code c) {
    // NOTE: there are no D3D12_PRIMITIVE_TOPOLOGY_xxx enum values
    switch (c) {
        case PrimitiveType::Points:         return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case PrimitiveType::Lines:          return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case PrimitiveType::LineStrip:      return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case PrimitiveType::Triangles:      return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case PrimitiveType::TriangleStrip:  return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default:
            o_error("d3d12Types::asPrimitiveTopology(): invalid value!\n");
            return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    }
}

//------------------------------------------------------------------------------
D3D12_PRIMITIVE_TOPOLOGY_TYPE
d3d12Types::asPrimitiveTopologyType(PrimitiveType::Code c) {
    switch (c) {
        case PrimitiveType::Points:     
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case PrimitiveType::Lines:
        case PrimitiveType::LineStrip:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case PrimitiveType::Triangles:
        case PrimitiveType::TriangleStrip:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        default:
            o_error("d3d12Types::asPrimitiveTopologyType(): invalid value!\n");
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }
}

//------------------------------------------------------------------------------
const char*
d3d12Types::asSemanticName(VertexAttr::Code attr) {
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
            o_error("d3d12Types::asSemanticName: invalid vertex attr!\n");
            return nullptr;
    }
}

//------------------------------------------------------------------------------
uint32
d3d12Types::asSemanticIndex(VertexAttr::Code attr) {
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
d3d12Types::asInputElementFormat(VertexFormat::Code fmt) {
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
            o_error("d3d12Types::asInputElementFormat: invalid vertex format!\n");
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
D3D12_INPUT_CLASSIFICATION
d3d12Types::asInputClassification(VertexStepFunction::Code func) {
    if (VertexStepFunction::PerVertex == func) {
        return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    }
    else {
        return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
    }
}

//------------------------------------------------------------------------------
DXGI_FORMAT
d3d12Types::asIndexType(IndexType::Code c) {
    switch (c) {
        case IndexType::None:       return DXGI_FORMAT_UNKNOWN; // this is a valid return type!   
        case IndexType::Index16:    return DXGI_FORMAT_R16_UINT;
        case IndexType::Index32:    return DXGI_FORMAT_R32_UINT;
        default:
            o_error("d3d12Types::asIndexType(): invalid value!\n");
            return DXGI_FORMAT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
void
d3d12Types::initHeapProps(D3D12_HEAP_PROPERTIES* out, D3D12_HEAP_TYPE type) {
    o_assert_dbg(out);
    Memory::Clear(out, sizeof(D3D12_HEAP_PROPERTIES));
    out->Type = type;
}

//------------------------------------------------------------------------------
void
d3d12Types::initRTResourceDesc(D3D12_RESOURCE_DESC* out, int width, int height, PixelFormat::Code fmt, int sampleCount) {
    o_assert_dbg(out);
    Memory::Clear(out, sizeof(D3D12_RESOURCE_DESC));
    out->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    out->Width = width;
    out->Height = height;
    out->DepthOrArraySize = 1;
    out->MipLevels = 1;
    out->Format = d3d12Types::asRenderTargetFormat(fmt);
    out->SampleDesc.Count = sampleCount;
    out->Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    if (PixelFormat::IsValidRenderTargetColorFormat(fmt)) {
        out->Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }
    else if (PixelFormat::IsDepthStencilFormat(fmt)) {
        out->Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }
}

//------------------------------------------------------------------------------
void
d3d12Types::initBufferResourceDesc(D3D12_RESOURCE_DESC* out, int size) {
    o_assert_dbg(out);
    Memory::Clear(out, sizeof(D3D12_RESOURCE_DESC));
    out->Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    out->Alignment = 0;
    out->Width = size;
    out->Height = 1;
    out->DepthOrArraySize = 1;
    out->MipLevels = 1;
    out->Format = DXGI_FORMAT_UNKNOWN;
    out->SampleDesc.Count = 1;
    out->SampleDesc.Quality = 0;
    out->Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    out->Flags = D3D12_RESOURCE_FLAG_NONE;
}

//------------------------------------------------------------------------------
void
d3d12Types::initColorClearValue(D3D12_CLEAR_VALUE* out, PixelFormat::Code fmt, float r, float g, float b, float a) {
    o_assert_dbg(out);
    Memory::Clear(out, sizeof(D3D12_CLEAR_VALUE));
    out->Format = d3d12Types::asRenderTargetFormat(fmt);
    out->Color[0] = r; 
    out->Color[1] = g; 
    out->Color[2] = b;
    out->Color[3] = a;
}

//------------------------------------------------------------------------------
void
d3d12Types::initDepthStencilClearValue(D3D12_CLEAR_VALUE* out, PixelFormat::Code fmt, float d, uint8 s) {
    o_assert_dbg(out);
    Memory::Clear(out, sizeof(D3D12_CLEAR_VALUE));
    out->Format = d3d12Types::asRenderTargetFormat(fmt);
    out->DepthStencil.Depth = d;
    out->DepthStencil.Stencil = s;
}

//------------------------------------------------------------------------------
void
d3d12Types::initDescriptorHeapDesc(D3D12_DESCRIPTOR_HEAP_DESC* out, int num, D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible) {
    o_assert_dbg(out);
    Memory::Clear(out, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
    out->NumDescriptors = num;
    out->Type = type;
    out->Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
}

//------------------------------------------------------------------------------
void
d3d12Types::initDSVDesc(D3D12_DEPTH_STENCIL_VIEW_DESC* out, PixelFormat::Code fmt, bool isMSAA) {
    o_assert_dbg(out);
    Memory::Clear(out, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
    out->Format = d3d12Types::asRenderTargetFormat(fmt);
    out->ViewDimension = isMSAA ? D3D12_DSV_DIMENSION_TEXTURE2DMS : D3D12_DSV_DIMENSION_TEXTURE2D;
    out->Flags = D3D12_DSV_FLAG_NONE;
}

} // namespace _priv
} // namespace Oryol
