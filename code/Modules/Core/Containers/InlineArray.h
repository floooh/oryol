#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::InlineArray
    @ingroup Core
    @brief dynamic array without heap allocation and a fixed capacity

    The InlineArray class is a simplified version of Array with
    a fixed maximum capacity (defined by a template parameter) and
    no heap allocation.

    NOTE: The capacity is part of the type, thus you can only
    copy or move from the other InlineArrays with the same
    capacity!.

    NOTE 2: unused 'invalid' items in the array will be in their
    default-constructed state
*/
#include "Core/Config.h"
#include "Core/Assertion.h"
#include <initializer_list>
#include <utility>

namespace Oryol {

template<class TYPE, int CAPACITY>  class InlineArray {
public:
    /// default constructor
    InlineArray();
    /// copy constructor
    InlineArray(const InlineArray& rhs);
    /// move constructor (TYPE must be moveable)
    InlineArray(InlineArray&& rhs);
    /// setup array from initializer_list
    InlineArray(std::initializer_list<TYPE> l);
    /// destructor
    ~InlineArray();

    /// copy-assignment
    void operator=(const InlineArray& rhs);
    /// move-assignment (TYPE must be moveable)
    void operator=(InlineArray&& rhs);

    /// get number of valid items in the array
    int Size() const;
    /// return true if no items in array
    bool Empty() const;
    /// return true if the array is full
    bool Full() const;
    /// get capacity (always identical to CAPACITY templ param)
    int Capacity() const;
    /// get number of free slots at back 
    int Spare() const;

    /// read/write an existing item
    TYPE& operator[](int index);
    /// read-only access an existing item
    const TYPE& operator[](int index) const;

    /// clear the array (destruct items and reset size to 0)
    void Clear();
    /// reset the array (do *not* destruct items, only reset size to 0)
    void Reset();

    /// copy-add a new item to the back
    TYPE& Add(const TYPE& item);
    /// move-add a new item to the back
    TYPE& Add(TYPE&& item);
    /// emplace a new item to the back
    template<class... ARGS> TYPE& Add(ARGS&&... args);
    /// add new items by initializer_list
    void Add(std::initializer_list<TYPE> items);
    /// copy-insert element at index, keep array order
    void Insert(int index, const TYPE& elm);
    /// move-insert element at index, keep array order
    void Insert(int index, TYPE&& elm);
    /// erase element at index, keep element order
    void Erase(int index);

    /// C++ begin
    TYPE* begin();
    /// C++ const begin
    const TYPE* begin() const;
    /// C++ end
    TYPE* end();
    /// C++ const end
    const TYPE* end() const;

private:
    /// check if enough room is avaible to add n items, fatal error otherwise
    void checkRoom(int numItems) const;
    /// make room for an inserted element
    void moveInsert(int index);
    /// erase an element
    void moveErase(int index);

    TYPE items[CAPACITY];
    int size;
};

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY>
InlineArray<TYPE, CAPACITY>::InlineArray():
size(0) {
    // empty
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY>
InlineArray<TYPE, CAPACITY>::InlineArray(const InlineArray& rhs) {
    this->size = rhs.size;
    for (int i = 0; i < this->size; i++) {
        this->items[i] = rhs[i];
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY>
InlineArray<TYPE, CAPACITY>::InlineArray(InlineArray&& rhs) {
    this->size = rhs.size;
    for (int i = 0; i < this->size; i++) {
        this->items[i] = std::move(rhs[i]);
    }
    rhs.size = 0;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY>
InlineArray<TYPE, CAPACITY>::InlineArray(std::initializer_list<TYPE> l) {
    this->size = 0;
    this->checkRoom(int(l.size()));
    for (const auto& item : l) {
        this->items[this->size++] = item;
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY>
InlineArray<TYPE, CAPACITY>::~InlineArray() {
    this->size = 0;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::operator=(const InlineArray& rhs) {
    if (rhs.size < this->size) {
        // if rhs has less items, 'destroy' own items that would not be overwritten
        for (int i = rhs.size; i < this->size; i++) {
            this->items[i] = TYPE();
        }
    }
    for (int i = 0; i < rhs.size; i++) {
        this->items[i] = rhs.items[i];
    }
    this->size = rhs.size;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::operator=(InlineArray&& rhs) {
    if (rhs.size < this->size) {
        // if rhs has less items, destroy own items that would not be overwritten
        for (int i =  rhs.size; i < this->size; i++) {
            this->items[i] = TYPE();
        }
    }
    for (int i = 0; i < rhs.size; i++) {
        this->items[i] = std::move(rhs.items[i]);
    }
    this->size = rhs.size;
    rhs.size = 0;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> int
InlineArray<TYPE, CAPACITY>::Size() const {
    return this->size;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> bool
InlineArray<TYPE, CAPACITY>::Empty() const {
    return 0 == this->size;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> bool
InlineArray<TYPE, CAPACITY>::Full() const {
    return CAPACITY == this->size;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> int
InlineArray<TYPE, CAPACITY>::Capacity() const {
    return CAPACITY;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> int
InlineArray<TYPE, CAPACITY>::Spare() const {
    return CAPACITY - this->size;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> TYPE&
InlineArray<TYPE, CAPACITY>::operator[](int index) {
    o_assert_range_dbg(index, this->size);
    return this->items[index];
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> const TYPE&
InlineArray<TYPE, CAPACITY>::operator[](int index) const {
    o_assert_range_dbg(index, this->size);
    return this->items[index];
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::Clear() {
    for (int i = 0; i < this->size; i++) {
        this->items[i] = TYPE();
    }
    this->size = 0;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::Reset() {
    this->size = 0;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::checkRoom(int numItems) const {
    if ((this->size + numItems) > CAPACITY) {
        o_error("InlineArray full!");
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> TYPE&
InlineArray<TYPE, CAPACITY>::Add(const TYPE& item) {
    this->checkRoom(1);
    this->items[this->size++] = item;
    return this->items[this->size - 1];
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> TYPE&
InlineArray<TYPE, CAPACITY>::Add(TYPE&& item) {
    this->checkRoom(1);
    this->items[this->size++] = std::move(item);
    return this->items[this->size - 1];
}
//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> template<class... ARGS> TYPE&
InlineArray<TYPE, CAPACITY>::Add(ARGS&&... args) {
    this->checkRoom(1);
    this->items[this->size++] = TYPE(std::forward<ARGS>(args)...);
    return this->items[this->size - 1];
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::Add(std::initializer_list<TYPE> l) {
    this->checkRoom(int(l.size()));
    for (const auto& item : l) {
        this->items[this->size++] = item;
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::moveInsert(int index) {
    o_assert_dbg((this->size > 0) && (this->size < CAPACITY));
    for (int i = this->size; i > index; i--) {
        this->items[i] = std::move(this->items[i-1]);
    }
    this->size++;
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::moveErase(int index) {
    for (int i = index; i < (this->size - 1); i++) {
        this->items[i] = std::move(this->items[i+1]);
    }
    this->items[--this->size] = TYPE();
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::Insert(int index, const TYPE& item) {
    o_assert_dbg((index >= 0) && (index <= this->size));
    this->checkRoom(1);
    if (index == this->size) {
        this->items[this->size++] = item;
    }
    else {
        this->moveInsert(index);
        this->items[index] = item;
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::Insert(int index, TYPE&& item) {
    o_assert_dbg((index >= 0) && (index <= this->size));
    this->checkRoom(1);
    if (index == this->size) {
        this->items[this->size++] = std::move(item);
    }
    else {
        this->moveInsert(index);
        this->items[index] = std::move(item);
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> void
InlineArray<TYPE, CAPACITY>::Erase(int index) {
    o_assert_dbg(this->size > 0);
    o_assert_range_dbg(index, this->size);
    if (index == (this->size - 1)) {
        this->items[--this->size] = TYPE();
    }
    else {
        this->moveErase(index);
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> TYPE*
InlineArray<TYPE, CAPACITY>::begin() {
    return &(this->items[0]);
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> const TYPE*
InlineArray<TYPE, CAPACITY>::begin() const {
    return &(this->items[0]);
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> TYPE*
InlineArray<TYPE, CAPACITY>::end() {
    return &(this->items[this->size]);
}

//------------------------------------------------------------------------------
template<class TYPE, int CAPACITY> const TYPE*
InlineArray<TYPE, CAPACITY>::end() const {
    return &(this->items[this->size]);
}

} // namespace Oryol
