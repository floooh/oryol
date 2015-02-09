#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::StaticArray
    @ingroup Core
    @brief simple C-style array with bounds check in debug mode
*/
#include "Core/Types.h"
#include "Core/Assertion.h"
#include <utility>

namespace Oryol {

template<class TYPE, int32 SIZE> class StaticArray {
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
    int32 Size() const;
    
    /// read/write access single element
    TYPE& operator[](int32 index);
    /// read-only access single element
    const TYPE& operator[](int32 index) const;
    
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
template<class TYPE, int32 SIZE>
StaticArray<TYPE, SIZE>::StaticArray() {
    // empty
}
    
//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE>
StaticArray<TYPE, SIZE>::StaticArray(const StaticArray& rhs) {
    for (int32 i = 0; i < SIZE; i++) {
        this->items[i] = rhs.items[i];
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE>
StaticArray<TYPE, SIZE>::StaticArray(StaticArray&& rhs) {
    for (int32 i = 0; i < SIZE; i++) {
        this->items[i] = std::move(rhs.items[i]);
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> void
StaticArray<TYPE, SIZE>::operator=(const StaticArray& rhs) {
    for (int32 i = 0; i < SIZE; i++) {
        this->items[i] = rhs.items[i];
    }
}
    
//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> void
StaticArray<TYPE, SIZE>::operator=(StaticArray&& rhs) {
    for (int32 i = 0; i < SIZE; i++) {
        this->items[i] = std::move(rhs.items[i]);
    }
}
    
//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> int32
StaticArray<TYPE, SIZE>::Size() const {
    return SIZE;
}
    
//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> TYPE&
StaticArray<TYPE, SIZE>::operator[](int32 index) {
    o_assert_range_dbg(index, SIZE);
    return this->items[index];
}
    
//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> const TYPE&
StaticArray<TYPE, SIZE>::operator[](int32 index) const {
    o_assert_range_dbg(index, SIZE);
    return this->items[index];
}
    
//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> void
StaticArray<TYPE, SIZE>::Fill(const TYPE& val) {
    for (int32 i = 0; i < SIZE; i++) {
        this->items[i] = val;
    }
}
    
//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> TYPE*
StaticArray<TYPE, SIZE>::begin() {
    return &(this->items[0]);
}
    
//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> const TYPE*
StaticArray<TYPE, SIZE>::begin() const {
    return &(this->items[0]);
}

//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> TYPE*
StaticArray<TYPE, SIZE>::end() {
    return &(this->items[SIZE]);
}

//------------------------------------------------------------------------------
template<class TYPE, int32 SIZE> const TYPE*
StaticArray<TYPE, SIZE>::end() const {
    return &(this->items[SIZE]);
}
    
} // namespace Oryol
    

    