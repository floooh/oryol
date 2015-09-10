#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11TextureBundle
    @ingroup _priv
    @brief D3D11 implementation of textureBundle
*/
#include "Gfx/resource/textureBundleBase.h"
#include "Gfx/d3d11/d3d11_decl.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class d3d11TextureBundle : public textureBundleBase {
public:
    /// clear the object
    void Clear();

    struct entry {
        ID3D11ShaderResourceView* d3d11ShaderResourceView = nullptr;
        ID3D11SamplerState* d3d11SamplerState = nullptr;
    };
    /// vertex shader textures
    StaticArray<entry, GfxConfig::MaxNumVSTextures> vs;
    /// fragment shader textures
    StaticArray<entry, GfxConfig::MaxNumFSTextures> fs;
};

} // namespace _priv
} // namespace Oryol