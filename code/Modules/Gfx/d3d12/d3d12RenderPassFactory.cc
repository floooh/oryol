//------------------------------------------------------------------------------
//  d3d12RenderPassFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12RenderPassFactory.h"
#include "Gfx/Resource/resource.h"
#include "Core/Assertion.h"
#include "d3d12_impl.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ResourceState::Code
d3d12RenderPassFactory::SetupResource(renderPass& rp) {
    o_assert_dbg(this->isValid);

    renderPassFactoryBase::SetupResource(rp);
    o_assert_dbg(rp.colorTextures[0]);
    
    d3d12DescAllocator& descAllocator = this->pointers.renderer->descAllocator;
    ID3D12Device* d3d12Device = this->pointers.renderer->d3d12Device;

    // create render-target-views
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        texture* colorTex = rp.colorTextures[i];
        if (colorTex) {
            o_assert_dbg(colorTex->slots[0].d3d12TextureRes);
            const Id& rtvHeap = this->pointers.renderer->rtvHeap;
            rp.rtvDescSlots[i] = descAllocator.AllocSlot(rtvHeap);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle;
            descAllocator.CPUHandle(rtvCPUHandle, rtvHeap, rp.rtvDescSlots[i]);
            d3d12Device->CreateRenderTargetView(colorTex->slots[0].d3d12TextureRes, nullptr, rtvCPUHandle);
        }
    }

    // create optional depth-stencil-view
    if (rp.depthStencilTexture) {
        texture* dsTex = rp.depthStencilTexture;
        const Id& dsvHeap = this->pointers.renderer->dsvHeap;
        rp.dsvDescSlot = descAllocator.AllocSlot(dsvHeap);
        D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUHandle;
        descAllocator.CPUHandle(dsvCPUHandle, dsvHeap, rp.dsvDescSlot);
        d3d12Device->CreateDepthStencilView(dsTex->d3d12DepthStencilTextureRes, nullptr, dsvCPUHandle);
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d12RenderPassFactory::DestroyResource(renderPass& rp) {
    o_assert_dbg(this->isValid);
    d3d12DescAllocator& descAllocator = this->pointers.renderer->descAllocator;
    const uint64_t frameIndex = this->pointers.renderer->frameIndex;
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        if (InvalidIndex != rp.rtvDescSlots[i]) {
            const Id& rtvHeap = this->pointers.renderer->rtvHeap;
            descAllocator.ReleaseSlotDeferred(rtvHeap, frameIndex, rp.rtvDescSlots[i]);
        }
    }
    if (InvalidIndex != rp.dsvDescSlot) {
        const Id& dsvHeap = this->pointers.renderer->dsvHeap;
        descAllocator.ReleaseSlotDeferred(dsvHeap, frameIndex, rp.dsvDescSlot);
    }
    renderPassFactoryBase::DestroyResource(rp);
}

} // namespace _priv
} // namespace Oryol
