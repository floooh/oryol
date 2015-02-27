//------------------------------------------------------------------------------
//  ResourceStreamTarget.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "ResourceStreamTarget.h"

namespace Oryol {

//------------------------------------------------------------------------------
void
ResourceStreamTarget::Add(void* ptr, int32 size) {
    o_assert_dbg(this->numEntries < MaxNumEntries);
    this->ptrs[this->numEntries] = ptr;
    this->sizes[this->numEntries] = size;
    this->numEntries++;
}

//------------------------------------------------------------------------------
int32
ResourceStreamTarget::Size() const {
    return this->numEntries;
}

//------------------------------------------------------------------------------
void*
ResourceStreamTarget::Get(int32 index, int32& outSize) const {
    o_assert_range_dbg(index, MaxNumEntries);
    outSize = this->sizes[index];
    return this->ptrs[index];
}

} // namespace Oryol