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

template<class TYPE> class ArrayView {
public:
    /// default constructor
    ArrayView(): ptr(nullptr), num(0) { };
    /// initialize from pointer to item, and number of items
    ArrayView(TYPE* firstItem, int numItems): ptr(firstItem), num(numItems) { 
        o_assert_dbg(ptr==nullptr ? num==0 : num>0);
    };
    /// copy constructor
    ArrayView(const ArrayView& rhs): ptr(rhs.ptr), num(rhs.num) {
        o_assert_dbg(ptr==nullptr ? num==0 : num>0);
    };
    /// copy-assignment
    void operator=(const ArrayView& rhs) { ptr=rhs.ptr; num=rhs.num; };
    /// return true if ArrayView is empty
    bool Empty() const { return num == 0; };
    /// get number of items
    int Size() const { return num; };
    /// read/write access to indexed item
    TYPE& operator[](int index) {
        o_assert_dbg(ptr && (index >= 0) && (index < num));
        return ptr[index];
    };
    /// read-only access to indexed item
    const TYPE& operator[](int index) const {
        o_assert_dbg(ptr && (index >= 0) && (index < num));
        return ptr[index];
    };
    /// C++ conform begin
    TYPE* begin() { return ptr; };
    /// C++ conform begin
    const TYPE* begin() const { return ptr; };
    /// C++ conform end
    TYPE* end() { return ptr + num; };
    /// C++ conform end
    const TYPE* end() const { return ptr + num; };
private:
    TYPE* ptr;
    int num;
};

} // namespace Oryol
