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
/**
 Returns a pointer to the current read position, and optionally a pointer
 to the end of the readable area. It is save to read bytes until outMaxValidPtr.
 outMaxValidPtr can be nullptr if this information is not required
 (it is recommended though to always get the end of the readable area
 and perform checks against this when reading).
*/
const uint8*
Stream::MapRead(const uint8** outMaxValidPtr) {
    o_assert(!this->isReadMapped);
    this->isReadMapped = true;
    if (nullptr != outMaxValidPtr) {
        *outMaxValidPtr = nullptr;
    }
    return nullptr;
}

//------------------------------------------------------------------------------
void
Stream::UnmapRead() {
    o_assert(this->isReadMapped);
    this->isReadMapped = false;
    // override in subclass!
}

//------------------------------------------------------------------------------
bool
Stream::IsOpen() const {
    return this->isOpen;
}

//------------------------------------------------------------------------------
bool
Stream::IsReadable() const {
    return (OpenMode::ReadOnly == this->openMode) ||
           (OpenMode::ReadWrite == this->openMode) ||
           (OpenMode::ReadWriteAppend == this->openMode);
}
    
//------------------------------------------------------------------------------
bool
Stream::IsWritable() const {
    return (OpenMode::WriteOnly == this->openMode) ||
           (OpenMode::WriteAppend == this->openMode) ||
           (OpenMode::ReadWrite == this->openMode) ||
           (OpenMode::ReadWriteAppend == this->openMode);
}
    
//------------------------------------------------------------------------------
bool
Stream::IsWriteMapped() const {
    return this->isWriteMapped;
}
    
//------------------------------------------------------------------------------
bool
Stream::IsReadMapped() const {
    return this->isReadMapped;
}
    
//------------------------------------------------------------------------------
void
Stream::SetURL(const URL& url_) {
    o_assert(!this->isOpen);
    this->url = url_;
}
    
//------------------------------------------------------------------------------
const URL&
Stream::GetURL() const {
    return this->url;
}
    
//------------------------------------------------------------------------------
void
Stream::SetContentType(const ContentType& ct) {
    this->contentType = ct;
}
    
//------------------------------------------------------------------------------
const ContentType&
Stream::GetContentType() const {
    return this->contentType;
}
    
//------------------------------------------------------------------------------
OpenMode::Enum
Stream::GetOpenMode() const {
    return this->openMode;
}
    
//------------------------------------------------------------------------------
int32
Stream::Size() const {
    return this->size;
}
    
//------------------------------------------------------------------------------
void
Stream::SetWritePosition(int32 pos) {
    o_assert(this->isOpen);
    o_assert((pos >= 0) && (pos <= this->size));
    this->writePosition = pos;
}
    
//------------------------------------------------------------------------------
void
Stream::MoveWritePosition(int32 diff) {
    o_assert(this->isOpen);
    int32 newPos = this->writePosition + diff;
    o_assert((newPos >= 0) && (newPos <= this->size));
    this->writePosition = newPos;
}
    
//------------------------------------------------------------------------------
int32
Stream::GetWritePosition() const {
    return this->writePosition;
}
    
//------------------------------------------------------------------------------
void
Stream::SetReadPosition(int32 pos) {
    o_assert(this->isOpen);
    o_assert((pos >= 0) && (pos <= this->size));
    this->readPosition = pos;
}
    
//------------------------------------------------------------------------------
void
Stream::MoveReadPosition(int32 diff) {
    o_assert(this->isOpen);
    int32 newPos = this->readPosition + diff;
    o_assert((newPos >= 0) && (newPos <= this->size));
    this->readPosition = newPos;
}
    
//------------------------------------------------------------------------------
int32
Stream::GetReadPosition() const {
    return this->readPosition;
}
    
//------------------------------------------------------------------------------
bool
Stream::IsEndOfStream() const {
    return this->readPosition == this->size;
}

} // namespace IO
} // namespace Oryol