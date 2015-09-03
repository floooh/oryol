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

    struct texAndTarget {
        GLuint glTex = 0;
        GLenum glTarget = 0;
    };
    /// vertex shader textures
    StaticArray<texAndTarget, GfxConfig::MaxNumVSTextures> vs;
    /// fragment shader textures
    StaticArray<texAndTarget, GfxConfig::MaxNumFSTextures> fs;
};

} // namespace _priv
} // namespace