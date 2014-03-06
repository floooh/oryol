#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Resource::Id
    @brief a generic resource identifier
    
    Resource identifiers are abstract handles to a resource object.
*/
#include "Core/Types.h"

namespace Oryol {
namespace Resource {
    
class Id {
public:
    /// invalid unique stamp constant
    static const uint32 InvalidUniqueStamp = 0xFFFFFFFF;
    /// invalid slot index constant
    static const uint16 InvalidSlotIndex = 0xFFFF;
    /// invalid type constant
    static const uint16 InvalidType = 0xFFFF;

    /// returns an invalid resource id
    static Id InvalidId();
    
    /// default constructor, constructs invalid id
    Id();
    /// create with uniqueStamp, slotIndex and type
    Id(uint32 uniqueStamp, uint16 slotIndex, uint16 type);
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
    /// get the slot index
    uint16 SlotIndex() const;
    /// get the type
    uint16 Type() const;
    /// get the unique-stamp
    uint32 UniqueStamp() const;
    
private:
    uint32 uniqueStamp;
    uint16 slotIndex;
    uint16 type;
};

//------------------------------------------------------------------------------
inline Id
Id::InvalidId() {
    return Id();
}

//------------------------------------------------------------------------------
inline
Id::Id() :
uniqueStamp(InvalidUniqueStamp),
slotIndex(InvalidSlotIndex),
type(InvalidType) {
    // empty
}

//------------------------------------------------------------------------------
inline
Id::Id(uint32 uniqueStamp_, uint16 slotIndex_, uint16 type_) :
uniqueStamp(uniqueStamp_),
slotIndex(slotIndex_),
type(type_) {
    // empty
}

//------------------------------------------------------------------------------
inline
Id::Id(const Id& rhs) :
uniqueStamp(rhs.uniqueStamp),
slotIndex(rhs.slotIndex),
type(rhs.type) {
    // empty
}

//------------------------------------------------------------------------------
inline void
Id::operator=(const Id& rhs) {
    if (this != &rhs) {
        this->uniqueStamp = rhs.uniqueStamp;
        this->slotIndex = rhs.slotIndex;
        this->type = rhs.type;
    }
}

//------------------------------------------------------------------------------
inline bool
Id::operator==(const Id& rhs) const {
    return (this->uniqueStamp == rhs.uniqueStamp) &&
           (this->slotIndex == rhs.slotIndex) &&
           (this->type == rhs.type);
}

//------------------------------------------------------------------------------
inline bool
Id::operator!=(const Id& rhs) const {
    return (this->uniqueStamp != rhs.uniqueStamp) ||
           (this->slotIndex != rhs.slotIndex) ||
           (this->type != rhs.type);
}

//------------------------------------------------------------------------------
inline bool
Id::operator<(const Id& rhs) const {
    if (this->type < rhs.type) {
        return true;
    }
    else if (this->slotIndex < rhs.slotIndex) {
        return true;
    }
    else if (this->uniqueStamp < rhs.uniqueStamp) {
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
inline bool
Id::IsValid() const {
    return InvalidUniqueStamp != this->uniqueStamp;
}

//------------------------------------------------------------------------------
inline void
Id::Invalidate() {
    this->uniqueStamp = InvalidUniqueStamp;
    this->slotIndex = InvalidSlotIndex;
    this->type = InvalidType;
}

//------------------------------------------------------------------------------
inline uint16
Id::SlotIndex() const {
    return this->slotIndex;
}

//------------------------------------------------------------------------------
inline uint16
Id::Type() const {
    return this->type;
}

//------------------------------------------------------------------------------
inline uint32
Id::UniqueStamp() const {
    return this->uniqueStamp;
}

} // namespace Resource
} // namespace Oryol
    
 
    
    
