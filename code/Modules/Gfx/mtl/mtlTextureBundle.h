#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTextureBundle
    @ingroup _priv
    @brief Metal implementation of textureBundle
*/
#include "Gfx/Resource/textureBundleBase.h"
#include "Gfx/mtl/mtl_decl.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class mtlTextureBundle : public textureBundleBase {
public:
    /// clear the object
    void Clear();

    struct entry {
        entry();
        ORYOL_OBJC_TYPED_ID(MTLTexture) mtlTex;
        ORYOL_OBJC_TYPED_ID(MTLSamplerState) mtlSamplerState;
    };
    /// vertex shader textures
    StaticArray<entry, GfxConfig::MaxNumVSTextures> vs;
    /// fragment shader textures
    StaticArray<entry, GfxConfig::MaxNumFSTextures> fs;
};

} // namespace _priv
} // namespace Oryol
