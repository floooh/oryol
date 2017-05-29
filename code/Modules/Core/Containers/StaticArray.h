#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::StaticArray
    @ingroup Core
    @brief simple C-style array with bounds check in debug mode
*/
#include "Core/Types.h"
#include "Core/Assertion.h"
#include "Core/Containers/Slice.h"
#include <utility>

namespace Oryol {

template<class TYPE, int SIZE> class StaticArray {
public:
    /// default constructor
    StaticArray();
    /// copy constructor
    StaticArray(const StaticArray& rhs);
    /// move constructor
    StaticArray(StaticArray&& rhs);
    
    /// copy-assignment operator
    void operator=(const StaticArray& rhs);
    /// move-assignment operator
    void operator=(StaticArray&& rhs);
    /// get number of elements in array
    int Size() const;
    
    /// read/write access single element
    TYPE& operator[](int index);
    /// read-only access single element
    const TYPE& operator[](int index) const;
    /// get a slice into the array
    Slice<TYPE> MakeSlice(int offset=0, int numItems=EndOfRange);
    
    /// fill the array with a value
    void Fill(const TYPE& val);
    
    /// C++ conform begin
    TYPE* begin();
    /// C++ conform begin
    const TYPE* begin() const;
    /// C++ conform end
    TYPE* end();
    /// C++ conform end
    const TYPE* end() const;

private:
    TYPE items[SIZE];
};
    
//------------------------------------------------------------------------------
template<class TYPE, int SIZE>
StaticArray<TYPE, SIZE>::StaticArray() {
    // empty
}
    
//------------------------------------------------------------------------------
template<class TYPE, int SIZE>
StaticArray<TYPE, SIZE>::StaticArray(const StaticArray& rhs) {
    for (int i = 0; i < SIZE; i++) {
        this->items[i] = rhs.items[i];
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int SIZE>
StaticArray<TYPE, SIZE>::StaticArray(StaticArray&& rhs) {
    for (int i = 0; i < SIZE; i++) {
        this->items[i] = std::move(rhs.items[i]);
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int SIZE> void
StaticArray<TYPE, SIZE>::operator=(const StaticArray& rhs) {
    for (int i = 0; i < SIZE; i++) {
        this->items[i] = rhs.items[i];
    }
}
    
//------------------------------------------------------------------------------
template<class TYPE, int SIZE> void
StaticArray<TYPE, SIZE>::operator=(StaticArray&& rhs) {
    for (int i = 0; i < SIZE; i++) {
        this->items[i] = std::move(rhs.items[i]);
    }
}
    
//------------------------------------------------------------------------------
template<class TYPE, int SIZE> int
StaticArray<TYPE, SIZE>::Size() const {
    return SIZE;
}
    
//------------------------------------------------------------------------------
template<class TYPE, int SIZE> TYPE&
StaticArray<TYPE, SIZE>::operator[](int index) {
    o_assert_range_dbg(index, SIZE);
    return this->items[index];
}
    
//------------------------------------------------------------------------------
template<class TYPE, int SIZE> const TYPE&
StaticArray<TYPE, SIZE>::operator[](int index) const {
    o_assert_range_dbg(index, SIZE);
    return this->items[index];
}

//------------------------------------------------------------------------------
template<class TYPE, int SIZE> Slice<TYPE>
StaticArray<TYPE, SIZE>::MakeSlice(int offset, int numItems) {
    if (numItems == EndOfRange) {
        numItems = SIZE - offset;
    }
    return Slice<TYPE>(&this->items[0], SIZE, offset, numItems);
}

//------------------------------------------------------------------------------
template<class TYPE, int SIZE> void
StaticArray<TYPE, SIZE>::Fill(const TYPE& val) {
    for (int i = 0; i < SIZE; i++) {
        this->items[i] = val;
    }
}
    
//------------------------------------------------------------------------------
template<class TYPE, int SIZE> TYPE*
StaticArray<TYPE, SIZE>::begin() {
    return &(this->items[0]);
}
    
//------------------------------------------------------------------------------
template<class TYPE, int SIZE> const TYPE*
StaticArray<TYPE, SIZE>::begin() const {
    return &(this->items[0]);
}

//------------------------------------------------------------------------------
template<class TYPE, int SIZE> TYPE*
StaticArray<TYPE, SIZE>::end() {
    return &(this->items[SIZE]);
}

//------------------------------------------------------------------------------
template<class TYPE, int SIZE> const TYPE*
StaticArray<TYPE, SIZE>::end() const {
    return &(this->items[SIZE]);
}
    
} // namespace Oryol
    

    
