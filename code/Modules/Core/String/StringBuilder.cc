//------------------------------------------------------------------------------
//  StringBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cstring>
#include <cstdio>
#include "StringBuilder.h"
#include "Core/Memory/Memory.h"

#if ORYOL_WINDOWS
#define o_strtok strtok_s
#else
#define o_strtok strtok_r
#endif

namespace Oryol {
    
//------------------------------------------------------------------------------
StringBuilder::StringBuilder() :
buffer(0),
capacity(0),
size(0) {
    // empty
}

//------------------------------------------------------------------------------
StringBuilder::StringBuilder(const char* str) :
StringBuilder() {
    this->Set(str);
}

//------------------------------------------------------------------------------
StringBuilder::StringBuilder(const char* str, int startIndex, int endIndex) :
StringBuilder() {
    this->Set(str, startIndex, endIndex);
}

//------------------------------------------------------------------------------
StringBuilder::StringBuilder(const String& str) :
StringBuilder() {
    this->Set(str);
}

//------------------------------------------------------------------------------
StringBuilder::StringBuilder(const String& str, int startIndex, int endIndex) :
StringBuilder() {
    this->Set(str, startIndex, endIndex);
}

//------------------------------------------------------------------------------
StringBuilder::StringBuilder(std::initializer_list<String> list) :
StringBuilder() {
    this->Set(list);
}

//------------------------------------------------------------------------------
StringBuilder::StringBuilder(char delim, std::initializer_list<String> list) :
StringBuilder() {
    this->Set(delim, list);
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
StringBuilder::ensureRoom(int numBytes) {

    // room for the terminating 0-byte
    numBytes++;

    // check if we need to grow
    if ((this->size + numBytes) >= this->capacity) {
        // need to make room
        int growBy = (numBytes < minGrowSize) ? minGrowSize : numBytes;
        const int newCapacity = this->capacity + growBy;
        char* newBuffer = (char*) Memory::Alloc(newCapacity);
        if (this->buffer) {
            // copy over old content and free old buffer
            #if ORYOL_WINDOWS
            errno_t res = strcpy_s(newBuffer, newCapacity, this->buffer);
            o_assert(0 == res);
            #else
            std::strcpy(newBuffer, this->buffer);
            #endif
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
StringBuilder::Reserve(int numBytes) {
    this->ensureRoom(numBytes);
}

//------------------------------------------------------------------------------
int
StringBuilder::Capacity() const {
    return this->capacity;
}

//------------------------------------------------------------------------------
int
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
        return String(this->buffer, 0, this->size);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
String
StringBuilder::GetSubString(int startIndex, int endIndex) const {
    if (this->buffer) {
        if (EndOfString == endIndex) {
            endIndex = this->size;
        }
        o_assert((startIndex >= 0) && (startIndex < endIndex));
        o_assert(endIndex <= this->size);
        return String(this->buffer, startIndex, endIndex);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
const char*
StringBuilder::AsCStr() const {
    if (this->buffer) {
        return this->buffer;
    }
    else {
        return "";
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
StringBuilder::Append(const char* ptr, int startIndex, int endIndex) {
    // ptr must be valid and not in this string builder's buffer
    o_assert(ptr && ((ptr < this->buffer) || (ptr >= (this->buffer+this->size))));
    if (EndOfString == endIndex) {
        endIndex = int(std::strlen(ptr));
    }
    o_assert(endIndex >= startIndex);
    const int length = endIndex - startIndex;
    if (length > 0) {
        this->ensureRoom(length);
        o_assert(this->buffer);
        std::strncpy(this->buffer + this->size, ptr + startIndex, length);
        this->size += length;
        this->buffer[this->size] = 0;
    }
}

//------------------------------------------------------------------------------
void
StringBuilder::Set(const char* ptr, int startIndex, int endIndex) {
    this->Clear();
    this->Append(ptr, startIndex, endIndex);
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(const char* ptr) {
    if (ptr) {
        this->Append(ptr, 0, int(std::strlen(ptr)));
    }
}

//------------------------------------------------------------------------------
void StringBuilder::Set(const char* ptr) {
    this->Clear();
    this->Append(ptr);
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(const String& str) {
    this->Append(str.AsCStr(), 0, str.Length());
}

//------------------------------------------------------------------------------
void
StringBuilder::Set(const String& str) {
    this->Clear();
    this->Append(str);
}

//------------------------------------------------------------------------------
void
StringBuilder::Append(const String& str, int startIndex, int endIndex) {
    const int strLen = str.Length();
    if (EndOfString == endIndex) {
        endIndex = strLen;
    }
    o_assert((startIndex >= 0) && (startIndex < endIndex));
    o_assert(endIndex <= strLen);
    this->Append(str.AsCStr(), startIndex, endIndex);
}

//------------------------------------------------------------------------------
void
StringBuilder::Set(const String& str, int startIndex, int endIndex) {
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
StringBuilder::Truncate(int index) {
    o_assert((index >= 0) && (index < this->size));
    this->buffer[index] = 0;
    this->size = index;
}

//------------------------------------------------------------------------------
char
StringBuilder::At(int index) const {
    o_assert((index >= 0) && (index < this->size));
    return this->buffer[index];
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
void
StringBuilder::substituteCommon(char* occur, int matchLen, int substLen, const char* subst) {
    o_assert((occur >= this->buffer) && (occur < (this->buffer+this->size)));
    const int diff = substLen - matchLen;
    if (diff > 0) {
        // preserve occur if buffer is reallocated by ensureRoom
        size_t offs = occur - this->buffer;
        this->ensureRoom(diff);
        occur = this->buffer + offs;
    }
    
    // move tail in or out
    const char* moveFrom = occur + matchLen;
    char* moveTo = occur + substLen;
    const int moveNumBytes = this->size - int(moveFrom - this->buffer);
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
}

//------------------------------------------------------------------------------
void
StringBuilder::SubstituteRange(int startIndex, int endIndex, const char* subst) {
    o_assert(this->buffer);
    o_assert(subst);
    o_assert(startIndex < this->size);
    if (EndOfString == endIndex) {
        endIndex = this->size;
    }
    o_assert(endIndex <= this->size);
    const int matchLen = endIndex - startIndex;
    const int substLen = int(std::strlen(subst));
    char* occur = this->buffer + startIndex;
    this->substituteCommon(occur, matchLen, substLen, subst);
}

//------------------------------------------------------------------------------
int
StringBuilder::SubstituteAll(const char* match, const char* subst) {
    o_assert(match && subst);
    o_assert(match[0] != 0);

    int numSubst = 0;
    if (nullptr != this->buffer) {
        char* ptr = this->buffer;
        const int matchLen = int(std::strlen(match));
        const int substLen = int(std::strlen(subst));
        while (nullptr != (ptr = std::strstr(ptr, match))) {
            this->substituteCommon(ptr, matchLen, substLen, subst);
            numSubst++;
        }
    }
    return numSubst;
}

//------------------------------------------------------------------------------
int
StringBuilder::SubstituteAll(const String& match, const String& subst) {
    return this->SubstituteAll(match.AsCStr(), subst.AsCStr());
}

//------------------------------------------------------------------------------
bool
StringBuilder::SubstituteFirst(const char* match, const char* subst) {
    o_assert(match && subst);
    o_assert(match[0] != 0);
    
    if (nullptr != this->buffer) {
        char* occur = std::strstr(this->buffer, match);
        if (nullptr != occur) {
            const int matchLen = int(std::strlen(match));
            const int substLen = int(std::strlen(subst));
            this->substituteCommon(occur, matchLen, substLen, subst);
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
int
StringBuilder::findFirstOf(const char* str, int strLen, int startIndex, int endIndex, const char* delims) {
    const char* ptr = str + startIndex;
    const int index = (const int)std::strcspn(ptr, delims) + startIndex;
    if (((endIndex != EndOfString) && (index >= endIndex)) || (index >= strLen)) {
        return InvalidIndex;
    }
    else {
        return index;
    }
}

//------------------------------------------------------------------------------
/**
 Find index of first occurrence of any characters in delim in an external string, between startIndex
 (including) and endIndex (excluding). If endIndex is EndOfString, search until end of string.
 Returns InvalidIndex if not found.
 */
int
StringBuilder::FindFirstOf(const char* str, int startIndex, int endIndex, const char* delims) {
    o_assert(0 != delims);
    o_assert(str);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    const int strLen = int(std::strlen(str));
    return findFirstOf(str, strLen, startIndex, endIndex, delims);
}

//------------------------------------------------------------------------------
/**
 Find index of first occurrence of any characters in delim, between startIndex
 (including) and endIndex (excluding). If endIndex is 0, search until end of string.
 Returns InvalidIndex if not found.
 */
int
StringBuilder::FindFirstOf(int startIndex, int endIndex, const char* delims) const {
    o_assert(0 != delims);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    o_assert(startIndex < this->size);
    if (nullptr != this->buffer) {
        return findFirstOf(this->buffer, this->size, startIndex, endIndex, delims);
    }
    else {
        // no content
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
int
StringBuilder::findLastOf(const char* str, int strLen, int startIndex, int endIndex, const char* delims) {
    const char* startPtr = str + startIndex;
    const char* ptr;
    if (EndOfString == endIndex) {
        ptr = str + strLen;
    }
    else {
        ptr = str + endIndex;
    }
    while (ptr > startPtr) {
        const char c = *--ptr;
        const char* delimPtr = delims;
        char sc;
        do {
            if ((sc = *delimPtr++) == c) {
                return int(ptr - startPtr);
            }
        }
        while (sc != 0);
    }
    // not found
    return InvalidIndex;
}

//------------------------------------------------------------------------------
int
StringBuilder::findLastNotOf(const char* str, int strLen, int startIndex, int endIndex, const char* delims) {
    const char* startPtr = str + startIndex;
    const char* ptr;
    if (EndOfString == endIndex) {
        ptr = str + strLen;
    }
    else {
        ptr = str + endIndex;
    }
    while (ptr > startPtr) {
        const char c = *--ptr;
        if (nullptr == strchr(delims, c)) {
            return int(ptr - startPtr);
        }
    }
    // not found
    return InvalidIndex;
}

//------------------------------------------------------------------------------
int
StringBuilder::FindLastOf(const char* str, int startIndex, int endIndex, const char* delims) {
    o_assert(0 != delims);
    o_assert(str);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    const int strLen = int(std::strlen(str));
    return findLastOf(str, strLen, startIndex, endIndex, delims);
}

//------------------------------------------------------------------------------
int
StringBuilder::FindLastOf(int startIndex, int endIndex, const char* delims) const {
    o_assert(0 != delims);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    o_assert(startIndex < this->size);
    if (nullptr != this->buffer) {
        return findLastOf(this->buffer, this->size, startIndex, endIndex, delims);
    }
    else {
        // no content
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
int
StringBuilder::FindLastNotOf(const char* str, int startIndex, int endIndex, const char* delims) {
    o_assert(0 != delims);
    o_assert(str);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    const int strLen = int(std::strlen(str));
    return findLastNotOf(str, strLen, startIndex, endIndex, delims);
}

//------------------------------------------------------------------------------
int
StringBuilder::FindLastNotOf(int startIndex, int endIndex, const char* delims) const {
    o_assert(0 != delims);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    o_assert(startIndex < this->size);
    if (nullptr != this->buffer) {
        return findLastNotOf(this->buffer, this->size, startIndex, endIndex, delims);
    }
    else {
        // no content
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
int
StringBuilder::findFirstNotOf(const char* str, int strLen, int startIndex, int endIndex, const char* delims) {
    const char* ptr = str + startIndex;
    int index = int(std::strspn(ptr, delims)) + startIndex;
    if (((EndOfString != endIndex) && (index >= endIndex)) || (index >= strLen)) {
        return InvalidIndex;
    }
    else {
        return index;
    }
}

//------------------------------------------------------------------------------
/**
 Find index of first occurrence of any characters NOT in delim, between startIndex
 (including) and endIndex (excluding). If endIndex is EndOfString, search until end of string.
 Returns InvalidIndex if not found.
 */
int
StringBuilder::FindFirstNotOf(const char* str, int startIndex, int endIndex, const char* delims) {
    o_assert(0 != delims);
    o_assert(str);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    const int strLen = int(std::strlen(str));
    return findFirstNotOf(str, strLen, startIndex, endIndex, delims);
}

//------------------------------------------------------------------------------
/**
 Find index of first occurrence of any characters NOT in delim, between startIndex
 (including) and endIndex (excluding). If endIndex is EndOfString, search until end of string.
 Returns InvalidIndex if not found.
 */
int
StringBuilder::FindFirstNotOf(int startIndex, int endIndex, const char* delims) const {
    o_assert(0 != delims);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    o_assert(startIndex < this->size);
    if (nullptr != this->buffer) {
        return findFirstNotOf(this->buffer, this->size, startIndex, endIndex, delims);
    }
    else {
        // no content
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
int
StringBuilder::findSubString(const char* str, int startIndex, int endIndex, const char* subStr) {
    const char* ptr = str + startIndex;
    const char* occur = std::strstr(ptr, subStr);
    if (nullptr == occur) {
        return InvalidIndex;
    }
    else {
        int index = int(occur - ptr) + startIndex;
        if ((EndOfString != endIndex) && (index >= endIndex)) {
            return InvalidIndex;
        }
        else {
            return index;
        }
    }
}

//------------------------------------------------------------------------------
/**
 Find first occurrence of subStr between startIndex (including) and endIndex
 (excluding). If endIndex is 0, search until end of string.
 Returns InvalidIndex if not found.
 */
int
StringBuilder::FindSubString(const char* str, int startIndex, int endIndex, const char* subStr) {
    o_assert(0 != subStr);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    return findSubString(str, startIndex, endIndex, subStr);
}
    
//------------------------------------------------------------------------------
/**
 Find first occurrence of subStr between startIndex (including) and endIndex
 (excluding). If endIndex is 0, search until end of string.
 Returns InvalidIndex if not found.
 */
int
StringBuilder::FindSubString(int startIndex, int endIndex, const char* subStr) const {
    o_assert(0 != subStr);
    o_assert((EndOfString == endIndex) || (endIndex >= startIndex));
    if (nullptr != this->buffer) {
        o_assert(startIndex < this->size);
        return findSubString(this->buffer, startIndex, endIndex, subStr);
    }
    else {
        // no content
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
bool
StringBuilder::Contains(const char* str) const {
    o_assert(str);
    return this->FindSubString(0, EndOfString, str) != InvalidIndex;
}

//------------------------------------------------------------------------------
bool
StringBuilder::Contains(const char* str, const char* subStr) {
    o_assert(str && subStr);
    return StringBuilder::FindSubString(str, 0, EndOfString, subStr) != InvalidIndex;
}

//------------------------------------------------------------------------------
/**
 NOTE: this method will destroy the content of the builder and clear it.
*/
int
StringBuilder::Tokenize(const char* delims, Array<String>& outTokens) {
    o_assert(nullptr != delims);
    
    outTokens.Clear();
    if (nullptr != this->buffer) {
        char* ptr = this->buffer;
        const char* token;
        char* contextPtr = 0;
        while (0 != (token = o_strtok(ptr, delims, &contextPtr))) {
            outTokens.Add(token);
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
int
StringBuilder::Tokenize(const char* delims, char fence, Array<String>& outTokens) {
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
                    outTokens.Add(ptr);
                    ptr = c;
                }
                else if (0 != (c = std::strpbrk(ptr, delims)))
                {
                    *c++ = 0;
                    outTokens.Add(ptr);
                    ptr = c;
                }
                else
                {
                    outTokens.Add(ptr);
                    break;
                }
            }
        }
    }
    this->Clear();
    return outTokens.Size();
}

//------------------------------------------------------------------------------
bool
StringBuilder::format(int maxLength, bool append, const char* fmt, va_list args) {
    if (!append) {
        this->Clear();
    }
    this->ensureRoom(maxLength);
#if ORYOL_ANDROID
    int res = vsnprintf(&(this->buffer[this->size]), maxLength, fmt, args);
#else
    int res = std::vsnprintf(&(this->buffer[this->size]), maxLength, fmt, args);
#endif
    if ((res < 0) || (res >= (maxLength-1))) {
        // error or string was truncated
        this->buffer[this->size] = 0;
        return false;
    }
    else {
        // all ok, need to adjust length
        this->size += res;
        return true;
    }
}

//------------------------------------------------------------------------------
bool
StringBuilder::Format(int maxLength, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool retval = this->format(maxLength, false, fmt, args);
    va_end(args);
    return retval;
}

//------------------------------------------------------------------------------
bool
StringBuilder::FormatVAList(int maxLength, const char* fmt, va_list args) {
    return this->format(maxLength, false, fmt, args);
}

//------------------------------------------------------------------------------
bool
StringBuilder::AppendFormat(int maxLength, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool retval = this->format(maxLength, true, fmt, args);
    va_end(args);
    return retval;
}

//------------------------------------------------------------------------------
bool
StringBuilder::AppendFormatVAList(int maxLength, const char* fmt, va_list args) {
    return this->format(maxLength, true, fmt, args);
}

} // namespace Oryol
