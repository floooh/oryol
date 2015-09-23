//------------------------------------------------------------------------------
//  d3d12Renderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Renderer.h"
#include "d3d12_impl.h"
#include "d3d12Types.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/gfxResourceContainer.h"
#include <glm/gtc/type_ptr.hpp>

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12Renderer::d3d12Renderer() :
d3d12Device(nullptr),
d3d12CommandQueue(nullptr),
d3d12RootSignature(nullptr),
frameIndex(0),
curFrameRotateIndex(0),
valid(false),
rtValid(false),
curRenderTarget(nullptr),
curDrawState(nullptr),
d3d12Fence(nullptr),
fenceEvent(nullptr),
msaaSurface(nullptr),
depthStencilSurface(nullptr),
curBackBufferIndex(0),
curConstantBufferOffset(0) {
    this->backbufferSurfaces.Fill(nullptr);
    this->rtvDescriptorSlots.Fill(InvalidIndex);
    this->dsvDescriptorSlot = InvalidIndex;
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
    HRESULT hr;

    this->valid = true;
    this->gfxSetup = setup;
    this->pointers = ptrs;
    this->d3d12Device = ptrs.displayMgr->d3d12Device;
    this->d3d12CommandQueue = ptrs.displayMgr->d3d12CommandQueue;

    this->descAllocator.Setup(setup, ptrs);

    this->createFrameResources(setup.GlobalUniformBufferSize, setup.MaxDrawCallsPerFrame);
    this->createFrameSyncObjects();
    this->createDefaultRenderTargets(setup.Width, setup.Height);
    this->createRootSignature();
   
    // prepare command list for first frame
    hr = this->curCommandList()->Reset(this->curCommandAllocator(), nullptr);
    o_assert(SUCCEEDED(hr));
    this->curCommandList()->SetGraphicsRootSignature(this->d3d12RootSignature);

    // set an initial fence for the 'previous' frame
    hr = this->d3d12CommandQueue->Signal(this->d3d12Fence, 0);
    o_assert(SUCCEEDED(hr));
    this->frameIndex = 1;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::discard() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->d3d12Fence);
    o_assert_dbg(this->fenceEvent);

    // need to wait that the GPU is done before destroying objects
    this->curCommandList()->Close();
    const uint64 waitFrameIndex = this->frameIndex - 1;
    if (this->d3d12Fence->GetCompletedValue() < waitFrameIndex) {
        HRESULT hr = this->d3d12Fence->SetEventOnCompletion(waitFrameIndex, this->fenceEvent);
        o_assert(SUCCEEDED(hr));
        ::WaitForSingleObject(this->fenceEvent, INFINITE);
    }

    this->destroyRootSignature();
    this->destroyDefaultRenderTargets();
    this->destroyFrameSyncObjects();
    this->destroyFrameResources();
    this->resAllocator.DestroyAll();
    this->descAllocator.Discard();
    this->d3d12CommandQueue = nullptr;
    this->d3d12Device = nullptr;
    
    this->pointers = gfxPointers();
    this->valid = false;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::createFrameResources(int32 cbSize, int32 maxDrawCallsPerFrame) {
    HRESULT hr;

    this->curFrameRotateIndex = 0;
    this->curConstantBufferOffset = 0;
    for (auto& frameRes : this->frameResources) {

        o_assert_dbg(nullptr == frameRes.constantBuffer);

        // create a command allocator
        hr = this->d3d12Device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            __uuidof(ID3D12CommandAllocator),
            (void**)&frameRes.commandAllocator);
        o_assert(SUCCEEDED(hr) && frameRes.commandAllocator);

        // create a command list
        hr = this->d3d12Device->CreateCommandList(
            0,                                  // nodeMask
            D3D12_COMMAND_LIST_TYPE_DIRECT,     // type
            frameRes.commandAllocator,          // pCommandAllocator
            nullptr,                            // pInitialState
            __uuidof(ID3D12CommandList),
            (void**)&frameRes.commandList);
        o_assert(SUCCEEDED(hr) && frameRes.commandList);
        hr = frameRes.commandList->Close();
        o_assert(SUCCEEDED(hr));

        // this is the buffer for shader constants that change between drawcalls,
        // it is placed in its own upload heap, written by the CPU and read by the GPU each frame
        frameRes.constantBuffer = this->resAllocator.AllocUploadBuffer(this->d3d12Device, cbSize);
        o_assert_dbg(frameRes.constantBuffer);

        // get the CPU and GPU start address of the constant buffer
        hr = frameRes.constantBuffer->Map(0, nullptr, (void**)&frameRes.cbCpuPtr);
        o_assert(SUCCEEDED(hr));
        frameRes.cbGpuPtr = frameRes.constantBuffer->GetGPUVirtualAddress();
        o_assert_dbg(frameRes.cbGpuPtr);

        // create the shader-resource-view- and sampler-heaps, this has one slot
        // per shader stage and ApplyDrawState call, and each slot
        // has as many descriptors as can be bound to a shader stage
        const int numSlots = ShaderStage::NumShaderStages * this->gfxSetup.MaxApplyDrawStatesPerFrame;
        const int numDescsPerSlot = GfxConfig::MaxNumTexturesPerStage;
        frameRes.srvHeap = this->descAllocator.AllocHeap(d3d12DescAllocator::ShaderResourceView, numSlots, numDescsPerSlot, false);
    }
}

//------------------------------------------------------------------------------
void
d3d12Renderer::destroyFrameResources() {
    for (auto& frameRes : this->frameResources) {
        if (frameRes.constantBuffer) {
            frameRes.constantBuffer->Unmap(0, nullptr);
            this->resAllocator.ReleaseDeferred(this->frameIndex, frameRes.constantBuffer);
            frameRes.constantBuffer = nullptr;
            frameRes.cbCpuPtr = nullptr;
            frameRes.cbGpuPtr = 0;
            frameRes.commandList->Release();
            frameRes.commandList = nullptr;
            frameRes.commandAllocator->Release();
            frameRes.commandAllocator = nullptr;
        }
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
d3d12Renderer::createDefaultRenderTargets(int width, int height) {

    // Here, 2 render-target-views, and optionally one 1 depth-stencil-view
    // are created for rendering into the default render target.
    // For the non-MSAA case, the 2 RSVs are associated with the DXGI
    // swapchain buffers. For the MSAA case, an image resource is created
    // which is the multi-sample surface, and both render-target-views
    // point to this surface (only one RTV would be necessary, but this
    // simplifies other code which only needs to know 1 case both 
    // for MSAA and non-MSAA.
    // For the depth-stencil-view, 1 depth-buffer resource is always
    // created.
    
    o_assert_dbg(nullptr == this->msaaSurface);
    o_assert_dbg(nullptr == this->depthStencilSurface);
    o_assert_dbg(this->pointers.displayMgr->dxgiSwapChain);
    HRESULT hr;
    const DisplayAttrs& dispAttrs = this->pointers.displayMgr->GetDisplayAttrs();
    const int smpCount = dispAttrs.SampleCount;
    const bool isMSAA = smpCount > 1;
    const PixelFormat::Code colorFormat = dispAttrs.ColorPixelFormat;

    // if MSAA is on, create a separate multisample-rendertarget as backbuffer,
    // this will be resolved into the swapchain buffer when needed
    if (isMSAA) {
        this->msaaSurface = this->resAllocator.AllocRenderTarget(this->d3d12Device, width, height, colorFormat, smpCount);
    }

    // create a descriptor heap for render-target-views and depth-stencil-views
    this->rtvHeap = this->descAllocator.AllocHeap(d3d12DescAllocator::RenderTargetView, d3d12Config::MaxNumRenderTargets, 1, true);
    this->dsvHeap = this->descAllocator.AllocHeap(d3d12DescAllocator::DepthStencilView, d3d12Config::MaxNumRenderTargets, 1, true);

    // create NumFrames render-target-views
    for (int i = 0; i < d3d12Config::NumFrames; i++) {
        o_assert_dbg(nullptr == this->backbufferSurfaces[i]);
        hr = this->pointers.displayMgr->dxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&this->backbufferSurfaces[i]);
        o_assert_dbg(SUCCEEDED(hr) && this->backbufferSurfaces[i]);
        
        // allocate a render-target-view descriptor
        this->rtvDescriptorSlots[i] = this->descAllocator.AllocSlot(this->rtvHeap);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
        this->descAllocator.CPUHandle(rtvHandle, this->rtvHeap, this->rtvDescriptorSlots[i]);
        if (isMSAA) {
            // MSAA: render-target-views are bound to msaa surface
            this->d3d12Device->CreateRenderTargetView(this->msaaSurface, nullptr, rtvHandle);
        }
        else {
            // no MSAA: render-target-views are directly bound to swap-chain buffers
            this->d3d12Device->CreateRenderTargetView(this->backbufferSurfaces[i], nullptr, rtvHandle);
        }
    }
    this->curBackBufferIndex = this->pointers.displayMgr->dxgiSwapChain->GetCurrentBackBufferIndex();

    // create a single depth-stencil buffer if requested
    const PixelFormat::Code depthFormat = dispAttrs.DepthPixelFormat;
    if (PixelFormat::None != depthFormat) {
        this->depthStencilSurface = this->resAllocator.AllocRenderTarget(this->d3d12Device, width, height, depthFormat, smpCount);
        this->dsvDescriptorSlot = this->descAllocator.AllocSlot(this->dsvHeap);
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
        this->descAllocator.CPUHandle(dsvHandle, this->dsvHeap, this->dsvDescriptorSlot);
        this->d3d12Device->CreateDepthStencilView(this->depthStencilSurface, nullptr, dsvHandle);
    }
}

//------------------------------------------------------------------------------
void
d3d12Renderer::destroyDefaultRenderTargets() {

    if (this->msaaSurface) {
        this->msaaSurface->Release();
        this->msaaSurface = nullptr;
    }
    if (this->depthStencilSurface) {
        this->depthStencilSurface->Release();
        this->depthStencilSurface = nullptr;
    }
    for (auto& rt : this->backbufferSurfaces) {
        if (rt) {
            rt->Release();
            rt = nullptr;
        }
    }
}

//------------------------------------------------------------------------------
void
d3d12Renderer::createRootSignature() {
    o_assert_dbg(nullptr == this->d3d12RootSignature);
    o_assert_dbg(this->d3d12Device);
    HRESULT hr;

    // currently, all constant buffers can change between draw calls,
    // so they are set directly in the root signature, textures and
    // samplers can only change between draw states, so they are set via descriptors

    // the root signature describes number of constant buffers,
    // textures and samplers that can be bound to the vertex- and pixel-shader
    StaticArray<D3D12_DESCRIPTOR_RANGE, 2> vsRanges;
    Memory::Clear(&vsRanges[0], sizeof(vsRanges[0]) * vsRanges.Size());
    StaticArray<D3D12_DESCRIPTOR_RANGE, 2> psRanges;
    Memory::Clear(&psRanges[0], sizeof(psRanges[0]) * psRanges.Size());

    vsRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    vsRanges[0].NumDescriptors = GfxConfig::MaxNumTextureBlocksPerStage;
    vsRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    vsRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    vsRanges[1].NumDescriptors = GfxConfig::MaxNumTextureBlocksPerStage;
    vsRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    psRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    psRanges[0].NumDescriptors = GfxConfig::MaxNumTextureBlocksPerStage;
    psRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    psRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    psRanges[1].NumDescriptors = GfxConfig::MaxNumTextureBlocksPerStage;
    psRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    StaticArray<D3D12_ROOT_PARAMETER, NumRootParams> rootParams;
    Memory::Clear(&rootParams[0], sizeof(rootParams[0]) * rootParams.Size());

    // vertex shader samplers
    rootParams[VSSamplers].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[VSSamplers].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParams[VSSamplers].DescriptorTable.pDescriptorRanges = &vsRanges[1];
    rootParams[VSSamplers].DescriptorTable.NumDescriptorRanges = 1;

    // pixel shader samplers
    rootParams[PSSamplers].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[PSSamplers].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[PSSamplers].DescriptorTable.pDescriptorRanges = &psRanges[1];
    rootParams[PSSamplers].DescriptorTable.NumDescriptorRanges = 1;

    // vertex shader textures
    rootParams[VSTextures].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[VSTextures].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParams[VSTextures].DescriptorTable.pDescriptorRanges = &vsRanges[0];
    rootParams[VSTextures].DescriptorTable.NumDescriptorRanges = 1;

    // pixel shader textures
    rootParams[PSTextures].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[PSTextures].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[PSTextures].DescriptorTable.pDescriptorRanges = &psRanges[0];
    rootParams[PSTextures].DescriptorTable.NumDescriptorRanges = 1;

    // vertex shader constant buffers
    rootParams[VSConstantBuffer0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[VSConstantBuffer0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParams[VSConstantBuffer0].Constants.ShaderRegister = 0;
    rootParams[VSConstantBuffer0].Constants.RegisterSpace = 0;
   
    rootParams[VSConstantBuffer1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[VSConstantBuffer1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParams[VSConstantBuffer1].Constants.ShaderRegister = 1;
    rootParams[VSConstantBuffer1].Constants.RegisterSpace = 0;

    // pixel shader constant buffers
    rootParams[PSConstantBuffer0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[PSConstantBuffer0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[PSConstantBuffer0].Constants.ShaderRegister = 0;
    rootParams[PSConstantBuffer0].Constants.RegisterSpace = 0;

    rootParams[PSConstantBuffer1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[PSConstantBuffer1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[PSConstantBuffer1].Constants.ShaderRegister = 1;
    rootParams[PSConstantBuffer1].Constants.RegisterSpace = 0;

    D3D12_ROOT_SIGNATURE_DESC rootDesc;
    Memory::Clear(&rootDesc, sizeof(rootDesc));
    rootDesc.NumParameters = rootParams.Size();
    rootDesc.pParameters = &rootParams[0];
    rootDesc.NumStaticSamplers = 0;
    rootDesc.pStaticSamplers = nullptr;
    rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    ID3DBlob* sig = nullptr;
    ID3DBlob* error = nullptr;
    hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &error); 
    if (FAILED(hr)) {
        o_assert(error);
        o_error("d3d12Renderer: Failed to create root signature with: %s\n", error->GetBufferPointer());
    }
    hr = this->d3d12Device->CreateRootSignature(
        0,                                  // NodeMask 
        sig->GetBufferPointer(),            // pBlobWithRootSignature
        sig->GetBufferSize(),               // blobLengthInBytes
        __uuidof(ID3D12RootSignature),      // riid
        (void**)&this->d3d12RootSignature); // ppvRootSignature
    o_assert(SUCCEEDED(hr) && this->d3d12RootSignature);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::destroyRootSignature() {
    if (this->d3d12RootSignature) {
        this->d3d12RootSignature->Release();
        this->d3d12RootSignature = nullptr;
    }
}

//------------------------------------------------------------------------------
bool
d3d12Renderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::commitFrame() {
    o_assert_dbg(this->valid);
    const DisplayAttrs& dispAttrs = this->pointers.displayMgr->GetDisplayAttrs();
    const bool isMSAA = dispAttrs.SampleCount > 1;

    // in MSAA situation, need to resolve the MSAA render target into non-multisampled backbuffer surfaces
    ID3D12Resource* curBackBuffer = this->backbufferSurfaces[this->curBackBufferIndex];
    if (isMSAA) {
        const DXGI_FORMAT d3d12Fmt = d3d12Types::asRenderTargetFormat(dispAttrs.ColorPixelFormat);
        this->rtTransition(this->msaaSurface, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
        this->rtTransition(curBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RESOLVE_DEST);
        this->curCommandList()->ResolveSubresource(curBackBuffer, 0,this->msaaSurface, 0, d3d12Fmt);
        this->rtTransition(curBackBuffer, D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_PRESENT);
        this->rtTransition(this->msaaSurface, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    }
    else {
        // transition the default render target back from render target to present state
        this->rtTransition(curBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    }

    // execute the command list, after this, displayMgr::present() is called,
    // which calls Present on the swapchain and waits for the previous frame to finish
    ID3D12GraphicsCommandList* cmdList = this->curCommandList();
    cmdList->Close();
    this->d3d12CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&cmdList);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::frameSync() {
    // NOTE: this method is called from displayMgr::Present(), and after d3d12Renderer::commitFrame()!
    o_assert_dbg(this->d3d12Fence);
    o_assert_dbg(this->fenceEvent);
    HRESULT hr;

    // set a fence with the current frame index...
    const uint64 newFenceValue = this->frameIndex;
    hr = this->d3d12CommandQueue->Signal(this->d3d12Fence, newFenceValue);
    o_assert(SUCCEEDED(hr));

    // wait on the previous frame
    uint64 waitFenceValue = this->frameIndex - 1;
    if (this->d3d12Fence->GetCompletedValue() < waitFenceValue) {
        hr = this->d3d12Fence->SetEventOnCompletion(waitFenceValue, this->fenceEvent);
        o_assert(SUCCEEDED(hr));
        ::WaitForSingleObject(this->fenceEvent, INFINITE);
    }

    // deferred-release resources
    this->resAllocator.GarbageCollect(this->frameIndex);
    this->descAllocator.GarbageCollect(this->frameIndex);

    // bump frame indices, this rotates to curFrameRotateIndex
    // to the frameResource slot of the previous frame, these
    // resource are now no longer used by the GPU and will
    // be written in the next frame
    this->frameIndex++;
    if (++this->curFrameRotateIndex >= d3d12Config::NumFrames) {
        this->curFrameRotateIndex = 0;
    }

    // reset the rotated command allocator and command list for next frame
    hr = this->curCommandAllocator()->Reset();
    o_assert(SUCCEEDED(hr));
    hr = this->curCommandList()->Reset(this->curCommandAllocator(), nullptr);
    o_assert(SUCCEEDED(hr));

    // lookup new backbuffer index
    this->curBackBufferIndex = this->pointers.displayMgr->dxgiSwapChain->GetCurrentBackBufferIndex();

    // prepare for next frame
    o_assert_dbg(this->d3d12RootSignature);
    this->curCommandList()->SetGraphicsRootSignature(this->d3d12RootSignature);
    this->curConstantBufferOffset = 0;
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
d3d12Renderer::rtTransition(ID3D12Resource* rt, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
    o_assert_dbg(rt);
    D3D12_RESOURCE_BARRIER barrier;
    Memory::Clear(&barrier, sizeof(barrier));
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = rt;
    barrier.Transition.StateBefore = before;
    barrier.Transition.StateAfter = after;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    this->curCommandList()->ResourceBarrier(1, &barrier);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyRenderTarget(texture* rt, const ClearState& clearState) {
    o_assert_dbg(this->valid);
    const displayMgr* dispMgr = this->pointers.displayMgr;
    ID3D12GraphicsCommandList* cmdList = this->curCommandList();

    ID3D12Resource* colorBuffer = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE* rtvCPUHandlePtr = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE* dsvCPUHandlePtr = nullptr; 
    D3D12_RESOURCE_STATES colorStateBefore = (D3D12_RESOURCE_STATES) 0xFFFFFFFF;
    this->invalidateTextureState();
    if (nullptr == rt) {
        const bool isMSAA = this->pointers.displayMgr->GetDisplayAttrs().SampleCount > 1;
        this->rtAttrs = dispMgr->GetDisplayAttrs();
        if (isMSAA) {
            colorBuffer = this->msaaSurface;
            colorStateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        }
        else {
            colorBuffer = this->backbufferSurfaces[this->curBackBufferIndex];
            colorStateBefore = D3D12_RESOURCE_STATE_PRESENT;
        }
        this->descAllocator.CPUHandle(rtvCPUHandle, this->rtvHeap, this->rtvDescriptorSlots[this->curBackBufferIndex]);
        rtvCPUHandlePtr = &rtvCPUHandle;
        if (this->depthStencilSurface) {
            this->descAllocator.CPUHandle(dsvCPUHandle, this->dsvHeap, this->dsvDescriptorSlot);
            dsvCPUHandlePtr = &dsvCPUHandle;
        }
    }
    else {
        o_warn("FIXME: d3d12Renderer apply offscreen render target!\n");
        // FIXME: the render target could have been a pixel-shader or
        // vertex-shader-texture, d3d12Texture objects must track their current
        // states (hmm does that mean a texture cannot be used as a vertex shader
        // texture and a pixel shader texture at the same time?
    }
    o_assert_dbg(colorBuffer && (colorStateBefore != 0xFFFFFFFF));

    this->curRenderTarget = rt;
    this->rtValid = true;

    // transition color buffer into render target state
    if (D3D12_RESOURCE_STATE_RENDER_TARGET != colorStateBefore) {
        this->rtTransition(colorBuffer, colorStateBefore, D3D12_RESOURCE_STATE_RENDER_TARGET);
    }

    // set the render target
    cmdList->OMSetRenderTargets(1, rtvCPUHandlePtr, FALSE, dsvCPUHandlePtr);

    // set viewport and scissor-rect to cover whole screen
    this->applyViewPort(0, 0, this->rtAttrs.FramebufferWidth, this->rtAttrs.FramebufferHeight, true);
    this->applyScissorRect(0, 0, this->rtAttrs.FramebufferWidth, this->rtAttrs.FramebufferHeight, true);

    // perform clear action
    if (clearState.Actions & ClearState::ColorBit) {
        if (rtvCPUHandlePtr) {
            const FLOAT* clearColor = glm::value_ptr(clearState.Color);        
            cmdList->ClearRenderTargetView(rtvCPUHandle, clearColor, 0, nullptr);
        }
    }
    if (clearState.Actions & ClearState::DepthStencilBits) {
        if (dsvCPUHandlePtr) {
            D3D12_CLEAR_FLAGS d3d12ClearFlags = (D3D12_CLEAR_FLAGS) 0;
            if (clearState.Actions & ClearState::DepthBit) {
                d3d12ClearFlags |= D3D12_CLEAR_FLAG_DEPTH;
            }
            if (clearState.Actions & ClearState::StencilBit) {
                d3d12ClearFlags |= D3D12_CLEAR_FLAG_STENCIL;
            }
            const FLOAT d = clearState.Depth;
            const UINT8 s = clearState.Stencil;
            cmdList->ClearDepthStencilView(dsvCPUHandle, d3d12ClearFlags, d, s, 0, nullptr);
        }
    }
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    D3D12_VIEWPORT vp;
    vp.TopLeftX = (FLOAT)x;
    vp.TopLeftY = (FLOAT)(originTopLeft ? y : (this->rtAttrs.FramebufferHeight - (y + height)));
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    this->curCommandList()->RSSetViewports(1, &vp);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    D3D12_RECT rect;
    rect.left = x;
    rect.top = originTopLeft ? y : this->rtAttrs.FramebufferHeight - (y + height);
    rect.right = x + width;
    rect.bottom = originTopLeft ? (y + height) : (this->rtAttrs.FramebufferHeight - y);
    this->curCommandList()->RSSetScissorRects(1, &rect);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyDrawState(drawState* ds) {
    if (nullptr == ds) {
        // the drawState is pending (dependent resource still loading),
        // invalidate rendering for followup draw calls
        this->curDrawState = nullptr;
        return;
    }
    this->curDrawState = ds;
    ID3D12GraphicsCommandList* cmdList = this->curCommandList();

    o_assert_dbg(ds->d3d12PipelineState);
    o_assert2(ds->Setup.BlendState.ColorFormat == this->rtAttrs.ColorPixelFormat, "ColorFormat in BlendState must match current render target!\n");
    o_assert2(ds->Setup.BlendState.DepthFormat == this->rtAttrs.DepthPixelFormat, "DepthFormat in BlendState must match current render target!\n");
    o_assert2(ds->Setup.RasterizerState.SampleCount == this->rtAttrs.SampleCount, "SampleCount in RasterizerState must match current render target!\n");

    // apply pipeline state (FIXME: only if changed!)
    cmdList->SetPipelineState(ds->d3d12PipelineState);
    
    // apply vertex and index buffers (FIXME: only if changed!)
    D3D12_VERTEX_BUFFER_VIEW vbViews[GfxConfig::MaxNumInputMeshes];
    D3D12_INDEX_BUFFER_VIEW ibView;
    const D3D12_INDEX_BUFFER_VIEW* ibViewPtr = nullptr;
    bool hasIndexBuffer = false;
    for (int i = 0; i < GfxConfig::MaxNumInputMeshes; i++) {
        const mesh* msh = ds->meshes[i];
        if (msh) {
            const mesh::buffer& vb = msh->buffers[mesh::vb];
            o_assert_dbg(vb.d3d12DefaultBuffers[vb.activeSlot]);
            vbViews[i].BufferLocation = vb.d3d12DefaultBuffers[vb.activeSlot]->GetGPUVirtualAddress();
            vbViews[i].SizeInBytes = msh->vertexBufferAttrs.ByteSize();
            vbViews[i].StrideInBytes = msh->vertexBufferAttrs.Layout.ByteSize();
            if ((0 == i) && (IndexType::None != msh->indexBufferAttrs.Type)) {
                const mesh::buffer& ib = msh->buffers[mesh::ib];
                o_assert_dbg(ib.d3d12DefaultBuffers[ib.activeSlot]);
                ibView.BufferLocation = ib.d3d12DefaultBuffers[ib.activeSlot]->GetGPUVirtualAddress();
                ibView.SizeInBytes = msh->indexBufferAttrs.ByteSize();
                ibView.Format = d3d12Types::asIndexType(msh->indexBufferAttrs.Type);
                ibViewPtr = &ibView;
            }
        }
        else {
            vbViews[i].BufferLocation = 0;
            vbViews[i].SizeInBytes = 0;
            vbViews[i].StrideInBytes = 0;
        }
    }
    cmdList->IASetVertexBuffers(0, GfxConfig::MaxNumInputMeshes, vbViews);
    cmdList->IASetIndexBuffer(ibViewPtr);
    cmdList->IASetPrimitiveTopology(ds->meshes[0]->d3d12PrimTopology);
    cmdList->OMSetBlendFactor(glm::value_ptr(ds->Setup.BlendColor));
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyUniformBlock(ShaderStage::Code bindStage, int32 bindSlot, int64 layoutHash, const uint8* ptr, int32 byteSize) {
    if (nullptr == this->curDrawState) {
        return;
    }

    #if ORYOL_DEBUG
    // verify that the provided uniform-block is type-compatible with 
    // the uniform-block expected at the binding stage and slot
    const shader* shd = this->curDrawState->shd;
    o_assert_dbg(shd);
    int32 ubIndex = shd->Setup.UniformBlockIndexByStageAndSlot(bindStage, bindSlot);
    const UniformBlockLayout& layout = shd->Setup.UniformBlockLayout(ubIndex);
    o_assert2(layout.TypeHash == layoutHash, "incompatible uniform block!\n");
    o_assert(byteSize == layout.ByteSize());
    #endif

    // copy uniform data into global constant buffer
    o_assert2((this->curConstantBufferOffset + byteSize) <= this->gfxSetup.GlobalUniformBufferSize, "Global constant buffer exhausted!");
    const auto& frameRes = this->frameResources[this->curFrameRotateIndex];
    uint8* dstPtr = frameRes.cbCpuPtr + this->curConstantBufferOffset;
    std::memcpy(dstPtr, ptr, byteSize);

    // get the GPU address of current constant buffer location and set
    // the constant buffer location directly in the root signature
    // the root parameter index can be: VSConstantBuffer0, VSConstantBuffer1, 
    // PSConstantBuffer0 or PSConstantBuffer1
    const uint64 cbGpuPtr = frameRes.cbGpuPtr + this->curConstantBufferOffset;
    static const int maxHighFreqCBs = 2;
    o_assert_dbg(bindSlot < maxHighFreqCBs);
    UINT rootParamIndex = ShaderStage::VS == bindStage ? VSConstantBuffer0 : PSConstantBuffer0;
    rootParamIndex += bindSlot * maxHighFreqCBs;
    this->curCommandList()->SetGraphicsRootConstantBufferView(rootParamIndex, cbGpuPtr);

    // advance constant buffer offset (must be multiple of 256)
    this->curConstantBufferOffset = Memory::RoundUp(this->curConstantBufferOffset + byteSize, 256);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyTextureBlock(ShaderStage::Code bindStage, int32 bindSlot, int64 layoutHash, texture** textures, int32 numTextures) {
    o_warn("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::draw(const PrimitiveGroup& primGroup) {
    o_assert2_dbg(this->rtValid, "No render target set!\n");
    if (nullptr == this->curDrawState) {
        return;
    }
    const IndexType::Code indexType = this->curDrawState->meshes[0]->indexBufferAttrs.Type;
    if (indexType != IndexType::None) {
        this->curCommandList()->DrawIndexedInstanced(
            primGroup.NumElements,  // IndexCountPerInstance
            1,                      // InstanceCount
            primGroup.BaseElement,  // StartIndexLocation
            0,                      // BaseVertexLocation
            0);                     // StartInstanceLocation
    }
    else {
        this->curCommandList()->DrawInstanced(
            primGroup.NumElements,  // VertexCountPerInstance
            1,                      // InstanceCount
            primGroup.BaseElement,  // StartVertexLocation
            0);                     // StartInstanceLocation
    }
}

//------------------------------------------------------------------------------
void
d3d12Renderer::draw(int32 primGroupIndex) {
    o_assert_dbg(this->valid);
    if (nullptr == this->curDrawState) {
        return;
    }
    o_assert_dbg(this->curDrawState->meshes[0]);
    if (primGroupIndex >= this->curDrawState->meshes[0]->numPrimGroups) {
        // this may happen if trying to render a placeholder which doesn't
        // have as many materials as the original mesh, anyway, this isn't
        // a serious error
        return;
    }
    const PrimitiveGroup& primGroup = this->curDrawState->meshes[0]->primGroups[primGroupIndex];
    this->draw(primGroup);
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

