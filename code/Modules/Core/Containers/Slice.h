#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Slice
    @ingroup Core
    @brief a reference to an array-like range of non-owned items

    An Slice<TYPE> is a generic reference to a number of array items
    which are owned by someone else. They are especially useful as function
    parameters or return values (just keep in mind that the referenced data is
    not owed, and the Slice may be affected by iterator-invalidation!)
*/
#include "Core/Config.h"
#include "Core/Assertion.h"

namespace Oryol {

template<typename TYPE> class Slice {
public:
    /// default constructor
    Slice();
    /// init from base pointer, start index and number of items
    Slice(TYPE* base, int numBaseItems, int sliceOffset=0, int numSliceItems=EndOfRange);
    /// copy constructor
    Slice(const Slice& rhs);
    /// copy-assignment
    void operator=(const Slice& rhs);
    /// read/write access to indexed item
    TYPE& operator[](int index);
    /// read-only access to indexed item
    const TYPE& operator[](int index) const;
    /// create a new slice from this slice
    class Slice MakeSlice(int sliceOffset=0, int numSliceItems=EndOfRange) const;

    /// reset the slice to its default state
    void Reset();
    /// return true if Slice is empty
    bool Empty() const;
    /// get number of items
    int Size() const;
    /// set new absolute size
    void SetSize(int numSliceItems);
    /// get the start index
    int Offset() const;
    /// set new absolute offset
    void SetOffset(int sliceOffset);
    /// move offset
    void Move(int delta);
    /// if slice is 'to the right' of the gap, move offset to left by gapSize
    void FillGap(int gapOffset, int gapSize);

    /// C++ begin
    TYPE* begin();
    /// C++ begin
    const TYPE* begin() const;
    /// C++ end
    TYPE* end();
    /// C++ end
    const TYPE* end() const;

private:
    TYPE* basePtr = nullptr;
    int baseSize = 0;
    int offset = 0;
    int num = 0;
};

//------------------------------------------------------------------------------
template<typename TYPE>
Slice<TYPE>::Slice() {
    // empty
}

//------------------------------------------------------------------------------
template<typename TYPE>
Slice<TYPE>::Slice(TYPE* base, int numBaseItems, int sliceOffset, int sliceNumItems):
basePtr(base),
baseSize(numBaseItems),
offset(sliceOffset),
num((sliceNumItems==EndOfRange)?numBaseItems:sliceNumItems)
{
    o_assert_dbg(basePtr && (offset>=0) && (num>=0) && ((offset+num)<=baseSize));
}

//------------------------------------------------------------------------------
template<typename TYPE>
Slice<TYPE>::Slice(const Slice& rhs):
basePtr(rhs.basePtr), baseSize(rhs.baseSize), offset(rhs.offset), num(rhs.num) {
    // empty
}

//------------------------------------------------------------------------------
template<typename TYPE> void
Slice<TYPE>::operator=(const Slice& rhs) {
    this->basePtr = rhs.basePtr;
    this->baseSize = rhs.baseSize;
    this->offset = rhs.offset;
    this->num = rhs.num;
}

//------------------------------------------------------------------------------
template<typename TYPE> TYPE&
Slice<TYPE>::operator[](int index) {
    o_assert_dbg(this->basePtr && (index >= 0) && (index < this->num));
    return this->basePtr[this->offset + index];
}

//------------------------------------------------------------------------------
template<typename TYPE> const TYPE&
Slice<TYPE>::operator[](int index) const {
    o_assert_dbg(this->basePtr && (index >= 0) && (index < this->num));
    return this->basePtr[this->offset + index];
}

//------------------------------------------------------------------------------
template<typename TYPE> Slice<TYPE>
Slice<TYPE>::MakeSlice(int sliceOffset, int numSliceItems) const {
    if (numSliceItems == EndOfRange) {
        numSliceItems = this->num - sliceOffset;
    }
    return Slice(this->basePtr, this->baseSize, this->offset+sliceOffset, numSliceItems);
}

//------------------------------------------------------------------------------
template<typename TYPE> void
Slice<TYPE>::Reset() {
    this->basePtr = nullptr;
    this->baseSize = 0;
    this->offset = 0;
    this->num = 0;
}

//------------------------------------------------------------------------------
template<typename TYPE> bool
Slice<TYPE>::Empty() const {
    return 0 == this->num;
}

//------------------------------------------------------------------------------
template<typename TYPE> int
Slice<TYPE>::Size() const {
    return this->num;
}

//------------------------------------------------------------------------------
template<typename TYPE> void
Slice<TYPE>::SetSize(int numSliceItems) {
    o_assert_dbg((numSliceItems >= 0) && ((this->offset+numSliceItems) <= this->baseSize));
    this->num = numSliceItems;
}

//------------------------------------------------------------------------------
template<typename TYPE> int
Slice<TYPE>::Offset() const {
    return this->offset;
}

//------------------------------------------------------------------------------
template<typename TYPE> void
Slice<TYPE>::SetOffset(int sliceOffset) {
    o_assert_dbg((sliceOffset >= 0) && ((sliceOffset+this->num) <= this->baseSize));
    this->offset = sliceOffset;
}

//------------------------------------------------------------------------------
template<typename TYPE> void
Slice<TYPE>::Move(int delta) {
    o_assert_dbg((this->offset + delta) >= 0);
    o_assert_dbg((this->offset + delta + this->num) <= this->baseSize);
    this->offset += delta;
}

//------------------------------------------------------------------------------
template<typename TYPE> void
Slice<TYPE>::FillGap(int gapOffset, int gapSize) {
    o_assert((gapOffset >= 0) && (gapSize > 0) && ((gapOffset+gapSize) <= this->baseSize));
    if (this->offset >= (gapOffset + gapSize)) {
        this->offset -= gapSize;
    }
}

//------------------------------------------------------------------------------
template<typename TYPE> TYPE*
Slice<TYPE>::begin() {
    if (this->basePtr) {
        return this->basePtr + this->offset;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<typename TYPE> const TYPE*
Slice<TYPE>::begin() const {
    if (this->basePtr) {
        return this->basePtr + this->offset;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<typename TYPE> TYPE*
Slice<TYPE>::end() {
    if (this->basePtr) {
        return this->basePtr + this->offset + this->num;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<typename TYPE> const TYPE*
Slice<TYPE>::end() const {
    if (this->basePtr) {
        return this->basePtr + this->offset + this->num;
    }
    else {
        return nullptr;
    }
}

} // namespace Oryol
