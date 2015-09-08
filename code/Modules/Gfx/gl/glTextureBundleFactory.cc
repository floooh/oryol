//------------------------------------------------------------------------------
//  glTextureBundleFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTextureBundleFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Resource/textureBundle.h"
#include "Gfx/Resource/resourcePools.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glTextureBundleFactory::glTextureBundleFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glTextureBundleFactory::~glTextureBundleFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
glTextureBundleFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
glTextureBundleFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
glTextureBundleFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glTextureBundleFactory::SetupResource(textureBundle& tb) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->pointers.texturePool);

    // This method will only be called by the gfxResourceContainer object
    // once all textures in the bundle have finished loading (or are already valid),
    // the textureBundle object will be in Pending state until this has happened, if any
    // of the textures are in failed state, the textureBundle object
    // will also be set to failed state. All that remains to do here is
    // to lookup the texture objects and gather the GL texture names from them.

    // get shader and progIndex in order to lookup the samplerIndex on the shader
    const shader* shd = this->pointers.shaderPool->Lookup(tb.Setup.Shader);
    o_assert_dbg(shd);
    int32 progIndex = shd->getProgIndexByMask(tb.Setup.ShaderSelectionMask);
    o_assert_dbg(InvalidIndex != progIndex);

    for (int i = 0; i < GfxConfig::MaxNumVSTextures; i++) {
        o_assert_dbg((0 == tb.vs[i].glTex) && (0 == tb.vs[i].glTarget));
        const auto& vsTex = tb.Setup.VS[i];
        if (vsTex.IsValid()) {
            o_assert_dbg(vsTex.Type == GfxResourceType::Texture);
            texture* tex = this->pointers.texturePool->Get(vsTex);
            o_assert_dbg(tex && (ResourceState::Valid == tex->State));
            o_assert_dbg((tex->glTex != 0) && (tex->glTarget != 0));
            if (tex->textureAttrs.Type != shd->getSamplerType(progIndex, ShaderStage::VS, i)) {
                int32 texIndex = shd->Setup.TextureIndexByStageAndSlot(ShaderStage::VS, i);
                o_assert_dbg(InvalidIndex != texIndex);
                o_error("Texture type mismatch at slot '%s'!\n", shd->Setup.TextureName(texIndex).AsCStr());
            }
            auto& entry = tb.vs[i];
            entry.samplerIndex = shd->getSamplerIndex(progIndex, ShaderStage::VS, i);
            entry.glTex = tex->glTex;
            entry.glTarget = tex->glTarget;
            o_assert_dbg((0 != entry.glTex) && (0 != entry.glTarget) && (InvalidIndex != entry.samplerIndex));
        }
    }

    for (int i = 0; i < GfxConfig::MaxNumFSTextures; i++) {
        o_assert_dbg((0 == tb.fs[i].glTex) && (0 == tb.fs[i].glTarget));
        const auto& fsTex = tb.Setup.FS[i];
        if (fsTex.IsValid()) {
            o_assert_dbg(fsTex.Type == GfxResourceType::Texture);
            texture* tex = this->pointers.texturePool->Get(fsTex);
            o_assert_dbg(tex && (ResourceState::Valid == tex->State));
            o_assert_dbg((tex->glTex != 0) && (tex->glTarget != 0));
            if (tex->textureAttrs.Type != shd->getSamplerType(progIndex, ShaderStage::FS, i)) {
                int32 texIndex = shd->Setup.TextureIndexByStageAndSlot(ShaderStage::FS, i);
                o_assert_dbg(InvalidIndex != texIndex);
                o_error("Texture type mismatch at slot '%s'!\n", shd->Setup.TextureName(texIndex).AsCStr());
            }
            auto& entry = tb.fs[i];
            entry.samplerIndex = shd->getSamplerIndex(progIndex, ShaderStage::FS, i);
            entry.glTex = tex->glTex;
            entry.glTarget = tex->glTarget;
            o_assert_dbg((0 != entry.glTex) && (0 != entry.glTarget) && (InvalidIndex != entry.samplerIndex));
        }
    }

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glTextureBundleFactory::DestroyResource(textureBundle& tb) {
    o_assert_dbg(this->isValid);
    tb.Clear();
}

} // namespace _priv
} // namespace Oryol
