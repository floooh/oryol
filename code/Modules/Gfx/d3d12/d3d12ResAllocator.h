#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12ResAllocator
    @ingroup _priv
    @brief D3D12 resource allocator wrapper

    Takes care of allocating buffer resources and uploading data to them.
    Currently the implementation is very simple, every resource
    will be allocated into its own resource heap, using its own 
    upload heap. The upload heap will be deallocated when safe.

    A proper implementation should try to place resources into
    bigger heaps (ideally one heap per resource label).
*/
#include "Core/Types.h"
#include "Core/Containers/Queue.h"
#include "Gfx/d3d12/d3d12_decl.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

class d3d12ResAllocator {
public:
    /// constructor
    d3d12ResAllocator();
    /// destructor
    ~d3d12ResAllocator();

    /// destroy all left-over resources (called at shutdown)
    void DestroyAll();
    /// garbage-collect released resources when safe, call once per frame
    void GarbageCollect(uint64 frameIndex);

    /// create a d3d12 buffer with implicit default heap
    ID3D12Resource* AllocDefaultBuffer(ID3D12Device* d3d12Device, uint32 size);
    /// create a d3d12 buffer with implicit upload heap
    ID3D12Resource* AllocUploadBuffer(ID3D12Device* d3d12Device, uint32 size);
    /// allocate a d3d12 buffer resource, optionally fill with data
    ID3D12Resource* AllocStaticBuffer(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* cmdList, uint64 frameIndex, const void* data, uint32 size);
    /// allocate a render target resource (e.g. for empty textures/render targets)
    ID3D12Resource* AllocRenderTarget(ID3D12Device* d3d12Device, int width, int height, PixelFormat::Code fmt, int smpCount);

    /// defer-free a resource (accepts any D3D12 object that needs to be deferred-deleted actually)
    void ReleaseDeferred(uint64 frameIndex, ID3D12Object* res);

    /// place a state-transition resource-barrier command
    void Transition(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* res, D3D12_RESOURCE_STATES fromState, D3D12_RESOURCE_STATES toState);

private:
    /// internal helper method to create a d3d12 buffer resource
    ID3D12Resource* createBuffer(ID3D12Device* d3d12Device, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, uint32 size);
    /// upload method, expects that the buffer is already in COPY_DEST state
    void upload(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* cmdList, uint64 frameIndex, ID3D12Resource* dstRes, const void* data, uint32 size);

    struct freeItem {
        freeItem() : frameIndex(0), res(nullptr) { };
        freeItem(uint64 frameIndex_, ID3D12Object* res_) : frameIndex(frameIndex_), res(res_) { };
        uint64 frameIndex;
        ID3D12Object* res;
    };
    Queue<freeItem> releaseQueue;
};

} // namespace _priv
} // namespace Oryol
