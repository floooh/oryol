#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12DescAllocator
    @ingroup _priv
    @brief wraps allocations from descriptor heaps
*/
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/gfxPointers.h"
#include "Resource/Id.h"
#include "Core/Containers/Queue.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/d3d12/d3d12_decl.h"

namespace Oryol {
namespace _priv {

class d3d12DescAllocator {
public:
    /// descriptor resource types
    enum Type : Id::TypeT {
        RenderTargetView = 0,
        DepthStencilView,
        TextureBlock,
        Sampler,

        NumTypes,
    };

    /// constructor
    d3d12DescAllocator();
    /// destructor
    ~d3d12DescAllocator();

    /// setup the allocator
    void Setup(const GfxSetup& setup, const gfxPointers& ptrs);
    /// discard the allocator (must be called at shutdown after GPU is done)
    void Discard();
    /// return true if the allocator has been setup
    bool IsValid() const;
    /// garbage-collect released descriptors when safe, call once per frame
    void GarbageCollect(uint64 frameIndex);

    /// allocate a descriptor resource
    Id Allocate(Type t);
    /// deferred-free a descriptor resource
    void ReleaseDeferred(uint64 frameIndex, const Id& id);

    /// get CPU handle into a descriptor heap
    void CPUHandle(const Id& id, int subDescIndex, D3D12_CPU_DESCRIPTOR_HANDLE& out) const;
    /// get GPU handle into a descriptor heap
    void GPUHandle(const Id& id, int subDescIndex, D3D12_GPU_DESCRIPTOR_HANDLE& out) const;

private:
    /// initialize heap of type with number of slots
    void initHeap(Type type, int32 numSlots, int32 descriptorsPerSlots);
    /// discard a heap
    void discardHeap(Type type);

    ID3D12Device* d3d12Device;
    uint32 uniqueCounter;
    bool valid;
    
    struct heap {
        int32 numSlots = 0;
        int32 descriptorsPerSlot = 0;
        uint32 descIncrSize = 0;
        uint32 slotIncrSize = 0;
        ID3D12DescriptorHeap* d3d12DescHeap = nullptr;
        Queue<uint16> freeSlots;
    };
    StaticArray<heap, NumTypes> heaps;

    struct freeItem {
        freeItem() : frameIndex(0) { };
        freeItem(uint64 frameIndex_, const Id& id_) : frameIndex(frameIndex_), id(id_) { };
        uint64 frameIndex;
        Id id;
    };
    Queue<freeItem> releaseQueue;
};

} // namespace _priv
} // namespace Oryol
