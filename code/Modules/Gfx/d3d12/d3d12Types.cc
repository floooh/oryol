//------------------------------------------------------------------------------
//  d3d12Types.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Types.h"
#include "d3d12_impl.h"

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

} // namespace _priv
} // namespace Oryol