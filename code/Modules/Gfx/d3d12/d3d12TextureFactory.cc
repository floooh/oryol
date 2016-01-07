//------------------------------------------------------------------------------
//  d3d12TextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12TextureFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/resourcePools.h"
#include "d3d12_impl.h"
#include "d3d12Types.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12TextureFactory::d3d12TextureFactory() :
isValid(false) {
    // empty
}

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
bool
d3d12TextureFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12TextureFactory::SetupResource(texture& tex) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.ShouldSetupFromPixelData());
    o_assert_dbg(!tex.Setup.ShouldSetupFromFile());

    if (tex.Setup.ShouldSetupAsRenderTarget()) {
        return this->createRenderTarget(tex);
    }
    else if (tex.Setup.ShouldSetupEmpty()) {
        return this->createEmptyTexture(tex);
    }
    else {
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12TextureFactory::SetupResource(texture& tex, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.ShouldSetupAsRenderTarget());

    if (tex.Setup.ShouldSetupFromPixelData()) {
        return this->createFromPixelData(tex, data, size);
    }
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
d3d12TextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);
    d3d12ResAllocator& resAllocator = this->pointers.renderer->resAllocator;
    d3d12DescAllocator& descAllocator = this->pointers.renderer->descAllocator;
    const uint64 frameIndex = this->pointers.renderer->frameIndex;

    for (const auto& slot : tex.slots) {
        if (slot.d3d12TextureRes) {
            resAllocator.ReleaseDeferred(frameIndex, slot.d3d12TextureRes);
        }
    }
    if (tex.d3d12DepthBufferRes) {
        resAllocator.ReleaseDeferred(frameIndex, tex.d3d12DepthBufferRes);
    }
    if (InvalidIndex != tex.rtvDescriptorSlot) {
        const Id& rtvHeap = this->pointers.renderer->rtvHeap;
        descAllocator.ReleaseSlotDeferred(rtvHeap, frameIndex, tex.rtvDescriptorSlot);
    }
    if (InvalidIndex != tex.dsvDescriptorSlot) {
        const Id& dsvHeap = this->pointers.renderer->dsvHeap;
        descAllocator.ReleaseSlotDeferred(dsvHeap, frameIndex, tex.dsvDescriptorSlot);
    }
    tex.Clear();
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12TextureFactory::createRenderTarget(texture& tex) {
    o_assert_dbg(nullptr == tex.slots[0].d3d12TextureRes);
    o_assert_dbg(1 == tex.numSlots);
    o_assert_dbg(nullptr == tex.d3d12DepthBufferRes);
    o_assert_dbg(InvalidIndex == tex.rtvDescriptorSlot);
    o_assert_dbg(InvalidIndex == tex.dsvDescriptorSlot);

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.ShouldSetupAsRenderTarget());
    o_assert_dbg(setup.TextureUsage == Usage::Immutable);
    o_assert_dbg(setup.NumMipMaps == 1);
    o_assert_dbg(setup.Type == TextureType::Texture2D);
    o_assert_dbg(PixelFormat::IsValidRenderTargetColorFormat(setup.ColorFormat));

    d3d12ResAllocator& resAllocator = this->pointers.renderer->resAllocator;
    d3d12DescAllocator& descAllocator = this->pointers.renderer->descAllocator;
    ID3D12Device* d3d12Device = this->pointers.renderer->d3d12Device;

    // get size of new render target
    int32 width, height;
    texture* sharedDepthProvider = nullptr;
    if (setup.IsRelSizeRenderTarget()) {
        const DisplayAttrs& dispAttrs = this->pointers.displayMgr->GetDisplayAttrs();
        width = int32(dispAttrs.FramebufferWidth * setup.RelWidth);
        height = int32(dispAttrs.FramebufferHeight * setup.RelHeight);
    }
    else if (setup.HasSharedDepth()) {
        sharedDepthProvider = this->pointers.texturePool->Lookup(setup.DepthRenderTarget);
        o_assert_dbg(nullptr != sharedDepthProvider);
        width = sharedDepthProvider->textureAttrs.Width;
        height = sharedDepthProvider->textureAttrs.Height;
    }
    else {
        width = setup.Width;
        height = setup.Height;
    }
    o_assert_dbg((width > 0) && (height > 0));

    // create the color buffer and render-target-view
    tex.slots[0].d3d12TextureRes = resAllocator.AllocRenderTarget(d3d12Device, width, height, setup.ColorFormat, setup.ClearHint, 1);
    tex.slots[0].d3d12TextureState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    const Id& rtvHeap = this->pointers.renderer->rtvHeap;
    tex.rtvDescriptorSlot = descAllocator.AllocSlot(rtvHeap);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle;
    descAllocator.CPUHandle(rtvCPUHandle, rtvHeap, tex.rtvDescriptorSlot);
    d3d12Device->CreateRenderTargetView(tex.slots[0].d3d12TextureRes, nullptr, rtvCPUHandle);

    // create optional depth-buffer
    if (setup.HasDepth()) {
        if (setup.HasSharedDepth()) {
            o_assert_dbg(sharedDepthProvider->d3d12DepthBufferRes);
            tex.d3d12DepthBufferRes = sharedDepthProvider->d3d12DepthBufferRes;
            tex.d3d12DepthBufferRes->AddRef();
        }
        else {
            tex.d3d12DepthBufferRes = resAllocator.AllocRenderTarget(d3d12Device, width, height, setup.DepthFormat, setup.ClearHint, 1);
        }
        tex.d3d12DepthBufferState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        const Id& dsvHeap = this->pointers.renderer->dsvHeap;
        tex.dsvDescriptorSlot = descAllocator.AllocSlot(dsvHeap);
        D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUHandle;
        descAllocator.CPUHandle(dsvCPUHandle, dsvHeap, tex.dsvDescriptorSlot);
        d3d12Device->CreateDepthStencilView(tex.d3d12DepthBufferRes, nullptr, dsvCPUHandle);
    }

    // setup texture attrs and set on texture
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = TextureType::Texture2D;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.DepthFormat = setup.DepthFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = width;
    attrs.Height = height;
    attrs.NumMipMaps = 1;
    attrs.IsRenderTarget = true;
    attrs.HasDepthBuffer = setup.HasDepth();
    attrs.HasSharedDepthBuffer = setup.HasSharedDepth();
    tex.textureAttrs = attrs;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code 
d3d12TextureFactory::createFromPixelData(texture& tex, const void* data, int32 size) {
    o_assert_dbg(nullptr == tex.slots[0].d3d12TextureRes);
    o_assert_dbg(1 == tex.numSlots);
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.NumMipMaps > 0);
    o_assert_dbg(setup.TextureUsage == Usage::Immutable);

    if (setup.Type == TextureType::Texture3D) {
        o_warn("d3d12TextureFactory: 3d textures not yet implemented!\n");
        return ResourceState::Failed;
    }
    if (DXGI_FORMAT_UNKNOWN == d3d12Types::asTextureFormat(setup.ColorFormat)) {
        o_warn("d3d12TextureFactory: unknown texture format!\n");
        return ResourceState::Failed;
    }

    // create d3d12 texture resource
    d3d12ResAllocator& resAllocator = this->pointers.renderer->resAllocator;
    ID3D12Device* d3d12Device = this->pointers.renderer->d3d12Device;
    ID3D12GraphicsCommandList* cmdList = this->pointers.renderer->curCommandList();
    const uint64 frameIndex = this->pointers.renderer->frameIndex;
    tex.slots[0].d3d12TextureRes = resAllocator.AllocTexture(d3d12Device, cmdList, frameIndex, setup, data, size);
    tex.slots[0].d3d12TextureState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    // setup texture attributes
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = setup.Type;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = setup.Width;
    attrs.Height = setup.Height;
    attrs.NumMipMaps = setup.NumMipMaps;
    tex.textureAttrs = attrs;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12TextureFactory::createEmptyTexture(texture& tex) {
    o_error("d3d12TextureFactory::createEmptyTexture: FIXME!");
    return ResourceState::Failed;
}

} // namespace _priv
} // namespace Oryol
