//------------------------------------------------------------------------------
//  d3d12ResAllocator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12ResAllocator.h"
#include "d3d12Types.h"
#include "d3d12_impl.h"
#include "d3d12Config.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12ResAllocator::d3d12ResAllocator() {
    // empty
}

//------------------------------------------------------------------------------
d3d12ResAllocator::~d3d12ResAllocator() {
    o_assert(this->releaseQueue.Empty());
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::DestroyAll() {    
    freeItem item;
    while (!this->releaseQueue.Empty()) {
        this->releaseQueue.Dequeue(item);
        ULONG count = item.res->Release();
        o_assert_dbg(0 == count);
        Log::Dbg("> released d3d12 resource %p at shutdown\n", item.res);
    }
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::GarbageCollect(uint64 frameIndex) {

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
            Log::Dbg("> released d3d12 resource %p at frame %d\n", item.res, frameIndex);
        }
    }
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResAllocator::createBuffer(ID3D12Device* d3d12Device, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, uint32 size) {

    D3D12_HEAP_PROPERTIES heapProps;
    d3d12Types::initHeapProps(&heapProps, heapType);
    D3D12_RESOURCE_DESC desc;
    d3d12Types::initBufferResourceDesc(&desc, size);

    ID3D12Resource* d3d12Resource = nullptr;
    HRESULT hr = d3d12Device->CreateCommittedResource(
        &heapProps,                     // pHeapProperties
        D3D12_HEAP_FLAG_NONE,           // HeapFlags
        &desc,                          // pResourceDesc
        initialState,                   // InitialResourceState
        nullptr,                        // pOptimizedClearValue
        __uuidof(ID3D12Resource),
        (void**) &d3d12Resource);
    o_assert(SUCCEEDED(hr) && d3d12Resource);
    
    return d3d12Resource;
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::Transition(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* res, D3D12_RESOURCE_STATES fromState, D3D12_RESOURCE_STATES toState) {
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
d3d12ResAllocator::AllocUploadBuffer(ID3D12Device* d3d12Device, uint32 size) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg(size > 0);

    ID3D12Resource* buffer = this->createBuffer(d3d12Device, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, size);
    Log::Dbg("> created d3d12 upload buffer at %p\n", buffer);
    return buffer;
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResAllocator::AllocDefaultBuffer(ID3D12Device* d3d12Device, uint32 size) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg(size > 0);

    ID3D12Resource* buffer = this->createBuffer(d3d12Device, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, size);
    Log::Dbg("> created d3d12 default buffer at %p\n", buffer);
    return buffer;
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResAllocator::AllocStaticBuffer(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* cmdList, uint64 frameIndex, const void* data, uint32 size) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg(cmdList);
    o_assert_dbg(size > 0);

    // create the default-heap buffer
    ID3D12Resource* buffer = this->AllocDefaultBuffer(d3d12Device, size);

    // optionally upload data
    if (data) {
        this->upload(d3d12Device, cmdList, frameIndex, buffer, data, size);
    }

    // transition buffer state to vertex buffer usage    
    this->Transition(cmdList, buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER|D3D12_RESOURCE_STATE_INDEX_BUFFER);

    return buffer;
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::ReleaseDeferred(uint64 frameIndex, ID3D12Object* res) {
    o_assert_dbg(res);

    // place a new item in the free-queue, the actual release will happen
    // a few frames later inside the GarbageCollect() method
    // when the GPU is done with the resource
    this->releaseQueue.Enqueue(frameIndex, res);
    Log::Dbg("> free d3d12 resource %p at frame %d\n", res, frameIndex);
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::upload(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* cmdList, uint64 frameIndex, ID3D12Resource* dstRes, const void* data, uint32 size) {
    o_assert_dbg(cmdList);
    o_assert_dbg(dstRes);
    o_assert_dbg(data && (size > 0));

    // create a temporary upload buffer and copy data into it
    ID3D12Resource* uploadBuffer = this->AllocUploadBuffer(d3d12Device, size);
    Log::Dbg("> created d3d12 upload buffer %p at frame %d\n", uploadBuffer, frameIndex);
    void* dstPtr = nullptr;
    HRESULT hr = uploadBuffer->Map(0, nullptr, &dstPtr);
    o_assert(SUCCEEDED(hr));
    Memory::Copy(data, dstPtr, size);
    uploadBuffer->Unmap(0, nullptr);

    // add upload command to command list
    cmdList->CopyBufferRegion(dstRes, 0, uploadBuffer, 0, size);

    // add the upload buffer to the release-queue, so that it will be deleted when no longer in use
    this->ReleaseDeferred(frameIndex, uploadBuffer);
}

} // namespace _priv
} // namespace Oryol
