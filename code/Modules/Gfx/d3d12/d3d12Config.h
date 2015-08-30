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
    /// max number of vertex shader constant buffers
    static const int MaxNumVSConstantBuffers = GfxConfig::MaxNumUniformBlocks;
    /// max number of pixel shader constant buffers
    static const int MaxNumPSConstantBuffers = GfxConfig::MaxNumUniformBlocks;
    /// max number of vertex shader textures
    static const int MaxNumVSTextures = 2;
    /// max number of pixel shader textures
    static const int MaxNumPSTextures = 8;
    /// maximum number of different samplers over whole application
    static const int MaxNumSamplers = 64;
};

} // namespace _priv
} // namespace Oryol