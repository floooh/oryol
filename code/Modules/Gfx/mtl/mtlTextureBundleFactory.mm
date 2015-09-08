//------------------------------------------------------------------------------
//  mtlTextureBundleFactory.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTextureBundleFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Resource/resourcePools.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlTextureBundleFactory::mtlTextureBundleFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlTextureBundleFactory::~mtlTextureBundleFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlTextureBundleFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
mtlTextureBundleFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
mtlTextureBundleFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureBundleFactory::SetupResource(textureBundle& tb) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->pointers.texturePool);

    const shader* shd = this->pointers.shaderPool->Lookup(tb.Setup.Shader);
    o_assert_dbg(shd);

    for (int i = 0; i < GfxConfig::MaxNumVSTextures; i++) {
        o_assert_dbg((nil == tb.vs[i].mtlTex) && (nil == tb.vs[i].mtlSamplerState));
        const auto& vsTex = tb.Setup.VS[i];
        if (vsTex.IsValid()) {
            o_assert_dbg(vsTex.Type == GfxResourceType::Texture);
            const texture* tex = this->pointers.texturePool->Get(vsTex);
            o_assert_dbg(tex && (ResourceState::Valid == tex->State));
            o_assert_dbg((nil != tex->mtlTex) && (nil != tex->mtlSamplerState));
            const int32 texIndex = shd->Setup.TextureIndexByStageAndSlot(ShaderStage::VS, i);
            o_assert_dbg(InvalidIndex != texIndex);
            if (tex->textureAttrs.Type != shd->Setup.TextureType(texIndex)) {
                o_error("Texture type mismatch at slot '%s'\n", shd->Setup.TextureName(texIndex).AsCStr());
            }
            auto& entry = tb.vs[i];
            entry.mtlTex = tex->mtlTex;
            entry.mtlSamplerState = tex->mtlSamplerState;
        }
    }

    for (int i = 0; i < GfxConfig::MaxNumFSTextures; i++) {
        o_assert_dbg((nil == tb.fs[i].mtlTex) && (nil == tb.fs[i].mtlSamplerState));
        const auto& fsTex = tb.Setup.FS[i];
        if (fsTex.IsValid()) {
            o_assert_dbg(fsTex.Type == GfxResourceType::Texture);
            const texture* tex = this->pointers.texturePool->Get(fsTex);
            o_assert_dbg(tex && (ResourceState::Valid == tex->State));
            o_assert_dbg((nil != tex->mtlTex) && (nil != tex->mtlSamplerState));
            const int32 texIndex = shd->Setup.TextureIndexByStageAndSlot(ShaderStage::FS, i);
            o_assert_dbg(InvalidIndex != texIndex);
            if (tex->textureAttrs.Type != shd->Setup.TextureType(texIndex)) {
                o_error("Texture type mismatch at slot '%s'\n", shd->Setup.TextureName(texIndex).AsCStr());
            }
            auto& entry = tb.fs[i];
            entry.mtlTex = tex->mtlTex;
            entry.mtlSamplerState = tex->mtlSamplerState;
        }
    }

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlTextureBundleFactory::DestroyResource(textureBundle& tb) {
    o_assert_dbg(this->isValid);
    tb.Clear();
}

} // namespace _priv
} // namespace Oryol
