//------------------------------------------------------------------------------
//  d3d12TextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12TextureFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Resource/resource.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/resourcePools.h"
#include "d3d12_impl.h"
#include "d3d12Types.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12TextureFactory::~d3d12TextureFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12TextureFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d12TextureFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12TextureFactory::SetupResource(texture& tex, const void* data, int size) {
    o_assert_dbg(this->isValid);
    return this->createTexture(tex, data, size);
}

//------------------------------------------------------------------------------
void
d3d12TextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);
    d3d12ResAllocator& resAllocator = this->pointers.renderer->resAllocator;
    const uint64_t frameIndex = this->pointers.renderer->frameIndex;
    for (const auto& slot : tex.slots) {
        if (slot.d3d12TextureRes) {
            resAllocator.ReleaseDeferred(frameIndex, slot.d3d12TextureRes);
        }
        if (slot.d3d12UploadBuffer) {
            resAllocator.ReleaseDeferred(frameIndex, slot.d3d12UploadBuffer);
        }
    }
    if (tex.d3d12DepthStencilTextureRes) {
        resAllocator.ReleaseDeferred(frameIndex, tex.d3d12DepthStencilTextureRes);
    }
    tex.Clear();
}

//------------------------------------------------------------------------------
void
d3d12TextureFactory::setupTextureAttrs(texture& tex) {
    TextureAttrs attrs;
    attrs.Locator = tex.Setup.Locator;
    attrs.Type = tex.Setup.Type;
    attrs.ColorFormat = tex.Setup.ColorFormat;
    attrs.DepthFormat = tex.Setup.DepthFormat;
    attrs.SampleCount = tex.Setup.SampleCount;
    attrs.TextureUsage = tex.Setup.TextureUsage;
    attrs.Width = tex.Setup.Width;
    attrs.Height = tex.Setup.Height;
    attrs.Depth = tex.Setup.Depth;
    attrs.IsRenderTarget = tex.Setup.RenderTarget;
    attrs.HasDepthBuffer = tex.Setup.HasDepth();
    tex.textureAttrs = attrs;
}

//------------------------------------------------------------------------------
ResourceState::Code 
d3d12TextureFactory::createTexture(texture& tex, const void* data, int size) {
    o_assert_dbg(nullptr == tex.slots[0].d3d12TextureRes);

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.NumMipMaps > 0);
    o_assert_dbg(setup.TextureUsage == Usage::Immutable);
    o_assert_dbg(!setup.ShouldSetupFromNativeTexture());

    if ((setup.Type == TextureType::Texture3D) || (setup.Type == TextureType::TextureArray)) {
        o_warn("FIXME texture type!\n");
        return ResourceState::Failed;
    }
    if (DXGI_FORMAT_UNKNOWN == d3d12Types::asTextureFormat(setup.ColorFormat)) {
        o_warn("d3d12TextureFactory: unknown texture format!\n");
        return ResourceState::Failed;
    }
    d3d12ResAllocator& resAllocator = this->pointers.renderer->resAllocator;
    ID3D12Device* d3d12Device = this->pointers.renderer->d3d12Device;
    ID3D12GraphicsCommandList* cmdList = this->pointers.renderer->curCommandList();
    const uint64_t frameIndex = this->pointers.renderer->frameIndex;
    uint32_t copyFootprint = 0;
    if (Usage::Immutable != setup.TextureUsage) {
        copyFootprint = resAllocator.ComputeTextureCopyFootprint(d3d12Device, setup);
    }

    // create d3d12 texture resource(s)
    tex.numSlots = Usage::Stream == setup.TextureUsage ? 2 : 1;
    for (uint32_t slotIndex = 0; slotIndex < tex.numSlots; slotIndex++) {
        if (setup.RenderTarget) {
            tex.slots[slotIndex].d3d12TextureRes = resAllocator.AllocRenderTarget(d3d12Device, setup.Width, setup.Height, setup.ColorFormat, setup.SampleCount);
        }
        else {
            tex.slots[slotIndex].d3d12TextureRes = resAllocator.AllocTexture(d3d12Device, cmdList, frameIndex, setup, data, size);
        }
        o_assert_dbg(tex.slots[slotIndex].d3d12TextureRes);
        tex.slots[slotIndex].d3d12TextureState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        if (Usage::Immutable != setup.TextureUsage) {
            tex.slots[slotIndex].d3d12UploadBuffer = resAllocator.AllocUploadBuffer(d3d12Device, copyFootprint);
            o_assert_dbg(tex.slots[slotIndex].d3d12UploadBuffer);
        }
    }

    // optional depth-stencil texture resource
    if (setup.RenderTarget && (setup.DepthFormat != PixelFormat::InvalidPixelFormat)) {
        tex.d3d12DepthStencilTextureRes = resAllocator.AllocRenderTarget(d3d12Device, setup.Width, setup.Height, setup.DepthFormat, setup.SampleCount);
        o_assert_dbg(tex.d3d12DepthStencilTextureRes);
        tex.d3d12DepthStencilTextureState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }

    // setup texture attributes
    this->setupTextureAttrs(tex);

    return ResourceState::Valid;
}

} // namespace _priv
} // namespace Oryol
