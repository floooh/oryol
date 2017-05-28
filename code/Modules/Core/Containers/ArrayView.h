#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ArrayView
    @ingroup Core
    @brief reference a number of non-owned items

    An ArrayView<TYPE> is a generic reference to a number of array items
    which are owned by someone else. They are especially usefulas function
    parameters or return values (just keep in mind that thereferenced data is
    not owed, and the ArrayView may be affected by iterator-invalidation!).
    Internally an ArrayViewis just a pointer and an integer (for the number
    of items pointed to).
*/
#include "Core/Config.h"
#include "Core/Assertion.h"

namespace Oryol {

template<typename TYPE> class ArrayView {
public:
    /// default constructor
    ArrayView();
    /// initialize from base pointer, start index and number of items
    ArrayView(TYPE* base, int startIndex, int num);
    /// copy constructor
    ArrayView(const ArrayView& rhs);
    /// copy-assignment
    void operator=(const ArrayView& rhs);
    /// return true if ArrayView is empty
    bool Empty() const;
    /// get number of items
    int Size() const;
    /// set new size
    void SetSize(int size);
    /// get the start index
    int StartIndex() const;
    /// set the start index
    void SetStartIndex(int startIndex);
    /// create a new view into this view
    ArrayView View(int startIndex, int num) const;
    /// read/write access to indexed item
    TYPE& operator[](int index);
    /// read-only access to indexed item
    const TYPE& operator[](int index) const;
    /// C++ conform begin
    TYPE* begin();
    /// C++ conform begin
    const TYPE* begin() const;
    /// C++ conform end
    TYPE* end();
    /// C++ conform end
    const TYPE* end() const;
private:
    TYPE* basePtr;
    int startIndex;
    int num;
};

//------------------------------------------------------------------------------
template<typename TYPE>
ArrayView<TYPE>::ArrayView() :
basePtr(nullptr), startIndex(0), num(0) {
    // empty;
}

//------------------------------------------------------------------------------
template<typename TYPE>
ArrayView<TYPE>::ArrayView(TYPE* basePtr_, int startIndex_, int num_):
basePtr(basePtr_), startIndex(startIndex_), num(num_) {
    // empty
}

//------------------------------------------------------------------------------
template<typename TYPE>
ArrayView<TYPE>::ArrayView(const ArrayView<TYPE>& rhs):
basePtr(rhs.basePtr), startIndex(rhs.startIndex), num(rhs.num) {
    // empty
}

//------------------------------------------------------------------------------
template<typename TYPE> void
ArrayView<TYPE>::operator=(const ArrayView& rhs) {
    this->basePtr = rhs.basePtr;
    this->startIndex = rhs.startIndex;
    this->num = rhs.num;
}

//------------------------------------------------------------------------------
template<typename TYPE> bool
ArrayView<TYPE>::Empty() const {
    return 0 == this->num;
}

//------------------------------------------------------------------------------
template<typename TYPE> int
ArrayView<TYPE>::Size() const {
    return this->num;
}

//------------------------------------------------------------------------------
template<typename TYPE> void
ArrayView<TYPE>::SetSize(int s) {
    o_assert_dbg(s >= 0);
    this->size = s;
}

//------------------------------------------------------------------------------
template<typename TYPE> int
ArrayView<TYPE>::StartIndex() const {
    return this->startIndex;
}

//------------------------------------------------------------------------------
template<typename TYPE> void
ArrayView<TYPE>::SetStartIndex(int i) {
    o_assert_dbg(i >= 0);
    this->startIndex = i;
}

//------------------------------------------------------------------------------
template<typename TYPE> ArrayView<TYPE>
ArrayView<TYPE>::View(int s, int n) const {
    o_assert_dbg(this->basePtr && ((s + n) <= this->num));
    return ArrayView(this->basePtr, this->startIndex+s, n);
}

//------------------------------------------------------------------------------
template<typename TYPE> TYPE&
ArrayView<TYPE>::operator[](int index) {
    o_assert_dbg(this->basePtr && (index >= 0) && (index < this->num));
    return this->basePtr[this->startIndex + index];
}

//------------------------------------------------------------------------------
template<typename TYPE> const TYPE&
ArrayView<TYPE>::operator[](int index) const {
    o_assert_dbg(this->basePtr && (index >= 0) && (index < this->num));
    return this->basePtr[this->startIndex + index];
}

//------------------------------------------------------------------------------
template<typename TYPE> TYPE*
ArrayView<TYPE>::begin() {
    if (this->basePtr) {
        return this->basePtr + this->startIndex;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<typename TYPE> const TYPE*
ArrayView<TYPE>::begin() const {
    if (this->basePtr) {
        return this->basePtr + this->startIndex;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<typename TYPE> TYPE*
ArrayView<TYPE>::end() {
    if (this->basePtr) {
        return this->basePtr + this->startIndex + this->num;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<typename TYPE> const TYPE*
ArrayView<TYPE>::end() const {
    if (this->basePtr) {
        return this->basePtr + this->startIndex + this->num;
    }
    else {
        return nullptr;
    }
}

} // namespace Oryol
