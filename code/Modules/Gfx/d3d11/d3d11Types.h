#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Types
    @ingroup _priv
    @brief D3D11 enum conversion helpers
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

class d3d11Types {
public:
    /// convert PixelFormat to a display DXGI format
    static DXGI_FORMAT asSwapChainFormat(PixelFormat::Code pixelFormat);
    /// convert PixelFormat to a D3D11 texture format
    static DXGI_FORMAT asTextureFormat(PixelFormat::Code pixelFormat);
};

} // namespace _priv 
} // namespace Oryol