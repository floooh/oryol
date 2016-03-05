//------------------------------------------------------------------------------
//  d3d12SamplerCache.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12_impl.h"
#include "d3d12SamplerCache.h"
#include "d3d12Types.h"
#include "Gfx/Core/renderer.h"
#include "fasthash.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12SamplerCache::d3d12SamplerCache() :
isValid(false) {
    this->cache.Reserve(NumSlots);
}

//------------------------------------------------------------------------------
d3d12SamplerCache::~d3d12SamplerCache() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12SamplerCache::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(this->cache.Empty());

    this->descAllocator = &ptrs.renderer->descAllocator;
    this->d3d12Device = ptrs.renderer->d3d12Device;
    this->samplerHeap = this->descAllocator->AllocHeap(
        d3d12DescAllocator::Sampler,
        NumSlots,
        GfxConfig::MaxNumTexturesPerStage,
        true);
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
d3d12SamplerCache::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->d3d12Device = nullptr;
    this->descAllocator = nullptr;
    this->samplerHeap.Invalidate();
    this->cache.Clear();
}

//------------------------------------------------------------------------------
bool
d3d12SamplerCache::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
int
d3d12SamplerCache::Lookup(const SamplerState* samplers, int numSamplers) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(samplers);
    o_assert_range_dbg(numSamplers, GfxConfig::MaxNumTexturesPerStage);

    // the resulting slotIndex, pointing to a new, or existing sampler heap location
    int slotIndex = InvalidIndex;

    // compute hash over sampler states
    const uint64 seed = 0xABADFEEDDEADBEEF;
    uint64 hash = fasthash64(samplers, numSamplers * sizeof(SamplerState), seed);
    int32 cacheIndex = this->cache.FindIndex(hash);
    if (InvalidIndex != cacheIndex) {
        // found an existing entry
        slotIndex = this->cache.ValueAtIndex(cacheIndex);

        // in debug mode, do a cross check in the hopefully unlikely case of a hash collision
        #if ORYOL_DEBUG
        const SamplerState* cachedSamplers = &this->dbgStore[slotIndex * NumSamplersPerSlot];
        for (int i = 0; i < numSamplers; i++) {
            o_assert2(samplers[i] == cachedSamplers[i], "hash collision in d3d12SamplerCache!");
        }
        #endif

        // all ok, reuse existing cache entry
        return slotIndex;
    }
    else {

        // a new entry, need to setup new d3d12 sampler in sampler heap
        slotIndex = this->descAllocator->AllocSlot(this->samplerHeap);
        this->cache.Add(hash, slotIndex);

        // in debug mode, store the original sampler state structs to check
        // for hash collisions later
        #if ORYOL_DEBUG
        SamplerState* cachedSamplers = &this->dbgStore[slotIndex * NumSamplersPerSlot];
        for (int i = 0; i < numSamplers; i++) {
            cachedSamplers[i] = samplers[i];
        }
        #endif

        // setup range of d3d12 samplers in sampler heap
        D3D12_SAMPLER_DESC d3d12SamplerDesc;
        const uint32 incrSize = this->descAllocator->DescriptorIncrementSize(this->samplerHeap);
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        this->descAllocator->CPUHandle(cpuHandle, this->samplerHeap, slotIndex);
        for (int i = 0; i < numSamplers; i++) {
            d3d12Types::initSamplerDesc(&d3d12SamplerDesc, samplers[i]);
            this->d3d12Device->CreateSampler(&d3d12SamplerDesc, cpuHandle);
            cpuHandle.ptr += incrSize;
        }
    }
    o_assert_dbg(InvalidIndex != slotIndex);
    return slotIndex;
}

} // namespace _priv
} // namespace Oryol
