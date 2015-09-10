//------------------------------------------------------------------------------
//  d3d11TextureBlockFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11TextureBlockFactory.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Resource/resourcePools.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11TextureBlockFactory::d3d11TextureBlockFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d11TextureBlockFactory::~d3d11TextureBlockFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d11TextureBlockFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d11TextureBlockFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
d3d11TextureBlockFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11TextureBlockFactory::SetupResource(textureBlock& tb) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->pointers.texturePool);
    o_assert_dbg(this->pointers.shaderPool);

#error "FIXME!"

    const shader* shd = this->pointers.shaderPool->Lookup(tb.Setup.Shader);
    o_assert_dbg(shd);

    // gather vertex shader textures
    for (int i = 0; i < GfxConfig::MaxNumVSTextures; i++) {
        o_assert_dbg(nullptr == tb.vs[i].d3d11ShaderResourceView);
        o_assert_dbg(nullptr == tb.vs[i].d3d11SamplerState);
        const auto& vsTex = tb.Setup.VS[i];
        if (vsTex.IsValid()) {
            o_assert_dbg(vsTex.Type == GfxResourceType::Texture);
            const texture* tex = this->pointers.texturePool->Get(vsTex);
            o_assert_dbg(tex && (ResourceState::Valid == tex->State));
            o_assert_dbg(tex->d3d11ShaderResourceView && tex->d3d11SamplerState);
            const int32 texIndex = shd->Setup.TextureIndexByStageAndSlot(ShaderStage::VS, i);
            o_assert_dbg(InvalidIndex != texIndex);
            if (tex->textureAttrs.Type != shd->Setup.TextureType(texIndex)) {
                o_error("Texture type mismatch at slot '%s'\n", shd->Setup.TextureName(texIndex).AsCStr());
            }
            auto& entry = tb.vs[i];
            entry.d3d11ShaderResourceView = tex->d3d11ShaderResourceView;
            entry.d3d11SamplerState = tex->d3d11SamplerState;
        }
    }

    // gather fragment shader textures
    for (int i = 0; i < GfxConfig::MaxNumFSTextures; i++) {
        o_assert_dbg(nullptr == tb.fs[i].d3d11ShaderResourceView);
        o_assert_dbg(nullptr == tb.fs[i].d3d11SamplerState);
        const auto& fsTex = tb.Setup.FS[i];
        if (fsTex.IsValid()) {
            o_assert_dbg(fsTex.Type == GfxResourceType::Texture);
            const texture* tex = this->pointers.texturePool->Get(fsTex);
            o_assert_dbg(tex && (ResourceState::Valid == tex->State));
            o_assert_dbg(tex->d3d11ShaderResourceView && tex->d3d11SamplerState);
            const int32 texIndex = shd->Setup.TextureIndexByStageAndSlot(ShaderStage::FS, i);
            o_assert_dbg(InvalidIndex != texIndex);
            if (tex->textureAttrs.Type != shd->Setup.TextureType(texIndex)) {
                o_error("Texture type mismatch at slot '%s'\n", shd->Setup.TextureName(texIndex).AsCStr());
            }
            auto& entry = tb.fs[i];
            entry.d3d11ShaderResourceView = tex->d3d11ShaderResourceView;
            entry.d3d11SamplerState = tex->d3d11SamplerState;
        }
    }

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11TextureBlockFactory::DestroyResource(textureBlock& tb) {
    o_assert_dbg(this->isValid);
    tb.Clear();
}

} // namespace _priv
} // namespace Oryol
