//------------------------------------------------------------------------------
//  Stream.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Stream.h"

namespace Oryol {
namespace IO {

OryolClassImpl(Stream);

//------------------------------------------------------------------------------
Stream::Stream() :
openMode(OpenMode::Invalid),
isOpen(false),
size(0),
writePosition(0),
readPosition(0) {
    
    // empty
}

//------------------------------------------------------------------------------
Stream::~Stream() {
    o_assert(!this->isOpen);
}

//------------------------------------------------------------------------------
bool
Stream::Open(OpenMode::Enum mode) {
    
    o_assert(!this->isOpen);
    o_assert(this->openMode == OpenMode::Invalid);
    
    this->openMode = mode;
    this->isOpen = true;
    
    // you may want to override this in the subclass, see OpenMode!
    this->size = 0;
    this->writePosition = 0;
    this->readPosition = 0;
    
    return true;
}

//------------------------------------------------------------------------------
void
Stream::Close() {
    o_assert(this->isOpen);
    this->isOpen = false;
    this->openMode = OpenMode::Invalid;
}

//------------------------------------------------------------------------------
void
Stream::DiscardContent() {
    o_assert(!this->isOpen);
    // this is usually implemented in a subclass
}

//------------------------------------------------------------------------------
int32
Stream::Write(void* ptr, int32 numBytes) {
    // implement in subclass!
    return 0;
}

//------------------------------------------------------------------------------
void*
Stream::MapWrite(int32 numBytes) {
    // implement in subclass!
    return nullptr;
}

//------------------------------------------------------------------------------
void
Stream::UnmapWrite() {
    // implement in subclass!
}

//------------------------------------------------------------------------------
int32
Stream::Read(void* ptr, int32 numBytes) {
    // implement in subclass!
    return 0;
}

//------------------------------------------------------------------------------
void*
Stream::MapRead(int32 numBytes) {
    // implement in subclass!
    return nullptr;
}

//------------------------------------------------------------------------------
void
Stream::UnmapRead() {
    // implement in subclass!
}

} // namespace IO
} // namespace Oryol