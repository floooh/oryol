#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Config
    @ingroup _priv
    @brief D3D12 configuration values
*/
#include "Core/Types.h"
#include "Gfx/Core/GfxConfig.h"

namespace Oryol {
namespace _priv {

class d3d12Config {
public:
    /// the number of frames that can be in-flight
    static const int NumFrames = 2;
    /// maximum number of different samplers over whole application
    static const int MaxNumSamplers = 64;
    /// maximum number of render targets that can exist at the same time
    static const int MaxNumRenderTargets = 64;
};

} // namespace _priv
} // namespace Oryol