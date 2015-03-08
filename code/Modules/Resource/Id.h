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
    /// invalid id value
    static const uint64 invalidId = 0xFFFFFFFFFFFFFFFF;

    union {
        struct {
            uint16 slotIndex;
            uint16 type;
            uint32 uniqueStamp;
        };
        uint64 id;
    };
};

//------------------------------------------------------------------------------
inline Id
Id::InvalidId() {
    return Id();
}

//------------------------------------------------------------------------------
inline
Id::Id() :
id(invalidId) {
    // empty
}

//------------------------------------------------------------------------------
inline
Id::Id(uint32 uniqueStamp_, uint16 slotIndex_, uint16 type_) :
slotIndex(slotIndex_),
type(type_),
uniqueStamp(uniqueStamp_) {
    // empty
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

//------------------------------------------------------------------------------
inline bool
Id::IsValid() const {
    return InvalidUniqueStamp != this->uniqueStamp;
}

//------------------------------------------------------------------------------
inline void
Id::Invalidate() {
    this->id = invalidId;
}

} // namespace Oryol
    
 
    
    
