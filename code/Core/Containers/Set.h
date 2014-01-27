#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Set
    @brief a sorted, dynamic array of values

    The Set class provides a dynamic array of binary-sorted values similar
    to the std::set class.
 
    @see Array, ArrayMap, Map
*/
#include "Core/Containers/Array.h"

namespace Oryol {
namespace Core {

template<class VALUETYPE> class Set {
public:
    /// default constructor
    Set() {
    };
    /// setup with allocation strategy
    Set(int32 minGrow, int32 maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW) :
        valueArray(minGrow, maxGrow) {
    };
    /// copy constructor (truncates to actual size)
    Set(const Set& rhs) :
        valueArray(rhs.values) {
    };
    /// move constructor (same capacity and size)
    Set(Set&& rhs) :
        valueArray(std::move(rhs.values)) {
    };

    /// copy-assignment operator (truncates to actual size)
    void operator=(const Set& rhs) {
        if (&rhs != this) {
            this->valueArray = rhs.valueArray;
        }
    };
    /// move-assignment operator (same capacity and size)
    void operator=(Set&& rhs) {
        if (&rhs != this) {
            this->valueArray = std::move(rhs.valueArray);
        }
    };
    
    /// set allocation strategy
    void SetAllocStrategy(int32 minGrow, int32 maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW) {
        this->valueArray.SetAllocStrategy(minGrow, maxGrow);
    };
    /// get min grow value
    int32 GetMinGrow() const {
        return this->valueArray.GetMinGrow();
    };
    /// get max grow value
    int32 GetMaxGrow() const {
        return this->valueArray.GetMaxGrow();
    };
    /// get number of elements in array
    int32 Size() const {
        return this->valueArray.Size();
    };
    /// return true if empty
    bool Empty() const {
        return this->valueArray.Empty();
    };
    /// get capacity of array
    int32 Capacity() const {
        return this->valueArray.Capacity();
    };
    /// test if an element exists
    bool Contains(const VALUETYPE& val) const {
        return std::binary_search(this->valueArray.begin(), this->valueArray.end(), val);
    };
    /// find element
    const VALUETYPE* Find(const VALUETYPE& val) const {
        const VALUETYPE* ptr = std::lower_bound(this->valueArray.begin(), this->valueArray.end(), val);
        if (ptr != this->valueArray.end() && val == *ptr) {
            return ptr;
        }
        else {
            return nullptr;
        }
    };
    /// insert element
    void Insert(const VALUETYPE& val) {
        const VALUETYPE* begin = this->valueArray.begin();
        const VALUETYPE* end = this->valueArray.end();
        const VALUETYPE* ptr = std::lower_bound(begin, end, val);
        if ((ptr != end) && (*ptr == val)) {
            o_error("Trying to insert duplicate element!\n");
        }
        else {
            int32 index = ptr - begin;
            this->valueArray.Insert(index, val);
        }
    };
    /// erase element
    void Erase(const VALUETYPE& val) {
        const VALUETYPE* begin = this->valueArray.begin();
        const VALUETYPE* end = this->valueArray.end();
        const VALUETYPE* ptr = std::lower_bound(begin, end, val);
        if (ptr != end) {
            int32 index = ptr - begin;
            this->valueArray.Erase(index);
        }
    };
    /// get value at index
    const VALUETYPE& ValueAtIndex(int32 index) {
        return this->valueArray[index];
    };
    
private:
    Array<VALUETYPE> valueArray;
};
    
} // namespace Core
} // namespace Oryol