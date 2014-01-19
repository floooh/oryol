#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Dictionary
    
    A key-value-pair container similar to std::map(), with the following
    differences:
    
    - trying to add a duplicate element will trigger an assertion
    - trying to access a non-existing element with operator[] will
      trigger an assertion
      
    When adding large numbers of elements, consider using the 
    bulk methods, these destroy the sorted order when inserting,
    and sorting will happen inside EndBulk().
    
    The Dictionary uses a double-ended element buffer internally which
    initially has spare room at the front and end. When inserting elements,
    movement happens towards the end which would create less move operations
    (so inserting at the front is just as fast as inserting at the end).
*/
#include "Core/Config.h"
#include "Core/Containers/elementBuffer.h"
#include "Core/Containers/KeyValuePair.h"

namespace Oryol {
namespace Core {

template<class KEY, class VALUE> class Dictionary {
public:
    /// default constructor
    Dictionary() :
        minGrow(ORYOL_CONTAINER_DEFAULT_MIN_GROW),
        maxGrow(ORYOL_CONTAINER_DEFAULT_MAX_GROW) {
    };
    /// setup with allocation strategy
    Dictionary(int32 minGrow_, int32 maxGrow_=ORYOL_CONTAINER_DEFAULT_MAX_GROW) :
        minGrow(minGrow_),
        maxGrow(maxGrow_) {
    };
    /// copy constructor (truncates to actual size)
    Dictionary(const Dictionary& rhs) {
        this->copy(rhs);
    };
    /// move constructor (same capacity and size)
    Dictionary(Dictionary&& rhs) {
        this->move(rhs);
    };
    /// destructor
    ~Dictionary() {
        this->destroy();
    };
    
    /// copy-assignment operator (truncates to actual size)
    void operator=(const Dictionary& rhs) {
        // FIXME: this should be optimized when rhs.size() < this->capacity()!
        this->destroy();
        this->copy(rhs);
    };
    /// move-assignment operator (same capacity and size)
    void operator=(Dictionary&& rhs) {
        // FIXME: this should be optimized when rhs.size() < this->capacity()!
        this->destroy();
        this->move(rhs);
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
    
    /// read/write access single element
    VALUE& operator[](const KEY& key);
    /// read-only access single element
    const VALUE& operator[](const KEY& key) const;
    /// test if an element exists
    bool Contains(const KEY& key) const;
    
    /// increase capacity to hold at least numElements more elements
    void Reserve(int32 numElements);
    /// trim capacity to size (this involves a re-alloc)
    void Trim();
    /// clear the array (deletes elements, keeps capacity)
    void Clear();

    /// insert new element
    void Insert(const KEY& key, const VALUE& value);
    /// insert new element
    void Insert(const KeyValuePair& kvp);
    /// insert new element
    void Insert(KeyValuePair&& kvp);
    /// erase an element by key
    void Erase(const KEY& key);
    
    /// begin bulk-mode
    void BeginBulk();
    /// insert element in bulk-mode (destroys sorting order)
    void InsertBulk(const KEY& key, const VALUE& value);
    /// insert element in bulk-mode (destroys sorting order)
    void InsertBulk(const KeyValuePair& kvp);
    /// insert element in bulk-mode (destroys sorting order)
    void InsertBulk(KeyValuePair&& kvp);
    /// end bulk-mode (sorting happend here)
    void EndBulk();
        
    /// find an element, returns index, or InvalidIndex
    int32 FindIndex(const KEY& key) const;
    /// erase element at index
    void EraseIndex(int32 index) const;
    /// get key at index
    const KEY& KeyAtIndex(int32 index) const;
    /// get value at index (read-only)
    const VALUE& ValueAtIndex(int32 index) const;
    /// get value at index (read/write)
    VALUE& ValueAtIndex(int32 index) const;
    
    /// C++ conform begin
    KeyValuePair<KEY, VALUE>* begin();
    /// C++ conform begin
    const KeyValuePair<KEY, VALUE>* begin() const;
    /// C++ conform end
    KeyValuePair<KEY, VALUE>* end();
    /// C++ conform end
    const KeyValuePair<KEY, VALUE>* end() const;
    
private:
    elementBuffer<KeyValuePair<KEY,VALUE>> buffer;
    int32 minGrow;
    int32 maxGrow;
    bool bulkMode;
};
    
} // namespace Core
} // namespace Oryol
