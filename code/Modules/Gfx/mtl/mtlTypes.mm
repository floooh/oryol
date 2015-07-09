//------------------------------------------------------------------------------
//  mtlTypes.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx/mtl/cocoa.h"
#include "mtlTypes.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
MTLPixelFormat
mtlTypes::asRenderTargetFormat(PixelFormat::Code fmt) {
    // see the capability table for the list of renderable pixel formats:
    // https://developer.apple.com/library/prerelease/ios/documentation/Metal/Reference/MetalConstants_Ref/index.html
    //
    switch (fmt) {
        case PixelFormat::RGBA8:
        case PixelFormat::RGB8:
            return MTLPixelFormatRGBA8Unorm;
        case PixelFormat::D32:
        case PixelFormat::D16:
            return MTLPixelFormatDepth32Float;
        case PixelFormat::D24S8:
            return MTLPixelFormatDepth32Float_Stencil8;
        default:
            return MTLPixelFormatInvalid;
    }
}

//------------------------------------------------------------------------------
MTLResourceOptions
mtlTypes::asBufferResourceOptions(Usage::Code usage) {
    switch (usage) {
        case Usage::Immutable:  return MTLResourceStorageModeShared;
        // FIXME: are these the right dynamic access flags?
        case Usage::Static:     return MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeManaged;
        case Usage::Dynamic:    return MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeManaged;
        case Usage::Stream:     return MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeManaged;
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

} // namespace _priv
} // namespace Oryol