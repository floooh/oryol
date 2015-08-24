//------------------------------------------------------------------------------
//  d3d12ResourceAllocator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12ResourceAllocator.h"
#include "d3d12_impl.h"
#include "d3d12Config.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12ResourceAllocator::d3d12ResourceAllocator() :
d3d12Device(nullptr),
valid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d12ResourceAllocator::~d3d12ResourceAllocator() {
    if (this->valid) {
        this->Discard();
    }
}

//------------------------------------------------------------------------------
void
d3d12ResourceAllocator::Setup(ID3D12Device* device) {
    o_assert_dbg(!this->valid);
    o_assert_dbg(device);

    this->valid = true;
    this->d3d12Device = device;
}

//------------------------------------------------------------------------------
void
d3d12ResourceAllocator::Discard() {
    o_assert_dbg(this->valid);
    this->valid = false;
    
    // release any remaining resources (GPU must be done when calling the method)
    freeItem item;
    while (!this->releaseQueue.Empty()) {
        this->releaseQueue.Dequeue(item);
        ULONG count = item.res->Release();
        o_assert_dbg(0 == count);
        Log::Dbg("> released d3d12 buffer %p at shutdown\n", item.res);
    }
    this->d3d12Device = nullptr;
}

//------------------------------------------------------------------------------
bool
d3d12ResourceAllocator::IsValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
d3d12ResourceAllocator::GarbageCollect(uint64 frameIndex) {
    o_assert_dbg(this->valid);

    // release all resources from longer then NumFrames befores,
    // these are definitely no longer accessed by the GPU
    const uint64 safeNumFrames = d3d12Config::NumFrames + 1;
    if (frameIndex > safeNumFrames) {
        const uint64 minReleaseFrame = frameIndex - safeNumFrames;
        freeItem item;
        while (!this->releaseQueue.Empty() && (this->releaseQueue.Front().frameIndex < minReleaseFrame)) {
            this->releaseQueue.Dequeue(item);
            o_assert_dbg(item.res);
            ULONG count = item.res->Release();
            o_assert_dbg(0 == count);
            Log::Dbg("> released d3d12 buffer %p at frame %d\n", item.res, frameIndex);
        }
    }
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResourceAllocator::createBuffer(D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, uint32 size) {

    D3D12_HEAP_PROPERTIES heapProperties;
    Memory::Clear(&heapProperties, sizeof(heapProperties));
    heapProperties.Type = heapType;

    D3D12_RESOURCE_DESC resourceDesc;
    Memory::Clear(&resourceDesc, sizeof(resourceDesc));
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = size;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ID3D12Resource* d3d12Resource = nullptr;
    HRESULT hr = this->d3d12Device->CreateCommittedResource(
        &heapProperties,                // pHeapProperties
        D3D12_HEAP_FLAG_NONE,           // HeapFlags
        &resourceDesc,                  // pResourceDesc
        initialState,                   // InitialResourceState
        nullptr,                        // pOptimizedClearValue
        __uuidof(ID3D12Resource),
        (void**) &d3d12Resource);
    o_assert(SUCCEEDED(hr) && d3d12Resource);
    
    return d3d12Resource;
}

//------------------------------------------------------------------------------
void
d3d12ResourceAllocator::transition(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* res, D3D12_RESOURCE_STATES fromState, D3D12_RESOURCE_STATES toState) {
    o_assert_dbg(cmdList);
    o_assert_dbg(res);
    o_assert_dbg(fromState != toState);

    D3D12_RESOURCE_BARRIER resBarrier;
    Memory::Clear(&resBarrier, sizeof(resBarrier));
    resBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    resBarrier.Transition.pResource = res;
    resBarrier.Transition.Subresource = 0;
    resBarrier.Transition.StateBefore = fromState;
    resBarrier.Transition.StateAfter = toState;
    cmdList->ResourceBarrier(1, &resBarrier);
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResourceAllocator::AllocBuffer(ID3D12GraphicsCommandList* cmdList, uint64 frameIndex, const void* data, uint32 size) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->d3d12Device);
    o_assert_dbg(cmdList);
    o_assert_dbg(size > 0);

    // create the default-heap buffer
    ID3D12Resource* buffer = this->createBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, size);
    Log::Dbg("> created d3d12 default buffer %p at frame %d\n", buffer, frameIndex);

    // optionally upload data
    if (data) {
        this->upload(cmdList, frameIndex, buffer, data, size);
    }

    // transition buffer state to vertex buffer usage    
    this->transition(cmdList, buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER|D3D12_RESOURCE_STATE_INDEX_BUFFER);

    return buffer;
}

//------------------------------------------------------------------------------
void
d3d12ResourceAllocator::Free(uint64 frameIndex, ID3D12Resource* res) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->d3d12Device);
    o_assert_dbg(res);

    // place a new item in the free-queue, the actual release will happen
    // a few frames later inside the GarbageCollect() method
    // when the GPU is done with the buffer
    this->releaseQueue.Enqueue(frameIndex, res);
    Log::Dbg("> free d3d12 buffer %p at frame %d\n", res, frameIndex);
}

//------------------------------------------------------------------------------
void
d3d12ResourceAllocator::upload(ID3D12GraphicsCommandList* cmdList, uint64 frameIndex, ID3D12Resource* dstRes, const void* data, uint32 size) {
    o_assert_dbg(this->valid);
    o_assert_dbg(cmdList);
    o_assert_dbg(dstRes);
    o_assert_dbg(data && (size > 0));

    // create a temporary upload buffer and copy data into it
    ID3D12Resource* uploadBuffer = this->createBuffer(D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, size);
    Log::Dbg("> created d3d12 upload buffer %p at frame %d\n", uploadBuffer, frameIndex);
    void* dstPtr = nullptr;
    HRESULT hr = uploadBuffer->Map(0, nullptr, &dstPtr);
    o_assert(SUCCEEDED(hr));
    Memory::Copy(data, dstPtr, size);
    uploadBuffer->Unmap(0, nullptr);

    // add upload command to command list
    cmdList->CopyBufferRegion(dstRes, 0, uploadBuffer, 0, size);

    // add the upload buffer to the release-queue, so that it will be deleted when no longer in use
    this->Free(frameIndex, uploadBuffer);
}

} // namespace _priv
} // namespace Oryol
