//------------------------------------------------------------------------------
//  d3d12Renderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Renderer.h"
#include "d3d12_impl.h"
#include "Gfx/Core/displayMgr.h"

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
d3d12Fence(nullptr),
fenceEvent(nullptr) {
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
    o_assert_dbg(0 == frameIndex);
    HRESULT hr;

    this->valid = true;
    this->pointers = ptrs;
    this->d3d12Device = ptrs.displayMgr->d3d12Device;
    this->d3d12CommandQueue = ptrs.displayMgr->d3d12CommandQueue;

    hr = this->d3d12Device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        __uuidof(ID3D12CommandAllocator),
        (void**) &this->d3d12CommandAllocator); 
    o_assert(SUCCEEDED(hr) && this->d3d12CommandAllocator);

    hr = this->d3d12Device->CreateCommandList(
        0,                                  // nodeMask
        D3D12_COMMAND_LIST_TYPE_DIRECT,     // type
        this->d3d12CommandAllocator,        // pCommandAllocator
        nullptr,    // pInitialState (FIXME: the docs say this must be a valid pipeline state, but the samples use nullptr?)
        __uuidof(ID3D12CommandList),
        (void**) &this->d3d12CommandList);
    o_assert(SUCCEEDED(hr) && this->d3d12CommandList); 

    // create the frame sync objects
    hr = this->d3d12Device->CreateFence(
        0,                                  // InitialValue
        D3D12_FENCE_FLAG_NONE,              // Flags
        __uuidof(ID3D12Fence), 
        (void**) &this->d3d12Fence);
    o_assert(SUCCEEDED(hr) && this->d3d12Fence);

    this->fenceEvent = ::CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    o_assert(nullptr != this->fenceEvent);

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
    this->waitForPreviousFrame();

    ::CloseHandle(this->fenceEvent);
    this->fenceEvent = nullptr;
    this->d3d12Fence->Release();
    this->d3d12Fence = nullptr;
    this->d3d12CommandList->Release();
    this->d3d12CommandList = nullptr;
    this->d3d12CommandAllocator->Release();
    this->d3d12CommandAllocator = nullptr;
    this->d3d12CommandQueue = nullptr;
    this->d3d12Device = nullptr;
    
    this->pointers = gfxPointers();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
d3d12Renderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::waitForPreviousFrame() {
    // NOTE: this method is called from displayMgr::Present()!
    o_assert_dbg(this->d3d12Fence);
    o_assert_dbg(this->fenceEvent);
    o_assert_dbg(this->d3d12CommandAllocator);
    o_assert_dbg(this->d3d12CommandList);
    HRESULT hr;

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
}

//------------------------------------------------------------------------------
void
d3d12Renderer::commitFrame() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->d3d12CommandList);

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

