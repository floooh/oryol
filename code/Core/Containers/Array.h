#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Array
    @brief dynamic array class similar to std::vector
    
    Dynamic array class (similar to std::vector) with room for growth at
    the end. Internally this is a double-ended array, e.g. erasing items
    at the front does not involve a move-operation, and erasing in the
    middle will move either from the front or the back, depending on what
    would involve less move ops, or use EraseSwap() if the element
    ordering is not important. If there is no spare room at the front
    (this is the is always the case, unless an element has been
    removed from the front), insertion at the front will involve
    size() move operations.
 
    When working with pointers or references into the array keep in mind
    that operations which add new elements may reallocate the whole array
    at a different memory location!
    
    NOTE: An array *copy* operation will trim the capacity to the
    number of elements, so that no spare room is in the array. A *move*
    will not truncate but keep the previous size and capacity.
    
    NOTE: An array growth operation will truncate any spare room
    at the front.
    
    For sorting, iterating and sorted insertion, use the standard 
    algorithm stuff!
    
    @see ArrayMap, Map, Set, HashSet
*/
#include "Core/Config.h"
#include "Core/Containers/elementBuffer.h"

namespace Oryol {
namespace Core {

template<class TYPE> class Array {
public:
    /// default constructor
    Array() :
        minGrow(ORYOL_CONTAINER_DEFAULT_MIN_GROW),
        maxGrow(ORYOL_CONTAINER_DEFAULT_MAX_GROW) {
    };
    /// setup with allocation strategy
    Array(int32 minGrow_, int32 maxGrow_=ORYOL_CONTAINER_DEFAULT_MAX_GROW) :
        minGrow(minGrow_),
        maxGrow(maxGrow_) {
    };
    /// copy constructor (truncates to actual size)
    Array(const Array& rhs) {
        this->copy(rhs);
    };
    /// move constructor (same capacity and size)
    Array(Array&& rhs) {
        this->move(rhs);
    };
    /// destructor
    ~Array() {
        this->destroy();
    };

    /// copy-assignment operator (truncates to actual size)
    void operator=(const Array& rhs) {
        // FIXME: this should be optimized when rhs.size() < this->capacity()!
        if (&rhs != this) {
            this->destroy();
            this->copy(rhs);
        }
    };
    /// move-assignment operator (same capacity and size)
    void operator=(Array&& rhs) {
        // FIXME: this should be optimized when rhs.size() < this->capacity()!
        if (&rhs != this) {
            this->destroy();
            this->move(std::move(rhs));
        }
    };
    
    /// set allocation strategy
    void SetAllocStrategy(int32 minGrow_, int32 maxGrow_=ORYOL_CONTAINER_DEFAULT_MAX_GROW) {
        this->minGrow = minGrow_;
        this->maxGrow = maxGrow_;
    };
    /// get min grow value
    int32 GetMinGrow() const {
        return this->minGrow;
    };
    /// get max grow value
    int32 GetMaxGrow() const {
        return this->maxGrow;
    };
    /// get number of elements in array
    int32 Size() const {
        return this->buffer.size();
    };
    /// return true if empty
    bool Empty() const {
        return this->buffer.elmStart == this->buffer.elmEnd;
    };
    /// get capacity of array
    int32 Capacity() const {
        return this->buffer.capacity();
    };
    /// get number of free slots at back of array
    int32 Spare() const {
        return this->buffer.backSpare();
    };
    
    /// read/write access single element
    TYPE& operator[](int32 index) {
        return this->buffer[index];
    };
    /// read-only access single element
    const TYPE& operator[](int32 index) const {
        return this->buffer[index];
    };
    /// read/write access to first element
    TYPE& Front() {
        return this->buffer.front();
    };
    /// read-only access to first element
    const TYPE& Front() const {
        return this->buffer.front();
    };
    /// read/write access to last element
    TYPE& Back() {
        return this->buffer.back();
    };
    /// read-only access to first element
    const TYPE& Back() const {
        return this->buffer.back();
    };

    /// increase capacity to hold at least numElements more elements
    void Reserve(int32 numElements) {
        int32 newCapacity = this->buffer.size() + numElements;
        if (newCapacity > this->buffer.capacity()) {
            this->adjustCapacity(newCapacity);
        }
    };
    /// trim capacity to size (this involves a re-alloc)
    void Trim() {
        const int32 curSize = this->buffer.size();
        if (curSize < this->buffer.capacity()) {
            this->adjustCapacity(curSize);
        }
    };
    /// clear the array (deletes elements, keeps capacity)
    void Clear() {
        this->buffer.clear();
    };
    
    /// copy-add element to back of array
    void AddBack(const TYPE& elm) {
        if (this->buffer.backSpare() == 0) {
            this->grow();
        }
        this->buffer.pushBack(elm);
    };
    /// move-add element to back of array
    void AddBack(TYPE&& elm) {
        if (this->buffer.backSpare() == 0) {
            this->grow();
        }
        this->buffer.pushBack(std::move(elm));
    };
    /// copy-insert element at index, keep array order
    void Insert(int32 index, const TYPE& elm) {
        if (this->buffer.spare() == 0) {
            this->grow();
        }
        this->buffer.insert(index, elm);
    };
    /// move-insert element at index, keep array order
    void Insert(int32 index, TYPE&& elm) {
        if (this->buffer.spare() == 0) {
            this->grow();
        }
        this->buffer.insert(index, std::move(elm));
    };
    /// emplace new element at back of array
    template<class... ARGS> void EmplaceBack(ARGS&&... args) {
        if (this->buffer.backSpare() == 0) {
            this->grow();
        }
        this->buffer.emplaceBack(std::forward<ARGS>(args)...);
    };
    
    /// erase element at index, keep element ordering
    void Erase(int32 index) {
        this->buffer.erase(index);
    };
    /// erase element at index, swap-in front or back element (destroys element ordering)
    void EraseSwap(int32 index) {
        this->buffer.eraseSwap(index);
    };
    /// erase element at index, always swap-in from back (destroys element ordering)
    void EraseSwapBack(int32 index) {
        this->buffer.eraseSwapBack(index);
    };
    /// erase element at index, always swap-in from front (destroys element ordering)
    void EraseSwapFront(int32 index) {
        this->buffer.eraseSwapFront(index);
    };
    
    /// find element index with slow linear search
    int32 FindIndexLinear(const TYPE& elm, int32 startIndex=0, int32 endIndex=InvalidIndex) {
        const int32 size = this->buffer.size();
        if (size > 0) {
            o_assert(startIndex < size);
            if (InvalidIndex == endIndex) {
                endIndex = size;
            }
            else {
                o_assert(endIndex <= size);
            }
            o_assert(startIndex <= endIndex);
            for (int32 i = startIndex; i < endIndex; i++) {
                TYPE* ptr = this->buffer.elmStart + i;
                if (elm == *ptr) {
                    return i;
                }
            }
        }
        // fallthrough: not found
        return InvalidIndex;
    };
    
    /// C++ conform begin
    TYPE* begin() {
        return this->buffer.elmStart;
    };
    /// C++ conform begin
    const TYPE* begin() const {
        return this->buffer.elmStart;
    };
    /// C++ conform end
    TYPE* end() {
        return this->buffer.elmEnd;
    };
    /// C++ conform end
    const TYPE* end() const {
        return this->buffer.elmEnd;
    };
    
private:
    /// destroy array resources
    void destroy() {
        this->minGrow = 0;
        this->maxGrow = 0;
        this->buffer.destroy();
    };
    /// copy from other array
    void copy(const Array& rhs) {
        this->minGrow = rhs.minGrow;
        this->maxGrow = rhs.maxGrow;
        this->buffer = rhs.buffer;
    };
    /// move from other array
    void move(Array&& rhs) {
        this->minGrow = rhs.minGrow;
        this->maxGrow = rhs.maxGrow;
        this->buffer  = std::move(rhs.buffer);
        rhs.minGrow = 0;
        rhs.maxGrow = 0;
    };
    /// reallocate with new capacity
    void adjustCapacity(int32 newCapacity) {
        this->buffer.alloc(newCapacity, 0);
    };
    /// grow to make room
    void grow() {
        const int32 curCapacity = this->buffer.capacity();
        int growBy = curCapacity >> 1;
        if (growBy < minGrow) {
            growBy = minGrow;
        }
        else if (growBy > maxGrow) {
            growBy = maxGrow;
        }
        o_assert(growBy > 0);
        int32 newCapacity = curCapacity + growBy;
        this->adjustCapacity(newCapacity);
    };
    
    elementBuffer<TYPE> buffer;
    int32 minGrow;
    int32 maxGrow;
};

} // namespace Core
} // namespace Oryol