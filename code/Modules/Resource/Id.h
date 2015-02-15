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
    static const uint8 InvalidType = 0xFF;
    /// invalid label constant
    static const uint8 InvalidLabel = 0xFF;

    /// returns an invalid resource id
    static Id InvalidId();
    
    /// default constructor, constructs invalid id
    Id();
    /// create with uniqueStamp, slotIndex and type
    Id(uint32 uniqueStamp, uint16 slotIndex, uint8 type, uint8 label);
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
    uint8 Type() const;
    /// get the label
    uint8 Label() const;
    /// get the unique-stamp
    uint32 UniqueStamp() const;
    
private:
    /// invalid id value
    static const uint64 invalidId = 0xFFFFFFFFFFFFFFFF;

    union {
        struct {
            uint16 slotIndex;
            uint8 type;
            uint8 label;
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
Id::Id(uint32 uniqueStamp_, uint16 slotIndex_, uint8 type_, uint8 label_) :
slotIndex(slotIndex_),
type(type_),
label(label_),
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
inline uint8
Id::Type() const {
    return this->type;
}

//------------------------------------------------------------------------------
inline uint8
Id::Label() const {
    return this->label;
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
    
 
    
    
