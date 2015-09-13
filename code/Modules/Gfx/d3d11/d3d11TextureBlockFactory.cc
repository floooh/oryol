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

    const shader* shd = this->pointers.shaderPool->Lookup(tb.Setup.Shader);
    o_assert_dbg(shd);
    int32 texBlockIndex = shd->Setup.TextureBlockIndexByStageAndSlot(tb.Setup.BindStage, tb.Setup.BindSlot);
    o_assert_dbg(InvalidIndex != texBlockIndex);
    const TextureBlockLayout& layout = shd->Setup.TextureBlockLayout(texBlockIndex);

    tb.bindStage = tb.Setup.BindStage;
    for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumTexturesPerStage; bindSlot++) {
        Id texId = tb.Setup.Slot[bindSlot];
        if (texId.IsValid()) {
            o_assert(texId.Type == GfxResourceType::Texture);

            // lookup source texture
            const texture* tex = this->pointers.texturePool->Get(texId);
            o_assert_dbg(tex && (ResourceState::Valid == tex->State));
            o_assert_dbg(tex->d3d11ShaderResourceView && tex->d3d11SamplerState);

            // make sure type of source texture (2D, Cube...) is matching the
            // expected binding slot texture type
            const auto& texBlockComp = layout.ComponentAt(layout.ComponentIndexForBindSlot(bindSlot));
            if (texBlockComp.Type != tex->textureAttrs.Type) {
                o_error("Texture type mismatch at slot '%s'\n", texBlockComp.Name.AsCStr());
            }

            // copy over the D3D11 resource pointers
            auto& dstEntry = tb.entries[tb.numEntries++];
            o_assert_dbg(InvalidIndex == dstEntry.bindSlot);
            o_assert_dbg(nullptr == dstEntry.d3d11ShaderResourceView);
            o_assert_dbg(nullptr == dstEntry.d3d11SamplerState);
            dstEntry.bindSlot = bindSlot;
            dstEntry.d3d11ShaderResourceView = tex->d3d11ShaderResourceView;
            dstEntry.d3d11SamplerState = tex->d3d11SamplerState;
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
