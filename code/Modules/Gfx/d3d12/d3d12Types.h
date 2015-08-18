#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Types
    @ingroup _priv
    @brief D3D12 enum conversion helpers
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/d3d12/d3d12_decl.h"

namespace Oryol {
namespace _priv {

class d3d12Types {
public:
    /// convert PixelFormat to a display DXGI format
    static DXGI_FORMAT asSwapChainFormat(PixelFormat::Code pixelFormat);
};

} // namespace _priv
} // namespace Oryol