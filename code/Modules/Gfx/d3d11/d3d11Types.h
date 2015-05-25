#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Types
    @ingroup _priv
    @brief D3D11 enum conversion helpers
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11Types {
public:
    /// convert PixelFormat to a display DXGI format
    static DXGI_FORMAT asSwapChainFormat(PixelFormat::Code pixelFormat);
    /// convert PixelFormat to a D3D11 texture format
    static DXGI_FORMAT asTextureFormat(PixelFormat::Code pixelFormat);
    /// convert Usage to a d3d11 usage for buffers
    static D3D11_USAGE asBufferUsage(Usage::Code usage);
    /// convert Usage to a d3d11 buffer CPU access flags mask
    static uint32 asBufferCPUAccessFlag(Usage::Code usage);
};

} // namespace _priv 
} // namespace Oryol