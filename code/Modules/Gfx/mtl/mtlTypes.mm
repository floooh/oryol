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

} // namespace _priv
} // namespace Oryol