//------------------------------------------------------------------------------
//  StreamBuffer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "StreamBuffer.h"

namespace Oryol {
namespace IO {
    
//------------------------------------------------------------------------------
StreamBuffer::StreamBuffer() :
mode(Mode::InvalidMode),
isOpen(false),
size(0),
writePosition(0),
readPosition(0) {
    
    // empty
}

//------------------------------------------------------------------------------
StreamBuffer::~StreamBuffer() {
    o_assert(!this->isOpen);
}

//------------------------------------------------------------------------------
bool
StreamBuffer::Open() {
    
    o_assert(!this->isOpen);
    o_assert(this->mode != Mode::InvalidMode);
    
    this->isOpen = true;
    this->writePosition = 0;
    this->readPosition = 0;
    
    return true;
}

//------------------------------------------------------------------------------
void
StreamBuffer::Close() {
    o_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
void
StreamBuffer::DiscardContent() {
    o_assert(!this->isOpen);
    // this is usually implemented in a subclass
}

//------------------------------------------------------------------------------
int32
StreamBuffer::Write(void* ptr, int32 numBytes) {
    // implement in subclass!
    return 0;
}

//------------------------------------------------------------------------------
void*
StreamBuffer::MapWrite(int32 numBytes) {
    // implement in subclass!
    return nullptr;
}

//------------------------------------------------------------------------------
void
StreamBuffer::UnmapWrite() {
    // implement in subclass!
}

//------------------------------------------------------------------------------
int32
StreamBuffer::Read(void* ptr, int32 numBytes) {
    // implement in subclass!
    return 0;
}

//------------------------------------------------------------------------------
void*
StreamBuffer::MapRead(int32 numBytes) {
    // implement in subclass!
    return nullptr;
}

//------------------------------------------------------------------------------
void
StreamBuffer::UnmapRead() {
    // implement in subclass!
}

} // namespace IO
} // namespace Oryol