#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glTextureBlock
    @ingroup _priv
    @brief GL implementation of textureBlock
*/
#include "Gfx/Resource/textureBlockBase.h"
#include "Gfx/gl/gl_decl.h"
#include "Gfx/Core/GfxConfig.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class glTextureBlock : public textureBlockBase {
public:
    /// clear the object (called from textureBlockFactory::DestroyResource())
    void Clear();

    struct entry {
        int32 samplerIndex = InvalidIndex;
        GLuint glTex = 0;
        GLenum glTarget = 0;
    };
    int numEntries = 0;
    StaticArray<entry, GfxConfig::MaxNumTexturesPerStage> entries;
};

} // namespace _priv
} // namespace Oryol
