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
    /// convert vertex attribute to d3d11 semantic name
    static const char* asSemanticName(VertexAttr::Code attr);
    /// convert vertex attribute to d3d11 semantic index
    static uint32 asSemanticIndex(VertexAttr::Code attr);
    /// convert vertex component format to d3d11 input element DXGI_FORMAT
    static DXGI_FORMAT asInputElementFormat(VertexFormat::Code fmt);
};

} // namespace _priv 
} // namespace Oryol