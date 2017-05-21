//------------------------------------------------------------------------------
//  gfxFactoryBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "gfxFactoryBase.h"
#include "Gfx/private/resource.h"
#include "Gfx/private/resourcePools.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
gfxFactoryBase::~gfxFactoryBase() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
gfxFactoryBase::setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->pointers = ptrs;
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
gfxFactoryBase::discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
}

//------------------------------------------------------------------------------
void
gfxFactoryBase::garbageCollect() {
    // this is only implemented in some subclasses, depending on 
    // rendering backend
}

//------------------------------------------------------------------------------
ResourceState::Code
gfxFactoryBase::initPipeline(pipeline& pip) {
    o_assert_dbg(this->isValid);
    pip.shd = this->pointers.shaderPool->Lookup(pip.Setup.Shader);
    o_assert_dbg(pip.shd && (ResourceState::Valid == pip.shd->State));
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
gfxFactoryBase::destroyPipeline(pipeline& pip) {
    o_assert_dbg(this->isValid);
    pip.Clear();
}

//------------------------------------------------------------------------------
ResourceState::Code
gfxFactoryBase::initRenderPass(renderPass& rp) {
    o_assert_dbg(this->isValid);
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        o_assert_dbg(nullptr == rp.colorTextures[i]);
        Id id = rp.Setup.ColorAttachments[i].Texture;
        if (id.IsValid()) {
            rp.colorTextures[i] = this->pointers.texturePool->Lookup(id);
            o_assert_dbg(rp.colorTextures[i] && (ResourceState::Valid == rp.colorTextures[i]->State));
        }
    }
    o_assert_dbg(nullptr == rp.depthStencilTexture);
    Id id = rp.Setup.DepthStencilTexture;
    if (id.IsValid()) {
        rp.depthStencilTexture = this->pointers.texturePool->Lookup(id);
        o_assert_dbg(rp.depthStencilTexture && (ResourceState::Valid == rp.depthStencilTexture->State));
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
gfxFactoryBase::destroyRenderPass(renderPass& rp) {
    o_assert_dbg(this->isValid);
    rp.Clear();
}

} // namespace _priv
} // namespace Oryol

