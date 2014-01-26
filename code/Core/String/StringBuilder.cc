//------------------------------------------------------------------------------
//  StringBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "StringBuilder.h"
#include "Core/Memory/Memory.h"

#if ORYOL_WINDOWS
#define o_strtok strtok_s
#else
#define o_strtok strtok_r
#endif

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
StringBuilder::Set(const char* ptr, int32 length) {
    this->Clear();
    this->Append(ptr, length);
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(const char* ptr) {
    o_assert(ptr);
    this->Append(ptr, std::strlen(ptr));
}

//------------------------------------------------------------------------------
void StringBuilder::Set(const char* ptr) {
    this->Clear();
    this->Append(ptr);
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(const String& str) {
    this->Append(str.AsCStr(), str.Length());
}

//------------------------------------------------------------------------------
void
StringBuilder::Set(const String& str) {
    this->Clear();
    this->Append(str);
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
StringBuilder::Set(const String& str, int32 startIndex, int32 endIndex) {
    this->Clear();
    this->Append(str, startIndex, endIndex);
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
StringBuilder::Set(std::initializer_list<String> list) {
    this->Clear();
    this->Append(list);
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
StringBuilder::Set(char delim, std::initializer_list<String> list) {
    this->Clear();
    this->Append(delim, list);
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
StringBuilder::Back() const {
    o_assert(this->size > 0);
    return this->buffer[this->size - 1];
}

//------------------------------------------------------------------------------
char
StringBuilder::PopBack() {
    o_assert(this->size > 0);
    char c = this->buffer[--this->size];
    this->buffer[this->size] = 0;
    return c;
}

//------------------------------------------------------------------------------
bool
StringBuilder::SubstituteFirst(const char* match, const char* subst) {
    o_assert(match && subst);
    o_assert(match[0] != 0);
    
    if (nullptr != this->buffer) {
        // find the first occurance of 'match'
        char* occur = std::strstr(this->buffer, match);
        if (nullptr != occur) {
            // we have a match, make sure we have enough room
            const int32 matchLen = std::strlen(match);
            const int32 substLen = std::strlen(subst);
            const int32 diff = substLen - matchLen;
            if (diff > 0) {
                this->ensureRoom(diff);
            }
            
            // move tail in or out
            const char* moveFrom = occur + matchLen;
            char* moveTo = occur + substLen;
            const int32 moveNumBytes = this->size - (moveFrom - this->buffer);
            if ((moveFrom != moveTo) && (moveNumBytes > 0)) {
                std::memmove(moveTo, moveFrom, moveNumBytes);
            }
            
            // properly zero-terminate and fix size
            moveTo[moveNumBytes] = 0;
            this->size += diff;
            
            // copy substitute into hole
            if (substLen > 0) {
                std::strncpy(occur, subst, substLen);
            }
            
            return true;
        }
        else {
            // no match
            return false;
        }
    }
    else {
        // string buffer is empty
        return false;
    }
}

//------------------------------------------------------------------------------
bool
StringBuilder::SubstituteFirst(const String& match, const String& subst) {
    return this->SubstituteFirst(match.AsCStr(), subst.AsCStr());
}

//------------------------------------------------------------------------------
/**
 Find index of first occurance of any chacters in delim, between startIndex
 (including) and endIndex (excluding). If endIndex is 0, search until end of string.
 Returns InvalidIndex if not found.
 */
int32
StringBuilder::FindFirstOf(int32 startIndex, int32 endIndex, const char* delims) const {
    o_assert(0 != delims);
    o_assert((endIndex == 0) || (endIndex >= startIndex));
    o_assert(startIndex < this->size);
    if (nullptr != this->buffer) {
        const char* ptr = this->buffer + startIndex;
        int index = std::strcspn(ptr, delims);
        if (((endIndex != 0) && (index >= endIndex)) || (index >= this->size)) {
            return InvalidIndex;
        }
        else {
            return startIndex + index;
        }
    }
    else {
        // no content
        return InvalidIndex;
    }
}
    
//------------------------------------------------------------------------------
/**
 Find index of first occurange of any characters NOT in delim, between startIndex
 (including) and endIndex (exluding). If endIndex is 0, search until end of string.
 Returns InvalidIndex if not found.
 */
int32
StringBuilder::FindFirstNotOf(int32 startIndex, int32 endIndex, const char* delims) const {
    o_assert(0 != delims);
    o_assert((endIndex == 0) || (endIndex >= startIndex));
    o_assert(startIndex < this->size);
    if (nullptr != this->buffer) {
        const char* ptr = this->buffer + startIndex;
        int index = std::strspn(ptr, delims);
        if (((endIndex != 0) && (index >= endIndex)) || (index >= this->size)) {
            return InvalidIndex;
        }
        else {
            return startIndex + index;
        }
    }
    else {
        // no content
        return InvalidIndex;
    }
}
    
//------------------------------------------------------------------------------
/**
 Find first occurance of subStr between startIndex (including) and endIndex
 (excluding). If endIndex is 0, search until end of string.
 Returns InvalidIndex if not found.
 */
int32
StringBuilder::FindSubString(int32 startIndex, int32 endIndex, const char* subStr) const {
    o_assert(0 != subStr);
    o_assert(startIndex < this->size);
    o_assert((endIndex == 0) || (endIndex >= startIndex));
    if (nullptr != this->buffer) {
        const char* ptr = this->buffer + startIndex;
        const char* occur = std::strstr(ptr, subStr);
        if (nullptr == occur) {
            return InvalidIndex;
        }
        else {
            int32 index = occur - ptr;
            if ((endIndex != 0) && (index >= endIndex)) {
                return InvalidIndex;
            }
            else {
                return startIndex + index;
            }
        }
    }
    else {
        // no content
        return InvalidIndex;
    }
}
    
//------------------------------------------------------------------------------
/**
 NOTE: this method will destroy the content of the builder and clear it.
*/
int32
StringBuilder::Tokenize(const char* delims, Array<String>& outTokens) {
    o_assert(nullptr != delims);
    
    outTokens.Clear();
    if (nullptr != this->buffer) {
        char* ptr = this->buffer;
        const char* token;
        char* contextPtr = 0;
        while (0 != (token = o_strtok(ptr, delims, &contextPtr))) {
            outTokens.AddBack(token);
            ptr = 0;
        }
    }
    this->Clear();
    return outTokens.Size();
}
    
//------------------------------------------------------------------------------
/**
 NOTE: this method will destroy the content of the builder and clear it.
 */
int32
StringBuilder::Tokenize(const char* delims, char fence, Array<String>& outTokens)
{
    outTokens.Clear();
    if (nullptr != this->buffer) {
        char* ptr = this->buffer;
        char* end = ptr + this->size;
        while (ptr < end)
        {
            char* c;
            
            // skip white space
            while (*ptr && std::strchr(delims, *ptr))
            {
                ptr++;
            }
            if (*ptr)
            {
                // check for fenced area
                if ((fence == *ptr) && (0 != (c = std::strchr(++ptr, fence))))
                {
                    *c++ = 0;
                    outTokens.AddBack(ptr);
                    ptr = c;
                }
                else if (0 != (c = std::strpbrk(ptr, delims)))
                {
                    *c++ = 0;
                    outTokens.AddBack(ptr);
                    ptr = c;
                }
                else
                {
                    outTokens.AddBack(ptr);
                    break;
                }
            }
        }
    }
    this->Clear();
    return outTokens.Size();
}

} // namespace Core
} // namespace Oryol