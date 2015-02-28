#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceStreamTarget
    @brief array of pointer/size pairs for streaming resource creation
    @ingroup Resource
    
    ResourceStreamTarget objects contain pointer/size pairs where
    data is streamed to for streaming resource creation. For instance,
    for a texture, the pointers would point to mipmap surfaces, and
    for a mesh, the pointers would point to vertex and index buffers.
*/
#include "Core/Containers/StaticArray.h"
#include "Resource/ResourceState.h"

namespace Oryol {

class ResourceStreamTarget {
public:
    static const int32 MaxNumEntries = 16;

    /// resource state set by factory (Valid or Failed)
    ResourceState::Code State = ResourceState::InvalidState;

    /// add a pointer/size pair
    void Add(void* ptr, int32 size);
    /// get number of entries
    int32 Size() const;
    /// get a pointer/size pair
    void* Get(int32 index, int32& outSize) const;

private:
    int32 numEntries = 0;
    StaticArray<void*, MaxNumEntries> ptrs;
    StaticArray<int32, MaxNumEntries> sizes;
};

} // namespace Oryol