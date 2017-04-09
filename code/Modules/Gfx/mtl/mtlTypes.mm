//------------------------------------------------------------------------------
//  mtlTypes.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTypes.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
MTLPixelFormat
mtlTypes::asRenderTargetColorFormat(PixelFormat::Code fmt) {
    // see the capability table for the list of renderable pixel formats:
    // https://developer.apple.com/library/prerelease/ios/documentation/Metal/Reference/MetalConstants_Ref/index.html
    //
    switch (fmt) {
        case PixelFormat::RGBA8:
            // this is a bit of a hack since the default backbuffer format is BGRA8
            return MTLPixelFormatBGRA8Unorm;
        case PixelFormat::RGBA32F:
            return MTLPixelFormatRGBA32Float;
        case PixelFormat::RGBA16F:
            return MTLPixelFormatRGBA16Float;
        case PixelFormat::R10G10B10A2:
            return MTLPixelFormatRGB10A2Unorm;
        default:
            return MTLPixelFormatInvalid;
    }
}

//------------------------------------------------------------------------------
MTLPixelFormat
mtlTypes::asRenderTargetDepthFormat(PixelFormat::Code fmt) {
    switch (fmt) {
        case PixelFormat::DEPTH:
            return MTLPixelFormatDepth32Float;
        case PixelFormat::DEPTHSTENCIL:
            // NOTE: MTLPixelFormatDepth24Unorm_Stencil8 isn't universally supported,
            // instead always use Depth32Float_Stencil8
            return MTLPixelFormatDepth32Float_Stencil8;
        default:
            return MTLPixelFormatInvalid;
    }
}

//------------------------------------------------------------------------------
MTLPixelFormat
mtlTypes::asRenderTargetStencilFormat(PixelFormat::Code fmt) {
    switch (fmt) {
        case PixelFormat::DEPTHSTENCIL:
            return MTLPixelFormatDepth32Float_Stencil8;
        default:
            return MTLPixelFormatInvalid;
    }
}

//------------------------------------------------------------------------------
MTLPixelFormat
mtlTypes::asTextureFormat(PixelFormat::Code fmt) {
    switch (fmt) {
        case PixelFormat::RGBA8:    return MTLPixelFormatRGBA8Unorm;
        case PixelFormat::R10G10B10A2:  return MTLPixelFormatRGB10A2Unorm;
        case PixelFormat::RGBA32F:  return MTLPixelFormatRGBA32Float;
        case PixelFormat::RGBA16F:  return MTLPixelFormatRGBA16Float;
        case PixelFormat::L8:       return MTLPixelFormatR8Unorm;
        #if ORYOL_MACOS
        case PixelFormat::DXT1:     return MTLPixelFormatBC1_RGBA;
        case PixelFormat::DXT3:     return MTLPixelFormatBC2_RGBA;
        case PixelFormat::DXT5:     return MTLPixelFormatBC3_RGBA;
        #elif ORYOL_IOS
        case PixelFormat::PVRTC2_RGB:   return MTLPixelFormatPVRTC_RGB_2BPP;
        case PixelFormat::PVRTC2_RGBA:  return MTLPixelFormatPVRTC_RGBA_2BPP;
        case PixelFormat::PVRTC4_RGB:   return MTLPixelFormatPVRTC_RGB_4BPP;
        case PixelFormat::PVRTC4_RGBA:  return MTLPixelFormatPVRTC_RGBA_4BPP;
        #endif
        default: return MTLPixelFormatInvalid;
    }
}

//------------------------------------------------------------------------------
MTLTextureType
mtlTypes::asTextureType(TextureType::Code type) {
    switch (type) {
        case TextureType::Texture2D:    return MTLTextureType2D;
        case TextureType::Texture3D:    return MTLTextureType3D;
        case TextureType::TextureCube:  return MTLTextureTypeCube;
        case TextureType::TextureArray: return MTLTextureType2DArray;
        default:
            o_error("mtlTypes::asTextureType(): invalid value!\n");
            return MTLTextureType1D;
    }
}

//------------------------------------------------------------------------------
MTLSamplerAddressMode
mtlTypes::asSamplerAddressMode(TextureWrapMode::Code mode) {
    switch (mode) {
        case TextureWrapMode::ClampToEdge:      return MTLSamplerAddressModeClampToEdge;
        case TextureWrapMode::Repeat:           return MTLSamplerAddressModeRepeat;
        case TextureWrapMode::MirroredRepeat:   return MTLSamplerAddressModeMirrorRepeat;
        default:
            o_error("mtlTypes::asSamplerAddressMode(): invalid value!\n");
            return MTLSamplerAddressModeRepeat;
    }
}

//------------------------------------------------------------------------------
MTLSamplerMinMagFilter
mtlTypes::asSamplerMinMagFilter(TextureFilterMode::Code f) {
    switch (f) {
        case TextureFilterMode::Nearest:
        case TextureFilterMode::NearestMipmapNearest:
        case TextureFilterMode::NearestMipmapLinear:
            return MTLSamplerMinMagFilterNearest;
        case TextureFilterMode::Linear:
        case TextureFilterMode::LinearMipmapNearest:
        case TextureFilterMode::LinearMipmapLinear:
            return MTLSamplerMinMagFilterLinear;
        default:
            o_error("mtlTypes::asSamplerMinMagFilter(): invalid value!\n");
            return MTLSamplerMinMagFilterNearest;
    }
}

//------------------------------------------------------------------------------
MTLSamplerMipFilter
mtlTypes::asSamplerMipFilter(TextureFilterMode::Code f) {
    switch (f) {
        case TextureFilterMode::Nearest:
        case TextureFilterMode::Linear:
            return MTLSamplerMipFilterNotMipmapped;
        case TextureFilterMode::NearestMipmapNearest:
        case TextureFilterMode::LinearMipmapNearest:
            return MTLSamplerMipFilterNearest;
        case TextureFilterMode::NearestMipmapLinear:
        case TextureFilterMode::LinearMipmapLinear:
            return MTLSamplerMipFilterLinear;
        default:
            o_error("mtlTypes::asSamplerMipFilter(): invalid value!\n");
            return MTLSamplerMipFilterNotMipmapped;
    }
}

//------------------------------------------------------------------------------
MTLResourceOptions
mtlTypes::asBufferResourceOptions(Usage::Code usage) {
    switch (usage) {
        case Usage::Immutable:  return MTLResourceStorageModeShared;
        // FIXME: are these the right dynamic access flags?
        #if ORYOL_MACOS
        case Usage::Dynamic:    return MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeManaged;
        case Usage::Stream:     return MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeManaged;
        #else
        case Usage::Dynamic:    return MTLResourceCPUCacheModeWriteCombined;
        case Usage::Stream:     return MTLResourceCPUCacheModeWriteCombined;
        #endif
        default:
            o_error("mtlTypes::asBufferResourceOptions(): invalid value!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
MTLCompareFunction
mtlTypes::asCompareFunc(CompareFunc::Code cmp) {
    switch (cmp) {
        case CompareFunc::Never:        return MTLCompareFunctionNever;
        case CompareFunc::Less:         return MTLCompareFunctionLess;
        case CompareFunc::Equal:        return MTLCompareFunctionEqual;
        case CompareFunc::LessEqual:    return MTLCompareFunctionLessEqual;
        case CompareFunc::Greater:      return MTLCompareFunctionGreater;
        case CompareFunc::NotEqual:     return MTLCompareFunctionNotEqual;
        case CompareFunc::GreaterEqual: return MTLCompareFunctionGreaterEqual;
        case CompareFunc::Always:       return MTLCompareFunctionAlways;
        default:
            o_error("mtlTypes::asCompareFunc(): invalid value!\n");
            return MTLCompareFunctionAlways;
    }
}

//------------------------------------------------------------------------------
MTLStencilOperation
mtlTypes::asStencilOp(StencilOp::Code op) {
    switch (op) {
        case StencilOp::Keep:       return MTLStencilOperationKeep;
        case StencilOp::Zero:       return MTLStencilOperationZero;
        case StencilOp::Replace:    return MTLStencilOperationReplace;
        case StencilOp::IncrClamp:  return MTLStencilOperationIncrementClamp;
        case StencilOp::DecrClamp:  return MTLStencilOperationDecrementClamp;
        case StencilOp::Invert:     return MTLStencilOperationInvert;
        case StencilOp::IncrWrap:   return MTLStencilOperationIncrementWrap;
        case StencilOp::DecrWrap:   return MTLStencilOperationDecrementWrap;
        default:
            o_error("mtlTypes::asStencilOp(): invalid value!\n");
            return MTLStencilOperationKeep;
    }
}

//------------------------------------------------------------------------------
MTLColorWriteMask
mtlTypes::asColorWriteMask(PixelChannel::Mask mask) {
    MTLColorWriteMask mtlMask = MTLColorWriteMaskNone;
    if (mask & PixelChannel::Red) {
        mtlMask |= MTLColorWriteMaskRed;
    }
    if (mask & PixelChannel::Green) {
        mtlMask |= MTLColorWriteMaskGreen;
    }
    if (mask & PixelChannel::Blue) {
        mtlMask |= MTLColorWriteMaskBlue;
    }
    if (mask & PixelChannel::Alpha) {
        mtlMask |= MTLColorWriteMaskAlpha;
    }
    return mtlMask;
}

//------------------------------------------------------------------------------
MTLBlendOperation
mtlTypes::asBlendOp(BlendOperation::Code op) {
    switch (op) {
        case BlendOperation::Add:               return MTLBlendOperationAdd;
        case BlendOperation::Subtract:          return MTLBlendOperationSubtract;
        case BlendOperation::ReverseSubtract:   return MTLBlendOperationReverseSubtract;
        default:
            o_error("mtlTypes::asBlendOp(): invalid value!\n");
            return MTLBlendOperationAdd;
    }
}

//------------------------------------------------------------------------------
MTLBlendFactor
mtlTypes::asBlendFactor(BlendFactor::Code factor) {
    switch (factor) {
        case BlendFactor::Zero:                 return MTLBlendFactorZero;
        case BlendFactor::One:                  return MTLBlendFactorOne;
        case BlendFactor::SrcColor:             return MTLBlendFactorSourceColor;
        case BlendFactor::OneMinusSrcColor:     return MTLBlendFactorOneMinusSourceColor;
        case BlendFactor::SrcAlpha:             return MTLBlendFactorSourceAlpha;
        case BlendFactor::OneMinusSrcAlpha:     return MTLBlendFactorOneMinusSourceAlpha;
        case BlendFactor::DstColor:             return MTLBlendFactorDestinationColor;
        case BlendFactor::OneMinusDstColor:     return MTLBlendFactorOneMinusDestinationColor;
        case BlendFactor::DstAlpha:             return MTLBlendFactorDestinationAlpha;
        case BlendFactor::OneMinusDstAlpha:     return MTLBlendFactorOneMinusDestinationAlpha;
        case BlendFactor::SrcAlphaSaturated:    return MTLBlendFactorSourceAlphaSaturated;
        case BlendFactor::BlendColor:           return MTLBlendFactorBlendColor;
        case BlendFactor::OneMinusBlendColor:   return MTLBlendFactorOneMinusBlendColor;
        case BlendFactor::BlendAlpha:           return MTLBlendFactorBlendAlpha;
        case BlendFactor::OneMinusBlendAlpha:   return MTLBlendFactorOneMinusBlendAlpha;
        default:
            o_error("mtlTypes::asBlendFactor(): invalid value!\n");
            return MTLBlendFactorOne;
    }
}

//------------------------------------------------------------------------------
MTLVertexFormat
mtlTypes::asVertexFormat(VertexFormat::Code fmt) {
    switch (fmt) {
        case VertexFormat::Float:       return MTLVertexFormatFloat;
        case VertexFormat::Float2:      return MTLVertexFormatFloat2;
        case VertexFormat::Float3:      return MTLVertexFormatFloat3;
        case VertexFormat::Float4:      return MTLVertexFormatFloat4;
        case VertexFormat::Byte4:       return MTLVertexFormatChar4;
        case VertexFormat::Byte4N:      return MTLVertexFormatChar4Normalized;
        case VertexFormat::UByte4:      return MTLVertexFormatUChar4;
        case VertexFormat::UByte4N:     return MTLVertexFormatUChar4Normalized;
        case VertexFormat::Short2:      return MTLVertexFormatShort2;
        case VertexFormat::Short2N:     return MTLVertexFormatShort2Normalized;
        case VertexFormat::Short4:      return MTLVertexFormatShort4;
        case VertexFormat::Short4N:     return MTLVertexFormatShort4Normalized;
        case VertexFormat::UInt10_2N:   return MTLVertexFormatUInt1010102Normalized;
        default:
            o_error("mtlTypes::asVertexFormat(): invalid value!\n");
            return MTLVertexFormatFloat;
    }
}

//------------------------------------------------------------------------------
MTLVertexStepFunction
mtlTypes::asVertexStepFunc(VertexStepFunction::Code func) {
    switch (func) {
        case VertexStepFunction::PerVertex:     return MTLVertexStepFunctionPerVertex;
        case VertexStepFunction::PerInstance:   return MTLVertexStepFunctionPerInstance;
        default:
            o_error("mtlTypes::asVertexStepFunc(): invalid value!\n");
            return MTLVertexStepFunctionPerVertex;
        }
}

//------------------------------------------------------------------------------
MTLCullMode
mtlTypes::asCullMode(bool cullEnabled, Face::Code face) {
    if (cullEnabled) {
        return (Face::Front == face) ? MTLCullModeFront : MTLCullModeBack;
    }
    else {
        return MTLCullModeNone;
    }
}

//------------------------------------------------------------------------------
MTLIndexType
mtlTypes::asIndexType(IndexType::Code c) {
    switch (c) {
        case IndexType::Index16: return MTLIndexTypeUInt16;
        case IndexType::Index32: return MTLIndexTypeUInt32;
        default:
            o_error("mtlTypes::asIndexType(): invalid value!\n");
            return MTLIndexTypeUInt16;
    }
}

//------------------------------------------------------------------------------
MTLPrimitiveType
mtlTypes::asPrimitiveType(PrimitiveType::Code c) {
    switch (c) {
        case PrimitiveType::Points:         return MTLPrimitiveTypePoint;
        case PrimitiveType::Lines:          return MTLPrimitiveTypeLine;
        case PrimitiveType::LineStrip:      return MTLPrimitiveTypeLineStrip;
        case PrimitiveType::Triangles:      return MTLPrimitiveTypeTriangle;
        case PrimitiveType::TriangleStrip:  return MTLPrimitiveTypeTriangleStrip;
        default:
            o_error("mtlTypes::asPrimitiveType(): invalid value!\n");
            return MTLPrimitiveTypePoint;
    }
}

//------------------------------------------------------------------------------
MTLLoadAction
mtlTypes::asLoadAction(const PassAction* action, int colorIndex, bool depthStencil) {
    o_assert_dbg(action);
    o_assert_range_dbg(colorIndex, GfxConfig::MaxNumColorAttachments);
    if (depthStencil) {
        if (action->Flags & PassAction::ClearDS) {
            return MTLLoadActionClear;
        }
        else if (action->Flags & PassAction::LoadDS) {
            return MTLLoadActionLoad;
        }
        else {
            return MTLLoadActionDontCare;
        }
    }
    else {
        if (action->Flags & (PassAction::ClearC0<<colorIndex)) {
            return MTLLoadActionClear;
        }
        else if (action->Flags & (PassAction::LoadC0<<colorIndex)) {
            return MTLLoadActionLoad;
        }
        else {
            return MTLLoadActionDontCare;
        }
    }
}

} // namespace _priv
} // namespace Oryol
