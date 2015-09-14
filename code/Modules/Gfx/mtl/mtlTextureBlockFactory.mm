//------------------------------------------------------------------------------
//  mtlTextureBlockFactory.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTextureBlockFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Resource/resourcePools.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlTextureBlockFactory::mtlTextureBlockFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlTextureBlockFactory::~mtlTextureBlockFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlTextureBlockFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
mtlTextureBlockFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
mtlTextureBlockFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureBlockFactory::SetupResource(textureBlock& tb) {
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
            o_assert_dbg(nil != tex->mtlTex);
            o_assert_dbg(nil != tex->mtlSamplerState);

            // check that the texture type is what the shader expects
            const auto& texBlockComp = layout.ComponentAt(layout.ComponentIndexForBindSlot(bindSlot));
            if (texBlockComp.Type != tex->textureAttrs.Type) {
                o_error("Texture type mismatch at slot '%s'\n", texBlockComp.Name.AsCStr());
            }

            // copy over the metal texture objects
            auto& dstEntry = tb.entries[tb.numEntries++];
            o_assert_dbg(InvalidIndex == dstEntry.bindSlot);
            o_assert_dbg(nil == dstEntry.mtlTex);
            o_assert_dbg(nil == dstEntry.mtlSamplerState);
            dstEntry.bindSlot = bindSlot;
            dstEntry.mtlTex = tex->mtlTex;
            dstEntry.mtlSamplerState = tex->mtlSamplerState;
        }
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlTextureBlockFactory::DestroyResource(textureBlock& tb) {
    o_assert_dbg(this->isValid);
    tb.Clear();
}

} // namespace _priv
} // namespace Oryol
