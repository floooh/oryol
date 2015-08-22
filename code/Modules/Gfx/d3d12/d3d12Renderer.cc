//------------------------------------------------------------------------------
//  d3d12Renderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Renderer.h"
#include "d3d12_impl.h"
#include "Gfx/Core/displayMgr.h"
#include <glm/gtc/type_ptr.hpp>

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12Renderer::d3d12Renderer() :
d3d12Device(nullptr),
d3d12CommandQueue(nullptr),
d3d12CommandAllocator(nullptr),
d3d12CommandList(nullptr),
frameIndex(0),
valid(false),
rtValid(false),
curRenderTarget(nullptr),
curDrawState(nullptr),
d3d12Fence(nullptr),
fenceEvent(nullptr),
d3d12RTVHeap(nullptr),
rtvDescriptorSize(0),
curBackbufferIndex(0) {
    this->d3d12RenderTargets.Fill(nullptr);
}

//------------------------------------------------------------------------------
d3d12Renderer::~d3d12Renderer() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert_dbg(!this->valid);
    o_assert_dbg(ptrs.displayMgr->d3d12Device);
    o_assert_dbg(ptrs.displayMgr->d3d12CommandQueue);
    o_assert_dbg(0 == this->frameIndex);

    this->valid = true;
    this->pointers = ptrs;
    this->d3d12Device = ptrs.displayMgr->d3d12Device;
    this->d3d12CommandQueue = ptrs.displayMgr->d3d12CommandQueue;

    this->createCommandAllocator();
    this->createCommandList();
    this->createFrameSyncObjects();
    this->createDefaultRenderTargets();

    this->frameIndex = 1;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::discard() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->d3d12CommandAllocator);
    o_assert_dbg(this->d3d12CommandList);
    o_assert_dbg(this->d3d12Fence);
    o_assert_dbg(this->fenceEvent);

    // need to wait that the GPU is done before destroying objects
    this->d3d12CommandList->Close();
    this->frameSync();

    this->destroyDefaultRenderTargets();
    this->destroyFrameSyncObjects();
    this->destroyCommandList();
    this->destroyCommandAllocator();
    this->d3d12CommandQueue = nullptr;
    this->d3d12Device = nullptr;
    
    this->pointers = gfxPointers();
    this->valid = false;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::createCommandAllocator() {
    o_assert_dbg(this->d3d12Device);
    o_assert_dbg(nullptr == this->d3d12CommandAllocator);

    // FIXME: I think we need d3d12Config::NumFrames allocators,
    // and rotate through them so that a new command list can
    // start recording before the previous is finished
    HRESULT hr = this->d3d12Device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        __uuidof(ID3D12CommandAllocator),
        (void**)&this->d3d12CommandAllocator);
    o_assert(SUCCEEDED(hr) && this->d3d12CommandAllocator);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::destroyCommandAllocator() {
    if (this->d3d12CommandAllocator) {
        this->d3d12CommandAllocator->Release();
        this->d3d12CommandAllocator = nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d12Renderer::createCommandList() {
    o_assert_dbg(this->d3d12Device);
    o_assert_dbg(this->d3d12CommandAllocator);
    o_assert_dbg(nullptr == this->d3d12CommandList);

    HRESULT hr = this->d3d12Device->CreateCommandList(
        0,                                  // nodeMask
        D3D12_COMMAND_LIST_TYPE_DIRECT,     // type
        this->d3d12CommandAllocator,        // pCommandAllocator
        nullptr,    // pInitialState (FIXME: the docs say this must be a valid pipeline state, but the samples use nullptr?)
        __uuidof(ID3D12CommandList),
        (void**)&this->d3d12CommandList);
    o_assert(SUCCEEDED(hr) && this->d3d12CommandList);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::destroyCommandList() {
    if (this->d3d12CommandList) {
        this->d3d12CommandList->Release();
        this->d3d12CommandList = nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d12Renderer::createFrameSyncObjects() {
    o_assert_dbg(this->d3d12Device);
    o_assert_dbg(nullptr == this->d3d12Fence);
    o_assert_dbg(nullptr == this->fenceEvent);
    
    HRESULT hr = this->d3d12Device->CreateFence(
        0,                                  // InitialValue
        D3D12_FENCE_FLAG_NONE,              // Flags
        __uuidof(ID3D12Fence),
        (void**)&this->d3d12Fence);
    o_assert(SUCCEEDED(hr) && this->d3d12Fence);

    this->fenceEvent = ::CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    o_assert(nullptr != this->fenceEvent);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::destroyFrameSyncObjects() {
    if (this->fenceEvent) {
        ::CloseHandle(this->fenceEvent);
        this->fenceEvent = nullptr;
    }
    if (this->d3d12Fence) {
        this->d3d12Fence->Release();
        this->d3d12Fence = nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d12Renderer::createDefaultRenderTargets() {
    o_assert_dbg(nullptr == this->d3d12RTVHeap);
    o_assert_dbg(this->pointers.displayMgr->dxgiSwapChain);
    HRESULT hr;

    // create a render-target-view heap with NumFrames entries
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    Memory::Clear(&heapDesc, sizeof(heapDesc));
    heapDesc.NumDescriptors = d3d12Config::NumFrames;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    hr = this->d3d12Device->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&this->d3d12RTVHeap);
    o_assert(SUCCEEDED(hr) && this->d3d12RTVHeap);
    this->rtvDescriptorSize = this->d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    o_assert(this->rtvDescriptorSize > 0);

    // create NumFrames render-target-views from the render-target-view-heap
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = this->d3d12RTVHeap->GetCPUDescriptorHandleForHeapStart();
    for (int i = 0; i < d3d12Config::NumFrames; i++) {
        o_assert_dbg(nullptr == this->d3d12RenderTargets[i]);
        hr = this->pointers.displayMgr->dxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&this->d3d12RenderTargets[i]);
        o_assert_dbg(SUCCEEDED(hr) && this->d3d12RenderTargets[i]);
        this->d3d12Device->CreateRenderTargetView(this->d3d12RenderTargets[i], nullptr, rtvHandle);
        rtvHandle.ptr += this->rtvDescriptorSize;
    }
    this->curBackbufferIndex = this->pointers.displayMgr->dxgiSwapChain->GetCurrentBackBufferIndex();

    o_warn("d3d12Renderer::createDefaultRenderTarget: create depth-stencil buffer(s?)\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::destroyDefaultRenderTargets() {

    // FIXME: destroy depth-stencil buffer!
    o_warn("Destroy depth-stencil buffer!\n");
    
    // release the render target resource objects
    for (auto& rt : this->d3d12RenderTargets) {
        if (rt) {
            rt->Release();
            rt = nullptr;
        }
    }

    // release the render-target-view heap
    if (this->d3d12RTVHeap) {
        this->d3d12RTVHeap->Release();
        this->d3d12RTVHeap = nullptr;
    }
}

//------------------------------------------------------------------------------
bool
d3d12Renderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::frameSync() {
    // NOTE: this method is called from displayMgr::Present()!
    o_assert_dbg(this->d3d12Fence);
    o_assert_dbg(this->fenceEvent);
    o_assert_dbg(this->d3d12CommandAllocator);
    o_assert_dbg(this->d3d12CommandList);
    HRESULT hr;

    // FIXME: what this actually does is wait for the CURRENT frame to finish
    // that has just been recorded, what we actually need to do is wait for
    // the frame BEFORE to finish, and start recording a new command list
    // while the current one is executing

    // FIXME REMINDER: the d3d12 hello world sample says that waiting for the
    // previous frame to finish is not best practice
    const uint64 fenceValue = this->frameIndex++;
    hr = this->d3d12CommandQueue->Signal(this->d3d12Fence, fenceValue);
    o_assert(SUCCEEDED(hr));

    // wait until previous frame is finished
    if (this->d3d12Fence->GetCompletedValue() < fenceValue) {
        hr = this->d3d12Fence->SetEventOnCompletion(fenceValue, this->fenceEvent);
        ::WaitForSingleObject(this->fenceEvent, INFINITE);
    }

    // reset the command allocator and command list for next frame
    hr = this->d3d12CommandAllocator->Reset();
    o_assert(SUCCEEDED(hr));
    hr = this->d3d12CommandList->Reset(this->d3d12CommandAllocator, nullptr);
    o_assert(SUCCEEDED(hr));

    // lookup new backbuffer index
    this->curBackbufferIndex = this->pointers.displayMgr->dxgiSwapChain->GetCurrentBackBufferIndex();
}

//------------------------------------------------------------------------------
void
d3d12Renderer::commitFrame() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->d3d12CommandList);

    // transition the default render target back from render target to present state
    const displayMgr* dispMgr = this->pointers.displayMgr;
    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = this->d3d12RenderTargets[this->curBackbufferIndex];
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    this->d3d12CommandList->ResourceBarrier(1, &barrier);

    // execute the command list, after this, displayMgr::present() is called,
    // which calls Present on the swapchain and waits for the previous
    // frame to finish
    this->d3d12CommandList->Close();
    this->d3d12CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**) &this->d3d12CommandList);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::resetStateCache() {
    o_warn("d3d12Renderer::resetStateCache()\n");
}

//------------------------------------------------------------------------------
bool
d3d12Renderer::queryFeature(GfxFeature::Code feat) const {
    switch (feat) {
        case GfxFeature::TextureCompressionDXT:
        case GfxFeature::TextureFloat:
        case GfxFeature::Instancing:
        case GfxFeature::OriginTopLeft:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
const DisplayAttrs&
d3d12Renderer::renderTargetAttrs() const {
    o_warn("d3d12Renderer::renderTargetAttrs()\n");
    static DisplayAttrs dummyAttrs;
    return dummyAttrs;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyRenderTarget(texture* rt, const ClearState& clearState) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->d3d12CommandList);
    const displayMgr* dispMgr = this->pointers.displayMgr;

    ID3D12Resource* colorRenderTarget = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE colorRTVHandle = { 0 };
    D3D12_RESOURCE_STATES colorStateBefore = (D3D12_RESOURCE_STATES) 0xFFFFFFFF;
    this->invalidateTextureState();
    if (nullptr == rt) {
        this->rtAttrs = dispMgr->GetDisplayAttrs();
        colorRenderTarget = this->d3d12RenderTargets[this->curBackbufferIndex];
        colorStateBefore = D3D12_RESOURCE_STATE_PRESENT;        
        colorRTVHandle = this->d3d12RTVHeap->GetCPUDescriptorHandleForHeapStart();
        colorRTVHandle.ptr += this->rtvDescriptorSize * this->curBackbufferIndex;
    }
    else {
        o_error("FIXME: d3d12Renderer apply offscreen render target!\n");
        // FIXME: the render target could have been a pixel-shader or
        // vertex-shader-texture, d3d12Texture objects must track their current
        // states (hmm does that mean a texture cannot be used as a vertex shader
        // texture and a pixel shader texture at the same time?
    }
    o_assert_dbg(colorRenderTarget && (colorStateBefore != 0xFFFFFFFF));

    this->curRenderTarget = rt;
    this->rtValid = true;

    // transition resource into render target state
    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = colorRenderTarget;
    barrier.Transition.StateBefore = colorStateBefore;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    this->d3d12CommandList->ResourceBarrier(1, &barrier);

    // set the render target
    this->d3d12CommandList->OMSetRenderTargets(1, &colorRTVHandle, FALSE, nullptr);

    // set viewport to cover whole screen
    this->applyViewPort(0, 0, this->rtAttrs.FramebufferWidth, this->rtAttrs.FramebufferHeight, true);

    // perform clear action
    if (clearState.Actions & ClearState::ColorBit) {
        if (colorRTVHandle.ptr) {
            const FLOAT* clearColor = glm::value_ptr(clearState.Color);        
            this->d3d12CommandList->ClearRenderTargetView(colorRTVHandle, clearColor, 0, nullptr);
        }
    }
    if (clearState.Actions & ClearState::DepthStencilBits) {
        o_warn("FIXME: clear depth-stencil buffer!\n");
    }

    o_warn("d3d12Renderer::applyRenderTarget()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_warn("d3d12Renderer::applyViewPort()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_warn("d3d12Renderer::applyScissorRect()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyDrawState(drawState* ds) {
    o_warn("d3d12Renderer::applyDrawState()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyUniformBlock(int32 blockIndex, int64 layoutHash, const uint8* ptr, int32 byteSize) {
    o_warn("d3d12Renderer::applyUniformBlock()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::draw(const PrimitiveGroup& primGroup) {
    o_warn("d3d12Renderer::draw()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::draw(int32 primGroupIndex) {
    o_warn("d3d12Renderer::draw()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_warn("d3d12Renderer::drawInstanced()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::drawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_warn("d3d12Renderer::drawInstanced()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
    o_warn("d3d12Renderer::updateVertices()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::updateIndices(mesh* msh, const void* data, int32 numBytes) {
    o_warn("d3d12Renderer::updateIndices()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::readPixels(void* buf, int32 bufNumBytes) {
    o_warn("d3d12Renderer::readPixels()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::invalidateMeshState() {
    o_warn("d3d12Renderer::invalidateMeshState()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::invalidateShaderState() {
    o_warn("d3d12Renderer::invalidateShaderState()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::invalidateDrawState() {
    o_warn("d3d12Renderer::invalidateDrawState()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::invalidateTextureState() {
    o_warn("d3d12Renderer::invalidateTextureState()\n");
}

} // namespace _priv
} // namespace Oryol

