//------------------------------------------------------------------------------
//  d3d12DescAllocator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12DescAllocator.h"
#include "d3d12Config.h"
#include "d3d12_impl.h"
#include "d3d12Types.h"
#include "Gfx/Core/displayMgr.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12DescAllocator::d3d12DescAllocator() :
d3d12Device(nullptr),
valid(false),
numHeaps(0) {
    // empty
}

//------------------------------------------------------------------------------
d3d12DescAllocator::~d3d12DescAllocator() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::Setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert_dbg(!this->valid);
    o_assert_dbg(0 == this->numHeaps);
    this->d3d12Device = ptrs.displayMgr->d3d12Device;
    this->valid = true;
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::Discard() {
    o_assert_dbg(this->valid);

    // free all heaps
    for (int i = 0; i < this->numHeaps; i++) {
        if (this->heaps[i].d3d12DescHeap) {
            this->heaps[i].d3d12DescHeap->Release();
        }
        this->heaps[i] = heap();
    }
    this->d3d12Device = nullptr;
    this->numHeaps = 0;
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
d3d12DescAllocator::IsValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
Id
d3d12DescAllocator::AllocHeap(Type type, int32 numSlots, int32 numDescriptorsPerSlot, bool allowAllocSlot) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->numHeaps < MaxNumHeaps);
    o_assert_dbg(numSlots > 0);
    o_assert_dbg(numDescriptorsPerSlot > 0);

    const Id heapId(0, this->numHeaps, type);
    heap& entry = this->heaps[this->numHeaps++];
    entry.numSlots = numSlots;
    entry.numDescriptorsPerSlot = numDescriptorsPerSlot;
    entry.type = type;

    D3D12_DESCRIPTOR_HEAP_TYPE d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    bool shaderVisible = false;
    switch (type) {
        case RenderTargetView:
            d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            shaderVisible = false;
            break;
        case DepthStencilView:
            d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            shaderVisible = false;
            break;
        case ShaderResourceView:
            d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            shaderVisible = true;
            break;
        case Sampler:
            d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            shaderVisible = true;
            break;
    }
    o_assert_dbg(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES != d3d12HeapType);

    const int numDescriptors = numSlots * numDescriptorsPerSlot;
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    d3d12Types::initDescriptorHeapDesc(&desc, numDescriptors, d3d12HeapType, shaderVisible);
    HRESULT hr = d3d12Device->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), (void**)&entry.d3d12DescHeap);
    o_assert(SUCCEEDED(hr) && entry.d3d12DescHeap);
    entry.descIncrSize = this->d3d12Device->GetDescriptorHandleIncrementSize(d3d12HeapType);
    entry.slotIncrSize = numDescriptorsPerSlot * entry.descIncrSize;

    if (allowAllocSlot) {
        entry.freeSlots.Reserve(numSlots);
        for (int i = 0; i < numSlots; i++) {
            entry.freeSlots.Enqueue(i);
        }
    }

    o_dbg("> allocated descriptor heap '%d' with '%d' slots\n", heapId.SlotIndex, numSlots);
    return heapId;
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::CPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE& out, const Id& heapId, int slotIndex) const {
    const auto& entry = this->heaps[heapId.SlotIndex];
    o_assert_dbg(entry.type == heapId.Type);
    o_assert_dbg(entry.d3d12DescHeap);
    out = entry.d3d12DescHeap->GetCPUDescriptorHandleForHeapStart();
    out.ptr += (entry.slotIncrSize * slotIndex);
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::GPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE& out, const Id& heapId, int slotIndex) const {
    const auto& entry = this->heaps[heapId.SlotIndex];
    o_assert_dbg(entry.type == heapId.Type);
    o_assert_dbg(entry.d3d12DescHeap);
    out = entry.d3d12DescHeap->GetGPUDescriptorHandleForHeapStart();
    out.ptr += (entry.slotIncrSize * slotIndex);
}

//------------------------------------------------------------------------------
uint32
d3d12DescAllocator::DescriptorIncrementSize(const Id& heapId) const {
    return this->heaps[heapId.SlotIndex].descIncrSize;
}

//------------------------------------------------------------------------------
ID3D12DescriptorHeap*
d3d12DescAllocator::DescriptorHeap(const Id& heapId) const {
    return this->heaps[heapId.SlotIndex].d3d12DescHeap;
}

//------------------------------------------------------------------------------
int
d3d12DescAllocator::AllocSlot(const Id& heapId) {
    return this->heaps[heapId.SlotIndex].freeSlots.Dequeue();
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::ReleaseSlotDeferred(const Id& heapId, uint64 frameIndex, int slotIndex) {
    this->releaseQueue.Enqueue(heapId, frameIndex, slotIndex);
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::GarbageCollect(uint64 frameIndex) {

    const uint64 safeNumFrames = d3d12Config::NumFrames + 1;
    if (frameIndex > safeNumFrames) {
        const uint64 minReleaseFrame = frameIndex - safeNumFrames;
        freeItem item;
        while (!this->releaseQueue.Empty() && (this->releaseQueue.Front().frameIndex < minReleaseFrame)) {
            this->releaseQueue.Dequeue(item);
            o_assert_dbg(InvalidIndex != item.slotIndex);
            this->heaps[item.heapId.SlotIndex].freeSlots.Enqueue(item.slotIndex);
            o_dbg("> released descriptor slot on heap '%d' slot '%d'\n", item.heapId, item.slotIndex);
        }
    }
}


} // namespace _priv
} // namespace Oryol
