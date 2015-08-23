#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12ResourceAllocator
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

class d3d12ResourceAllocator {
public:
    /// constructor
    d3d12ResourceAllocator();
    /// destructor
    ~d3d12ResourceAllocator();

    /// setup the heap allocator
    void Setup(ID3D12Device* device);
    /// discard the heap allocator (call after GPU is finished)
    void Discard();
    /// return true if has been setup
    bool IsValid() const;
    /// garbage-collect released resources when safe, call once per frame
    void GarbageCollect(uint64 frameIndex);

    /// allocate a d3d12 buffer resource for use and vertex-, index- or constant-buffer optionally fill with data
    ID3D12Resource* AllocBuffer(ID3D12GraphicsCommandList* cmdList, uint64 frameIndex, const void* data, uint32 size);
    /// defer-free all data associated with resource label
    void Free(uint64 frameIndex, ID3D12Resource* res);

private:
    /// internal helper method to create a d3d12 buffer resource
    ID3D12Resource* createBuffer(D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, uint32 size);
    /// internal data upload method, expects that the buffer is already in COPY_DEST state
    void upload(ID3D12GraphicsCommandList* cmdList, uint64 frameIndex, ID3D12Resource* dstRes, const void* data, uint32 size);
    /// place a state-transition resource-barrier command
    void transition(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* res, D3D12_RESOURCE_STATES fromState, D3D12_RESOURCE_STATES toState);

    struct freeItem {
        freeItem() : frameIndex(0), res(nullptr) { };
        freeItem(uint64 frameIndex_, ID3D12Resource* res_) : frameIndex(frameIndex_), res(res_) { };
        uint64 frameIndex;
        ID3D12Resource* res;
    };
    ID3D12Device* d3d12Device;
    Queue<freeItem> releaseQueue;
    bool valid;
};

} // namespace _priv
} // namespace Oryol
