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
openMode(OpenMode::InvalidOpenMode),
isOpen(false),
isWriteMapped(false),
isReadMapped(false),
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
    o_assert(this->openMode == OpenMode::InvalidOpenMode);
    o_assert(mode != OpenMode::InvalidOpenMode);
    
    this->openMode = mode;
    this->isOpen = true;
    
    switch (this->openMode) {
        case OpenMode::ReadOnly:
            this->readPosition = 0;
            break;
            
        case OpenMode::WriteOnly:
            this->writePosition = 0;
            this->size = 0;
            break;
            
        case OpenMode::WriteAppend:
            this->writePosition = this->size;
            break;
            
        case OpenMode::ReadWrite:
            this->readPosition = 0;
            this->writePosition = 0;
            this->size = 0;
            break;
            
        case OpenMode::ReadWriteAppend:
            this->readPosition = 0;
            this->writePosition = this->size;
            break;
            
        // silence compiler
        case OpenMode::InvalidOpenMode:
            break;
    }
    return true;
}

//------------------------------------------------------------------------------
void
Stream::Close() {
    o_assert(this->isOpen);
    if (this->IsWriteMapped()) {
        this->UnmapWrite();
    }
    if (this->IsReadMapped()) {
        this->UnmapRead();
    }
    this->isOpen = false;
    this->openMode = OpenMode::InvalidOpenMode;
}

//------------------------------------------------------------------------------
void
Stream::DiscardContent() {
    o_assert(!this->isOpen);
    // this is usually implemented in a subclass
}

//------------------------------------------------------------------------------
int32
Stream::Write(const void* ptr, int32 numBytes) {
    // implement in subclass!
    return 0;
}

//------------------------------------------------------------------------------
uint8*
Stream::MapWrite(int32 numBytes) {
    o_assert(!this->isWriteMapped);
    this->isWriteMapped = true;
    // override in subclass!
    return nullptr;
}

//------------------------------------------------------------------------------
void
Stream::UnmapWrite() {
    o_assert(this->isWriteMapped);
    this->isWriteMapped = false;
    // override in subclass!
}

//------------------------------------------------------------------------------
int32
Stream::Read(void* ptr, int32 numBytes) {
    // implement in subclass!
    return 0;
}

//------------------------------------------------------------------------------
const uint8*
Stream::MapRead(const uint8*& outMaxValidPtr) {
    o_assert(!this->isReadMapped);
    this->isReadMapped = true;
    // override in subclass!
    outMaxValidPtr = nullptr;
    return nullptr;
}

//------------------------------------------------------------------------------
void
Stream::UnmapRead() {
    o_assert(this->isReadMapped);
    this->isReadMapped = false;
    // override in subclass!
}

} // namespace IO
} // namespace Oryol