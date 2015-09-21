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
        ShaderResourceView,
        Sampler,

        NumTypes,
        InvalidType,
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
    /// garbage-collect released resources when safe, call once per frame
    void GarbageCollect(uint64 frameIndex);

    /// create descriptor heap of given type and number of slots and slot entries
    Id AllocHeap(Type type, int32 numSlots, int32 numDescriptorsPerSlot, bool allowAllocSlot);

    /// allocate a slot index from given heap
    int AllocSlot(const Id& heapId);
    /// deferred-release a slot-index from heap
    void ReleaseSlotDeferred(const Id& heapId, uint64 frameIndex, int slotIndex);

    /// get CPU handle into a descriptor heap
    void CPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE& out, const Id& heapId, int slotIndex) const;
    /// get GPU handle into a descriptor heap
    void GPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE& out, const Id& heapId, int slotIndex) const;

private:
    ID3D12Device* d3d12Device;
    bool valid;
    int32 numHeaps;

    static const int MaxNumHeaps = 32;
    struct heap {
        int32 numSlots = 0;
        int32 numDescriptorsPerSlot = 0;
        uint32 slotIncrSize = 0;
        Type type = InvalidType;
        ID3D12DescriptorHeap* d3d12DescHeap = nullptr;
        Queue<int> freeSlots;           // only if heap was allocated with allowAllocSlot=true
    };
    struct freeItem {
        freeItem() : frameIndex(0), slotIndex(InvalidIndex) {};
        freeItem(const Id& heapId_, uint64 frameIndex_, int slotIndex_) :
            heapId(heapId_), frameIndex(frameIndex_), slotIndex(slotIndex_) {
        };
        Id heapId;
        uint64 frameIndex;
        int slotIndex;
    };
    StaticArray<heap, MaxNumHeaps> heaps;
    Queue<freeItem> releaseQueue;
};

} // namespace _priv
} // namespace Oryol
