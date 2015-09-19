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
uniqueCounter(0),
valid(false) {
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

    this->d3d12Device = ptrs.displayMgr->d3d12Device;
    this->initHeap(RenderTargetView, d3d12Config::MaxNumRenderTargets, 1);
    this->initHeap(DepthStencilView, d3d12Config::MaxNumRenderTargets, 1);
    this->initHeap(TextureBlock, setup.PoolSize(GfxResourceType::TextureBlock), GfxConfig::MaxNumTexturesPerStage);
    this->initHeap(Sampler, d3d12Config::MaxNumSamplers, 1);

    this->valid = true;
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::Discard() {
    o_assert_dbg(this->valid);
    for (int i = 0; i < NumTypes; i++) {
        this->discardHeap(Type(i));
    }
    this->d3d12Device = nullptr;
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
d3d12DescAllocator::IsValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::GarbageCollect(uint64 frameIndex) {
    o_assert_dbg(this->valid);

    // release all deferred-released descriptors from longer 
    // then NumFrames befores, these are definitely no longer accessed by the GPU
    const uint64 safeNumFrames = d3d12Config::NumFrames + 1;
    if (frameIndex > safeNumFrames) {
        const uint64 minReleaseFrame = frameIndex - safeNumFrames;
        freeItem item;
        while (!this->releaseQueue.Empty() && (this->releaseQueue.Front().frameIndex < minReleaseFrame)) {
            this->releaseQueue.Dequeue(item);
            o_assert_dbg(item.id.IsValid());
            this->heaps[item.id.Type].freeSlots.Enqueue(item.id.SlotIndex);
        }
    }
}

//------------------------------------------------------------------------------
Id
d3d12DescAllocator::Allocate(Type t) {
    o_assert_dbg(this->valid);
    Log::Dbg("> allocate d3d12 descriptor %d\n", this->uniqueCounter+1);
    return Id(++this->uniqueCounter, this->heaps[t].freeSlots.Dequeue(), t);
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::ReleaseDeferred(uint64 frameIndex, const Id& id) {
    o_assert_dbg(this->valid);
    this->releaseQueue.Enqueue(frameIndex, id);
    Log::Dbg("> free d3d12 descriptor %d at frame %d\n", id.UniqueStamp, frameIndex);
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::CPUHandle(const Id& id, D3D12_CPU_DESCRIPTOR_HANDLE& out) const {
    const auto& heapEntry = this->heaps[id.Type];
    out = heapEntry.d3d12DescHeap->GetCPUDescriptorHandleForHeapStart();
    out.ptr += heapEntry.incrSize * id.SlotIndex;
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::GPUHandle(const Id& id, D3D12_GPU_DESCRIPTOR_HANDLE& out) const {
    const auto& heapEntry = this->heaps[id.Type];
    out = heapEntry.d3d12DescHeap->GetGPUDescriptorHandleForHeapStart();
    out.ptr = heapEntry.incrSize * id.SlotIndex;
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::initHeap(Type type, int32 numSlots, int32 descriptorsPerSlot) {
    o_assert_dbg(nullptr == this->heaps[type].d3d12DescHeap);
    o_assert_dbg(0 == this->heaps[type].numSlots);
    o_assert_dbg(this->heaps[type].freeSlots.Empty());
    o_assert_dbg(numSlots > 0);
    o_assert_dbg(descriptorsPerSlot > 0);
    o_assert_dbg(numSlots < (1<<16));

    auto& heapEntry = this->heaps[type];
    heapEntry.numSlots = numSlots;
    heapEntry.descriptorsPerSlot = descriptorsPerSlot;
    const int32 numHeapEntries = numSlots * descriptorsPerSlot;

    D3D12_DESCRIPTOR_HEAP_TYPE d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    switch (type) {
        case RenderTargetView:  d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; break;
        case DepthStencilView:  d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; break;
        case TextureBlock:      d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; break;
        case Sampler:           d3d12HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER; break;        
    }
    o_assert_dbg(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES != d3d12HeapType);

    D3D12_DESCRIPTOR_HEAP_DESC desc;
    d3d12Types::initDescriptorHeapDesc(&desc, numHeapEntries, d3d12HeapType, true);
    HRESULT hr = d3d12Device->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), (void**)&heapEntry.d3d12DescHeap);
    o_assert(SUCCEEDED(hr) && heapEntry.d3d12DescHeap);

    heapEntry.incrSize = descriptorsPerSlot * this->d3d12Device->GetDescriptorHandleIncrementSize(d3d12HeapType);

    heapEntry.freeSlots.Reserve(numSlots);
    for (uint16 slot = 0; slot < numSlots; slot++) {
        heapEntry.freeSlots.Enqueue(slot);
    }
}

//------------------------------------------------------------------------------
void
d3d12DescAllocator::discardHeap(Type type) {
    o_assert_dbg(this->heaps[type].d3d12DescHeap);
    ULONG refCount = this->heaps[type].d3d12DescHeap->Release();
    o_assert(0 == refCount);
    this->heaps[type] = heap();    
}

} // namespace _priv
} // namespace Oryol
