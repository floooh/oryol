//------------------------------------------------------------------------------
//  glTextureBlockFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTextureBlockFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Resource/textureBlock.h"
#include "Gfx/Resource/resourcePools.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glTextureBlockFactory::glTextureBlockFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glTextureBlockFactory::~glTextureBlockFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
glTextureBlockFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
glTextureBlockFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
glTextureBlockFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glTextureBlockFactory::SetupResource(textureBlock& tb) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->pointers.texturePool);
    o_assert_dbg(this->pointers.shaderPool);

    // This method will only be called by the gfxResourceContainer object
    // once all textures in the block have finished loading (or are already valid),
    // the textureBlock object will be in Pending state until this has happened, if any
    // of the textures are in failed state, the textureBlock object
    // will also be set to failed state. All that remains to do here is
    // to lookup the texture objects and gather the GL texture names from them.

    // get shader and progIndex in order to lookup the samplerIndex on the shader
    const shader* shd = this->pointers.shaderPool->Lookup(tb.Setup.Shader);
    o_assert_dbg(shd);
    int32 progIndex = shd->getProgIndexByMask(tb.Setup.ShaderSelectionMask);
    o_assert_dbg(InvalidIndex != progIndex);
    int32 texBlockIndex = shd->Setup.TextureBlockIndexByStageAndSlot(tb.Setup.BindStage, tb.Setup.BindSlot);
    o_assert_dbg(InvalidIndex != texBlockIndex);
    const TextureBlockLayout& layout = shd->Setup.TextureBlockLayout(progIndex);

    for (int texIndex = 0; texIndex < GfxConfig::MaxNumTexturesPerStage; texIndex++) {
        const Id& texId = tb.Setup.Slot[texIndex];
        if (texId.IsValid()) {
            o_assert(texId.Type == GfxResourceType::Texture);

            // lookup source texture
            const texture* tex = this->pointers.texturePool->Get(texId);
            o_assert_dbg(tex && (ResourceState::Valid == tex->State));
            o_assert_dbg(0 != tex->glTex);
            o_assert_dbg(0 != tex->glTarget);

            // make sure the texture types matches the expected type in shader
            const auto& texBlockComp = layout.ComponentAt(layout.ComponentIndexForBindSlot(texIndex));
            if (texBlockComp.Type != tex->textureAttrs.Type) {
                o_error("Texture type mismatch at slot '%s'\n", texBlockComp.Name.AsCStr());
            }

            // copy over the GL texture ids
            auto& dstEntry = tb.entries[tb.numEntries++];
            o_assert_dbg(InvalidIndex == dstEntry.samplerIndex);
            o_assert_dbg(0 == dstEntry.glTex);
            o_assert_dbg(0 == dstEntry.glTarget);
            dstEntry.samplerIndex = shd->getSamplerIndex(progIndex, tb.Setup.BindStage, tb.Setup.BindSlot, texIndex);
            dstEntry.glTex = tex->glTex;
            dstEntry.glTarget = tex->glTarget;
        }
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glTextureBlockFactory::DestroyResource(textureBlock& tb) {
    o_assert_dbg(this->isValid);
    tb.Clear();
}

} // namespace _priv
} // namespace Oryol
