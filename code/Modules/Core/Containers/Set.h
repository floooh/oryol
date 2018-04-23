#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Set
    @ingroup Core
    @brief a sorted, dynamic array of values

    The Set class provides a dynamic array of binary-sorted values similar
    to the std::set class. 
     
    @see Array, ArrayMap, Map
*/
#include <algorithm>
#include "Core/Containers/Array.h"

namespace Oryol {

template<class VALUE> class Set {
public:
    /// default constructor
    Set();
    /// copy constructor (truncates to actual size)
    Set(const Set& rhs);
    /// move constructor (same capacity and size)
    Set(Set&& rhs);

    /// copy-assignment operator (truncates to actual size)
    void operator=(const Set& rhs);
    /// move-assignment operator (same capacity and size)
    void operator=(Set&& rhs);
    
    /// set allocation strategy
    void SetAllocStrategy(int minGrow, int maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    /// get min grow value
    int GetMinGrow() const;
    /// get max grow value
    int GetMaxGrow() const;
    /// get number of elements in array
    int Size() const;
    /// return true if empty
    bool Empty() const;
    /// get capacity of array
    int Capacity() const;
    /// clear the array (deletes elements, keeps capacity)
    void Clear();
    /// test if an element exists
    bool Contains(const VALUE& val) const;
    /// find element
    const VALUE* Find(const VALUE& val) const;
    /// add element
    void Add(const VALUE& val);
    /// erase element
    void Erase(const VALUE& val);
    /// get value at index
    const VALUE& ValueAtIndex(int index) const;
    
    /// C++ conform begin
    VALUE* begin();
    /// C++ conform begin
    const VALUE* begin() const;
    /// C++ conform end
    VALUE* end();
    /// C++ conform end
    const VALUE* end() const;
    
private:
    Array<VALUE> valueArray;
};

//------------------------------------------------------------------------------
template<class VALUE>
Set<VALUE>::Set() {
    // empty
}

//------------------------------------------------------------------------------
template<class VALUE>
Set<VALUE>::Set(const Set& rhs) :
valueArray(rhs.valueArray) {
    // empty
}

//------------------------------------------------------------------------------
template<class VALUE>
Set<VALUE>::Set(Set&& rhs) :
valueArray(std::move(rhs.valueArray)) {
    // empty
}
    
//------------------------------------------------------------------------------
template<class VALUE> void
Set<VALUE>::operator=(const Set& rhs) {
    if (&rhs != this) {
        this->valueArray = rhs.valueArray;
    }
}

//------------------------------------------------------------------------------
template<class VALUE> void
Set<VALUE>::operator=(Set&& rhs) {
    if (&rhs != this) {
        this->valueArray = std::move(rhs.valueArray);
    }
}
    
//------------------------------------------------------------------------------
template<class VALUE> void
Set<VALUE>::SetAllocStrategy(int minGrow, int maxGrow) {
    this->valueArray.SetAllocStrategy(minGrow, maxGrow);
}

//------------------------------------------------------------------------------
template<class VALUE> int
Set<VALUE>::GetMinGrow() const {
    return this->valueArray.GetMinGrow();
}

//------------------------------------------------------------------------------
template<class VALUE> int
Set<VALUE>::GetMaxGrow() const {
    return this->valueArray.GetMaxGrow();
}

//------------------------------------------------------------------------------
template<class VALUE> int
Set<VALUE>::Size() const {
    return this->valueArray.Size();
}

//------------------------------------------------------------------------------
template<class VALUE> bool
Set<VALUE>::Empty() const {
    return this->valueArray.Empty();
}

//------------------------------------------------------------------------------
template<class VALUE> int
Set<VALUE>::Capacity() const {
    return this->valueArray.Capacity();
}

//------------------------------------------------------------------------------
template<class VALUE> void
Set<VALUE>::Clear() {
    this->valueArray.Clear();
}

//------------------------------------------------------------------------------
template<class VALUE> bool
Set<VALUE>::Contains(const VALUE& val) const {
    return std::binary_search(this->valueArray.begin(), this->valueArray.end(), val);
}

//------------------------------------------------------------------------------
template<class VALUE> const VALUE*
Set<VALUE>::Find(const VALUE& val) const {
    const VALUE* ptr = std::lower_bound(this->valueArray.begin(), this->valueArray.end(), val);
    if (ptr != this->valueArray.end() && val == *ptr) {
        return ptr;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<class VALUE> void
Set<VALUE>::Add(const VALUE& val) {
    const VALUE* begin = this->valueArray.begin();
    const VALUE* end = this->valueArray.end();
    const VALUE* ptr = std::lower_bound(begin, end, val);
    if ((ptr != end) && (*ptr == val)) {
        o_error("Trying to insert duplicate element!\n");
    }
    else {
        int index = int(ptr - begin);
        this->valueArray.Insert(index, val);
    }
}

//------------------------------------------------------------------------------
template<class VALUE> void
Set<VALUE>::Erase(const VALUE& val) {
    const VALUE* begin = this->valueArray.begin();
    const VALUE* end = this->valueArray.end();
    const VALUE* ptr = std::lower_bound(begin, end, val);
    if (ptr != end) {
        int index = int(ptr - begin);
        this->valueArray.Erase(index);
    }
}

//------------------------------------------------------------------------------
template<class VALUE> const VALUE&
Set<VALUE>::ValueAtIndex(int index) const {
    return this->valueArray[index];
};
    
//------------------------------------------------------------------------------
template<class VALUE> VALUE*
Set<VALUE>::begin() {
    return this->valueArray.begin();
}

//------------------------------------------------------------------------------
template<class VALUE> const VALUE*
Set<VALUE>::begin() const {
    return this->valueArray.begin();
}

//------------------------------------------------------------------------------
template<class VALUE> VALUE*
Set<VALUE>::end() {
    return this->valueArray.end();
}

//------------------------------------------------------------------------------
template<class VALUE> const VALUE*
Set<VALUE>::end() const {
    return this->valueArray.end();
}

} // namespace Oryol
