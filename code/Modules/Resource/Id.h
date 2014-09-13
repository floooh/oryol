#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Id
    @ingroup Resource
    @brief a generic resource identifier
    
    Resource identifiers are abstract handles to a resource object.
*/
#include "Core/Types.h"

namespace Oryol {
    
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
    /// make id from uniqueStamp, slotIndex and type
    uint64 makeId(uint32 uniqueStamp, uint16 slotIndex, uint16 type);

    uint64 id;
};

//------------------------------------------------------------------------------
inline Id
Id::InvalidId() {
    return Id();
}

//------------------------------------------------------------------------------
inline uint64
Id::makeId(uint32 uniqueStamp, uint16 slotIndex, uint16 type) {
    // type must be most-signifant, then uniqueStamp, then slotIndex
    uint64 result = type;
    result <<= 32;
    result |= uniqueStamp;
    result <<= 16;
    result |= slotIndex;
    return result;
}

//------------------------------------------------------------------------------
inline
Id::Id() {
    this->id = this->makeId(InvalidUniqueStamp, InvalidSlotIndex, InvalidType);
}

//------------------------------------------------------------------------------
inline
Id::Id(uint32 uniqueStamp, uint16 slotIndex, uint16 type) {
    this->id = this->makeId(uniqueStamp, slotIndex, type);
}

//------------------------------------------------------------------------------
inline
Id::Id(const Id& rhs) :
id(rhs.id) {
    // empty
}

//------------------------------------------------------------------------------
inline void
Id::operator=(const Id& rhs) {
    if (this != &rhs) {
        this->id = rhs.id;
    }
}

//------------------------------------------------------------------------------
inline bool
Id::operator==(const Id& rhs) const {
    return this->id == rhs.id;
}

//------------------------------------------------------------------------------
inline bool
Id::operator!=(const Id& rhs) const {
    return this->id != rhs.id;
}

//------------------------------------------------------------------------------
inline bool
Id::operator<(const Id& rhs) const {
    return this->id < rhs.id;
}

//------------------------------------------------------------------------------
inline uint16
Id::SlotIndex() const {
    return this->id & 0xFFFF;
}

//------------------------------------------------------------------------------
inline uint16
Id::Type() const {
    return (this->id >> 48) & 0xFFFF;
}

//------------------------------------------------------------------------------
inline uint32
Id::UniqueStamp() const {
    return (this->id >> 16) & 0xFFFFFFFF;
}

//------------------------------------------------------------------------------
inline bool
Id::IsValid() const {
    return InvalidUniqueStamp != this->UniqueStamp();
}

//------------------------------------------------------------------------------
inline void
Id::Invalidate() {
    this->id = this->makeId(InvalidUniqueStamp, InvalidSlotIndex, InvalidType);
}

} // namespace Oryol
    
 
    
    
