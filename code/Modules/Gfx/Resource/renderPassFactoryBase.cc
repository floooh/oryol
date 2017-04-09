//------------------------------------------------------------------------------
//  renderPassFactoryBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "renderPassFactoryBase.h"
#include "Core/Assertion.h"
#include "Gfx/Resource/resource.h"
#include "Gfx/Resource/resourcePools.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
renderPassFactoryBase::~renderPassFactoryBase() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
renderPassFactoryBase::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->pointers = ptrs;
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
renderPassFactoryBase::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
}

//------------------------------------------------------------------------------
ResourceState::Code
renderPassFactoryBase::SetupResource(renderPass& rp) {
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
renderPassFactoryBase::DestroyResource(renderPass& rp) {
    rp.Clear();
}

} // namespace _priv
} // namespace Oryol
