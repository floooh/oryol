#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTextureBlock
    @ingroup _priv
    @brief Metal implementation of textureBlock
*/
#include "Gfx/Resource/textureBlockBase.h"
#include "Gfx/mtl/mtl_decl.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class mtlTextureBlock : public textureBlockBase {
public:
    /// clear the object
    void Clear();

    ShaderStage::Code bindStage = ShaderStage::InvalidShaderStage;
    struct entry {
        entry();
        int32 bindSlot;
        ORYOL_OBJC_TYPED_ID(MTLTexture) mtlTex;
        ORYOL_OBJC_TYPED_ID(MTLSamplerState) mtlSamplerState;
    };
    int32 numEntries = 0;
    StaticArray<entry, GfxConfig::MaxNumTexturesPerStage> entries;
};

} // namespace _priv
} // namespace Oryol
