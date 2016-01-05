#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Buffer
    @ingroup Core
    @brief growable memory buffer for raw data
*/
#include "Core/Types.h"
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"

namespace Oryol {

class Buffer {
public:
    /// default constructor
    Buffer();
    /// move constructor
    Buffer(Buffer&& rhs);
    /// destructor
    ~Buffer();

    /// always force move-construct
    Buffer(const Buffer& rhs) = delete;
    /// always force move-assign
    void operator=(const Buffer& rhs) = delete;

    /// move-assignment
    void operator=(Buffer&& rhs);

    /// get number of bytes in buffer
    int32 Size() const;
    /// return true if empty
    bool Empty() const;
    /// get capacity in bytes of buffer
    int32 Capacity() const;
    /// get number of free bytes at back
    int32 Spare() const;

    /// make room for N more bytes
    void Reserve(int32 numBytes);
    /// add bytes to buffer
    void Add(const uint8* data, int32 numBytes);
    /// add uninitialized bytes to buffer, return pointer to start
    uint8* Add(int32 numBytes);
    /// clear the buffer (deletes content, keeps capacity)
    void Clear();
    /// get read-only pointer to content (throws assert if would return nullptr)
    const uint8* Data() const;
    /// get read/write pointer to content (throws assert if would return nullptr)
    uint8* Data();

private:
    /// (re-)allocate buffer
    void alloc(int32 newCapacity);
    /// destroy buffer
    void destroy();
    /// append-copy content into currently allocated buffer, bump size
    void copy(const uint8* ptr, int numBytes);

    int32 size;
    int32 capacity;
    uint8* data;
};

//------------------------------------------------------------------------------
inline
Buffer::Buffer() :
size(0),
capacity(0),
data(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
inline
Buffer::Buffer(Buffer&& rhs) :
size(rhs.size),
capacity(rhs.capacity),
data(rhs.data) {
    rhs.size = 0;
    rhs.capacity = 0;
    rhs.data = nullptr;
}

//------------------------------------------------------------------------------
inline
Buffer::~Buffer() {
    this->destroy();
}

//------------------------------------------------------------------------------
inline void
Buffer::alloc(int32 newCapacity) {
    o_assert_dbg(newCapacity > this->capacity);
    o_assert_dbg(newCapacity > this->size);

    uint8* newBuf = (uint8*) Memory::Alloc(newCapacity);
    if (this->size > 0) {
        o_assert_dbg(this->data);
        Memory::Copy(this->data, newBuf, this->size);
    }
    if (this->data) {
        Memory::Free(this->data);
    }
    this->data = newBuf;
    this->capacity = newCapacity;
}

//------------------------------------------------------------------------------
inline void
Buffer::destroy() {
    if (this->data) {
        Memory::Free(this->data);
    }
    this->data = nullptr;
    this->size = 0;
    this->capacity = 0;
}

//------------------------------------------------------------------------------
inline void
Buffer::copy(const uint8* ptr, int32 numBytes) {
    // NOTE: it is valid to call copy with numBytes==0
    o_assert_dbg(this->data);
    o_assert_dbg((this->size + numBytes) <= this->capacity);
    Memory::Copy(ptr, this->data + this->size, numBytes);
    this->size += numBytes;
}

//------------------------------------------------------------------------------
inline void
Buffer::operator=(Buffer&& rhs) {
    this->destroy();
    this->size = rhs.size;
    this->capacity = rhs.capacity;
    this->data = rhs.data;
    rhs.size = 0;
    rhs.capacity = 0;
    rhs.data = nullptr;
}

//------------------------------------------------------------------------------
inline int32
Buffer::Size() const {
    return this->size;
}

//------------------------------------------------------------------------------
inline bool
Buffer::Empty() const {
    return 0 == this->size;
}

//------------------------------------------------------------------------------
inline int32
Buffer::Capacity() const {
    return this->capacity;
}

//------------------------------------------------------------------------------
inline int32
Buffer::Spare() const {
    return this->capacity - this->size;
}

//------------------------------------------------------------------------------
inline void
Buffer::Reserve(int32 numBytes) {
    // need to grow?
    if ((this->size + numBytes) > this->capacity) {
        const int32 newCapacity = this->size + numBytes;
        this->alloc(newCapacity);
    }
}

//------------------------------------------------------------------------------
inline void
Buffer::Add(const uint8* data, int32 numBytes) {
    this->Reserve(numBytes);
    this->copy(data, numBytes);
}

//------------------------------------------------------------------------------
inline uint8*
Buffer::Add(int32 numBytes) {
    this->Reserve(numBytes);
    uint8* ptr = this->data + this->size;
    this->size += numBytes;
    return ptr;
}

//------------------------------------------------------------------------------
inline void
Buffer::Clear() {
    this->size = 0;
}

//------------------------------------------------------------------------------
inline const uint8*
Buffer::Data() const {
    o_assert(this->data);
    return this->data;
}

//------------------------------------------------------------------------------
inline uint8*
Buffer::Data() {
    o_assert(this->data);
    return this->data;
}

} // namespace Oryol
