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
minGrow(ORYOL_STREAM_DEFAULT_MIN_GROW),
maxGrow(ORYOL_STREAM_DEFAULT_MAX_GROW),
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
    if (this->IsOpen()) {
        this->Close();
    }
    this->DiscardContent();
    this->minGrow = 0;
    this->maxGrow = 0;
    this->capacity = 0;
}

//------------------------------------------------------------------------------
bool
MemoryStream::hasRoom(int32 numBytes) const {
    return this->writePosition + numBytes <= this->capacity;
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
MemoryStream::makeRoom(int32 numBytes) {
    o_assert(numBytes > 0);
    o_assert((this->writePosition + numBytes) > this->capacity);

    int32 bytesNeeded = (this->writePosition + numBytes) - this->capacity;
    int32 growBy = this->capacity >> 1;
    if (growBy < this->minGrow) {
        growBy = this->minGrow;
    }
    if (growBy > this->maxGrow) {
        growBy = this->maxGrow;
    }
    if (growBy < bytesNeeded) {
        growBy = bytesNeeded;
    }
    int32 newCapacity = this->capacity + growBy;
    this->alloc(newCapacity);
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

//------------------------------------------------------------------------------
void
MemoryStream::DiscardContent() {
    o_assert(!this->isOpen);
    if (nullptr != this->buffer) {
        Memory::Free(this->buffer);
        this->buffer = 0;
    }
    this->size = 0;
    this->capacity = 0;
    this->writePosition = 0;
    this->readPosition = 0;
}

//------------------------------------------------------------------------------
void
MemoryStream::incrWritePosition(int32 numBytes) {
    this->writePosition += numBytes;
    if (this->writePosition > this->size) {
        this->size = this->writePosition;
    }
}

//------------------------------------------------------------------------------
int32
MemoryStream::Write(const void* ptr, int32 numBytes) {
    o_assert(this->isOpen);
    o_assert(this->IsWritable());
    o_assert((this->writePosition >= 0) && (this->writePosition <= this->size));
    
    if (numBytes > 0) {
        // need to make room?
        if (!this->hasRoom(numBytes)) {
            this->makeRoom(numBytes);
        }
        
        // write data to stream and update write cursor and size
        o_assert(0 != this->buffer);
        o_assert((this->writePosition + numBytes) <= this->capacity);
        Memory::Copy(ptr, this->buffer + this->writePosition, numBytes);
        this->incrWritePosition(numBytes);
    }
    return numBytes;
}

//------------------------------------------------------------------------------
uint8*
MemoryStream::MapWrite(int32 numBytes) {
    o_assert(this->isOpen);
    o_assert(!this->isWriteMapped);
    o_assert(this->IsWritable());
    o_assert((OpenMode::ReadWrite == this->openMode) || (OpenMode::WriteOnly == this->openMode) || (OpenMode::ReadWriteAppend == this->openMode));
    
    this->isWriteMapped = true;
    
    // need to make room?
    if  (!this->hasRoom(numBytes)) {
        this->makeRoom(numBytes);
    }
    
    uint8* ptr = this->buffer + this->writePosition;
    this->incrWritePosition(numBytes);
    return ptr;
}

//------------------------------------------------------------------------------
void
MemoryStream::incrReadPosition(int32 numBytes) {
    o_assert((this->readPosition + numBytes) <= this->size);
    this->readPosition += numBytes;
}

//------------------------------------------------------------------------------
int32
MemoryStream::Read(void* ptr, int32 numBytes) {
    o_assert(this->isOpen);
    o_assert(this->IsReadable());
    o_assert((this->readPosition >= 0) && (this->readPosition <= this->size));
    
    // cap numBytes if EndOfStream or trying to read past stream
    if ((EndOfStream == numBytes) || ((this->readPosition + numBytes) > this->size)) {
        numBytes = this->size - this->readPosition;
    }

    // read data from stream
    if (numBytes > 0) {
        o_assert(0 != this->buffer);
        Memory::Copy(this->buffer + this->readPosition, ptr, numBytes);
        this->incrReadPosition(numBytes);
    }
    return numBytes;
}

//------------------------------------------------------------------------------
/**
 See Stream::MapRead() for details!
*/
const uint8*
MemoryStream::MapRead(const uint8** outMaxValidPtr) {
    o_assert(this->isOpen);
    o_assert(!this->isReadMapped);
    o_assert(this->IsReadable());
    o_assert((this->readPosition >= 0) && (this->readPosition <= this->size));
    
    this->isReadMapped = true;

    // cap numBytes if trying to read past stream, or EndOfStream
    int32 numBytes = this->size - this->readPosition;
    o_assert(numBytes >= 0);
    if (0 == numBytes) {
        if (nullptr != outMaxValidPtr) {
            *outMaxValidPtr = nullptr;
        }
        return nullptr;
    }
    else {
        if (nullptr != outMaxValidPtr) {
            *outMaxValidPtr = this->buffer + this->size;
        }
        return this->buffer + this->readPosition;
    }
}

} // namespace IO
} // namespace Oryol