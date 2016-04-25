#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12SamplerCache
    @ingroup _priv
    @brief manage shared combinations of texture samplers

    One sampler heap can only contains 2048 entries, and there should be
    a relatively small number of different samplers per app. Samplers
    are bound in groups of up to 16 to shader stages. The d3d12SamplerCache
    class implements a cache for such sampler groups on a single sampler
    heap. Existing sampler groups will never be freed.
*/
#include "Gfx/Core/SamplerState.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/d3d12/d3d12DescAllocator.h"
#include "Gfx/d3d12/d3d12_decl.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace _priv {

class d3d12SamplerCache {
public:
    /// constructor
    d3d12SamplerCache();
    /// destructor
    ~d3d12SamplerCache();

    /// setup the cache
    void Setup(const gfxPointers& ptrs);
    /// discard the cache
    void Discard();
    /// return true if cache has been setup
    bool IsValid() const;

    /// create or lookup an entry in the cache
    int Lookup(const SamplerState* samplers, int numSamplers); 
    /// get CPU handle into sampler heap
    void CPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE& out, int item) const;
    /// get GPU handle into sampler heap
    void GPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE& out, int item) const;
    /// get d3d12 descriptor heap pointer
    ID3D12DescriptorHeap* DescriptorHeap() const;

private:
    static const int MaxNumSamplers = 2048;    // this is defined by d3d12
    static const int NumSamplersPerSlot = 16;
    static_assert(NumSamplersPerSlot >= GfxConfig::MaxNumShaderTextures, "Fix NumSamplersPerSlot");
    static const int NumSlots = MaxNumSamplers / NumSamplersPerSlot;
    static_assert(NumSamplersPerSlot * NumSlots == MaxNumSamplers, "MaxNumSamplers must be multiple of num textures per stage");

    bool isValid;
    ID3D12Device* d3d12Device;          // shortcut ptr, owned by d3d12Renderer
    d3d12DescAllocator* descAllocator;  // shortcut ptr, owned by d3d12Renderer
    Id samplerHeap;
    Map<uint64_t, int> cache;   // key: hash of sampler states, value: slot index in cache
    #if ORYOL_DEBUG
    StaticArray<SamplerState, MaxNumSamplers> dbgStore;
    #endif
};

//------------------------------------------------------------------------------
inline void
d3d12SamplerCache::CPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE& out, int item) const {
    return this->descAllocator->CPUHandle(out, this->samplerHeap, item);
}

//------------------------------------------------------------------------------
inline void
d3d12SamplerCache::GPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE& out, int item) const {
    return this->descAllocator->GPUHandle(out, this->samplerHeap, item);
}

//------------------------------------------------------------------------------
inline ID3D12DescriptorHeap*
d3d12SamplerCache::DescriptorHeap() const {
    return this->descAllocator->DescriptorHeap(this->samplerHeap);
}

} // namespace _priv
} // namespace Oryol