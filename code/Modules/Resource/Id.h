#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Resource Resource
    @brief resource management building blocks

    @class Oryol::Id
    @ingroup Resource
    @brief a generic resource identifier
    
    Resource identifiers are abstract handles to a resource object.
*/
#include "Core/Types.h"

namespace Oryol {
    
class Id {
public:
    /// unique-stamp type (sizeof all types must remain 64 bit)
    typedef uint32_t UniqueStampT;
    /// slot-index type
    typedef uint16_t SlotIndexT;
    /// resource type type
    typedef uint16_t TypeT;

    /// invalid unique stamp constant
    static const UniqueStampT InvalidUniqueStamp = 0xFFFFFFFF;
    /// invalid slot index constant
    static const SlotIndexT InvalidSlotIndex = 0xFFFF;
    /// invalid type constant
    static const TypeT InvalidType = 0xFFFF;

    /// returns an invalid resource id
    static Id InvalidId();
    
    /// default constructor, constructs invalid id
    Id();
    /// create with uniqueStamp, slotIndex and type
    Id(UniqueStampT uniqueStamp, SlotIndexT slotIndex, TypeT type);
    /// copy constructor
    Id(const Id& rhs);
    
    /// assignment operator
    void operator=(const Id& rhs);
    
    /// equality operator
    bool operator==(const Id& rhs) const;
    /// inequality operator
    bool operator!=(const Id& rhs) const;
    /// less-then operator
    bool operator<(const Id& rhs) const;
    
    /// return true if this is a valid id
    bool IsValid() const;
    /// invalidate the id
    void Invalidate();
    
    /// component access
    union {
        struct {
            SlotIndexT SlotIndex;
            TypeT Type;
            UniqueStampT UniqueStamp;
        };
        uint64_t Value;
    };
    
private:
    /// invalid id value
    static const uint64_t invalidId = 0xFFFFFFFFFFFFFFFF;
};

//------------------------------------------------------------------------------
inline Id
Id::InvalidId() {
    return Id();
}

//------------------------------------------------------------------------------
inline
Id::Id() :
Value(invalidId) {
    // empty
}

//------------------------------------------------------------------------------
inline
Id::Id(UniqueStampT uniqueStamp_, SlotIndexT slotIndex_, TypeT type_) :
SlotIndex(slotIndex_),
Type(type_),
UniqueStamp(uniqueStamp_) {
    // empty
}

//------------------------------------------------------------------------------
inline
Id::Id(const Id& rhs) :
Value(rhs.Value) {
    // empty
}

//------------------------------------------------------------------------------
inline void
Id::operator=(const Id& rhs) {
    if (this != &rhs) {
        this->Value = rhs.Value;
    }
}

//------------------------------------------------------------------------------
inline bool
Id::operator==(const Id& rhs) const {
    return this->Value == rhs.Value;
}

//------------------------------------------------------------------------------
inline bool
Id::operator!=(const Id& rhs) const {
    return this->Value != rhs.Value;
}

//------------------------------------------------------------------------------
inline bool
Id::operator<(const Id& rhs) const {
    return this->Value < rhs.Value;
}

//------------------------------------------------------------------------------
inline bool
Id::IsValid() const {
    return InvalidUniqueStamp != this->UniqueStamp;
}

//------------------------------------------------------------------------------
inline void
Id::Invalidate() {
    this->Value = invalidId;
}

} // namespace Oryol
    
 
    
    
