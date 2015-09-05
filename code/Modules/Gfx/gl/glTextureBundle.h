#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glTextureBundle
    @ingroup _priv
    @brief GL implementation of textureBundle
*/
#include "Gfx/Resource/textureBundleBase.h"
#include "Gfx/gl/gl_decl.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class glTextureBundle : public textureBundleBase {
public:
    /// clear the object (called from textureBundleFactory::DestroyResource())
    void Clear();

    struct bindEntry {
        int32 samplerIndex = InvalidIndex;
        GLuint glTex = 0;
        GLenum glTarget = 0;
    };
    /// vertex shader textures
    StaticArray<bindEntry, GfxConfig::MaxNumVSTextures> vs;
    /// fragment shader textures
    StaticArray<bindEntry, GfxConfig::MaxNumFSTextures> fs;
};

} // namespace _priv
} // namespace