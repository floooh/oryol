//------------------------------------------------------------------------------
//  StringBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "StringBuilder.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace Core {
    
//------------------------------------------------------------------------------
StringBuilder::StringBuilder() :
buffer(0),
capacity(0),
size(0) {
    // empty
}

//------------------------------------------------------------------------------
StringBuilder::~StringBuilder() {
    if (0 != this->buffer) {
        Memory::Free(this->buffer);
    }
    this->buffer = 0;
    this->capacity = 0;
    this->size = 0;
}

//------------------------------------------------------------------------------
void
StringBuilder::ensureRoom(int32 numBytes) {

    // room for the terminating 0-byte
    numBytes++;

    // check if we need to grow
    if ((this->size + numBytes) >= this->capacity) {
        // need to make room
        int32 growBy = (numBytes < minGrowSize) ? minGrowSize : numBytes;
        const int32 newCapacity = this->capacity + growBy;
        char* newBuffer = (char*) Memory::Alloc(newCapacity);
        if (this->buffer) {
            // copy over old content and free old buffer
            std::strcpy(newBuffer, this->buffer);
            Memory::Free(this->buffer);
            this->buffer = 0;
        }
        else {
            newBuffer[0] = 0;
        }
        this->buffer = newBuffer;
        this->capacity = newCapacity;
    }
}

//------------------------------------------------------------------------------
void
StringBuilder::Reserve(int32 numBytes) {
    this->ensureRoom(numBytes);
}

//------------------------------------------------------------------------------
int32
StringBuilder::Capacity() const {
    return this->capacity;
}

//------------------------------------------------------------------------------
int32
StringBuilder::Length() const {
    return this->size;
}

//------------------------------------------------------------------------------
void
StringBuilder::Clear() {
    if (0 != this->buffer) {
        this->buffer[0] = 0;
    }
    this->size = 0;
}

//------------------------------------------------------------------------------
String
StringBuilder::GetString() const {
    if ((this->buffer) && (this->size > 0)) {
        return String(this->buffer, this->size);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
String
StringBuilder::GetSubString(int32 startIndex, int32 endIndex) const {
    if (this->buffer) {
        if (0 == endIndex) {
            endIndex = this->size;
        }
        o_assert((startIndex >= 0) && (startIndex < endIndex));
        o_assert(endIndex <= this->size);
        return String(this->buffer + startIndex, endIndex);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(char c) {
    this->ensureRoom(1);
    this->buffer[this->size++] = c;
    this->buffer[this->size] = 0;
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(const char* ptr, int32 length) {
    if (length > 0) {
        o_assert(ptr);
        this->ensureRoom(length);
        std::strncpy(this->buffer + this->size, ptr, length);
        this->size += length;
        this->buffer[this->size] = 0;
    }
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(const char* ptr) {
    o_assert(ptr);
    this->Append(ptr, std::strlen(ptr));
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(const String& str) {
    this->Append(str.AsCStr(), str.Length());
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(const String& str, int32 startIndex, int32 endIndex) {
    const int32 strLen = str.Length();
    if (0 == endIndex) {
        endIndex = strLen;
    }
    o_assert((startIndex >= 0) && (startIndex < endIndex));
    o_assert(endIndex <= strLen);
    this->Append(str.AsCStr() + startIndex, endIndex - startIndex);
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(std::initializer_list<String> list) {
    for (const String& str : list) {
        this->Append(str);
    }
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(char delim, std::initializer_list<String> list) {
    for (const String& str : list) {
        this->Append(str);
        this->Append(delim);
    }
    // remove the last delimiter
    this->PopBack();
}

//------------------------------------------------------------------------------
void
StringBuilder::Truncate(int32 index) {
    o_assert((index >= 0) && (index < this->size));
    this->buffer[index] = 0;
    this->size = index;
}

//------------------------------------------------------------------------------
char
StringBuilder::PopBack() {
    o_assert(this->size > 0);
    char c = this->buffer[--this->size];
    this->buffer[this->size] = 0;
    return c;
}

} // namespace Core
} // namespace Oryol