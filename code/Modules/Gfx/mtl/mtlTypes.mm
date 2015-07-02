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

} // namespace _priv
} // namespace Oryol