//------------------------------------------------------------------------------
//  MemoryStream.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MemoryStream.h"

namespace Oryol {
namespace IO {

OryolClassImpl(MemoryStream);

using namespace Core;

//------------------------------------------------------------------------------
MemoryStream::MemoryStream() :
minGrow(ORYOL_CONTAINER_DEFAULT_MIN_GROW),
maxGrow(ORYOL_CONTAINER_DEFAULT_MAX_GROW),
capacity(0),
buffer(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
MemoryStream::MemoryStream(int32 capacity_, int32 minGrow_, int32 maxGrow_) :
minGrow(minGrow_),
maxGrow(maxGrow_),
capacity(0),
buffer(0) {
    this->alloc(capacity_);
}

//------------------------------------------------------------------------------
MemoryStream::~MemoryStream() {
    if (0 != this->buffer) {
        Memory::Free(this->buffer);
        this->buffer = 0;
    }
    this->minGrow = 0;
    this->maxGrow = 0;
    this->capacity = 0;
}

//------------------------------------------------------------------------------
void
MemoryStream::alloc(int32 newCapacity) {
    o_assert(newCapacity > 0);
    if (this->capacity == newCapacity) {
        return;
    }
    o_assert(this->size <= newCapacity);
    
    // allocate new buffer
    const int32 newBufSize = newCapacity;
    uchar* newBuffer = (uchar*) Memory::Alloc(newBufSize);
    
    // need to move content?
    if (this->size > 0) {
        o_assert(this->buffer);
        Memory::Copy(this->buffer, newBuffer, this->size);
    }
    
    // need to free old buffer?
    if (this->buffer) {
        Memory::Free(this->buffer);
        this->buffer = nullptr;
    }
    
    // write new values
    this->buffer = newBuffer;
    this->capacity = newCapacity;
}

//------------------------------------------------------------------------------
void
MemoryStream::SetAllocStrategy(int32 minGrow_, int32 maxGrow_) {
    this->minGrow = minGrow_;
    this->maxGrow = maxGrow_;
}

//------------------------------------------------------------------------------
int32
MemoryStream::GetMinGrow() const {
    return this->minGrow;
}

//------------------------------------------------------------------------------
int32
MemoryStream::GetMaxGrow() const {
    return this->maxGrow;
}

//------------------------------------------------------------------------------
int32
MemoryStream::Capacity() const {
    return this->capacity;
}

    
} // namespace IO
} // namespace Oryol