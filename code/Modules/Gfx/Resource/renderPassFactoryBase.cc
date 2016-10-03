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
renderPassFactoryBase::renderPassFactoryBase() :
isValid(false) {
    // empty
}

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
    Id id = rp.Setup.DepthStencilAttachment.Texture;
    if (id.IsValid()) {
        rp.depthStencilTexture = this->pointers.texturePool->Lookup(id);
        o_assert_dbg(rp.depthStencilTexture && (ResourceState::Valid == rp.depthStencilTexture->State));
    }
    this->validateRenderPass(rp);
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
renderPassFactoryBase::DestroyResource(renderPass& rp) {
    rp.Clear();
}

//------------------------------------------------------------------------------
void
renderPassFactoryBase::validateRenderPass(const renderPass& rp) {

    // check that at least one color attachment texture is defined
    // and that there are no 'holes' if there are multiple attachments
    bool continuous = true;
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        if (rp.colorTextures[i]) {
            if (!continuous) {
                o_error("invalid render pass: must have continuous color attachments!\n");
            }
        }
        else {
            if (0 == i) {
                o_error("invalid render pass: must have color attachment at slot 0!\n");
            }
            continuous = false;
        }
    }

    // check that all render targets have the required params
    const int w = rp.colorTextures[0]->textureAttrs.Width;
    const int h = rp.colorTextures[0]->textureAttrs.Height;
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        const texture* tex = rp.colorTextures[i];
        if (tex) {
            const auto& attrs = tex->textureAttrs;
            if ((attrs.Width != w) || (attrs.Height != h)) {
                o_error("invalid render pass: all color attachments must have the same size!\n");
            }
            if (attrs.TextureUsage != Usage::Immutable) {
                o_error("invalid render pass: color attachments must have immutable usage!\n");
            }
            if (!tex->Setup.ShouldSetupAsRenderTarget()) {
                o_error("invalid render pass: color attachment must have been setup as render target!\n");
            }
        }
    }
    const texture* dsTex = rp.depthStencilTexture;
    if (dsTex) {
        const auto& attrs = dsTex->textureAttrs;
        if ((attrs.Width != w) || (attrs.Height != h)) {
            o_error("invalid render pass: depth-stencil attachment must have same size as color attachments!\n");
        }
        if (attrs.TextureUsage != Usage::Immutable) {
            o_error("invalid render pass: depth attachment must have immutable usage!\n");
        }
        if (!dsTex->Setup.ShouldSetupAsRenderTarget()) {
            o_error("invalid render pass: depth attachment must have been setup as render target!\n");
        }
    }
}

} // namespace _priv
} // namespace Oryol
