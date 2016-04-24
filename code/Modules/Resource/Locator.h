#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Locator
    @ingroup Resources
    @brief resource string name for sharing and locating resources
    
    A Locator describes where the source data for a resource
    is located. It usually contains a string that can be resolved
    to an URL. Locators are also used for sharing. If 2 Locators
    are completely identical, they identify the same resource.
    The optional signature can be used to forbid sharing between
    2 otherwise identical Locators. Only Locators with identical location
    and signature will be shared.
    
    Note that the special signature reserved signature values
    NonSharedSignature (0xFFFFFFFF) and DefaultSignature(0xFFFFFFFE).
*/
#include "Core/Types.h"
#include "Core/String/StringAtom.h"

namespace Oryol {

class Locator {
public:
    /// the signature for non-shared locators
    static const uint32_t NonSharedSignature = 0xFFFFFFFF;
    /// the default-sharing signature
    static const uint32_t DefaultSignature = 0xFFFFFFFE;

    /// default constructor
    Locator();
    /// shared locator from location
    Locator(const StringAtom& location);
    /// shared locator from raw location string
    Locator(const char* location);
    /// shared locator from location and signature
    Locator(const StringAtom& location, uint32_t signature);
    /// shared locator from raw location string
    Locator(const char* location, uint32_t signature);
    
    /// create a unique Locator without location
    static Locator NonShared();
    /// create a unique Locator with location
    static Locator NonShared(const StringAtom& location);
    
    /// copy constructor
    Locator(const Locator& rhs);
    /// assignment
    void operator=(const Locator& rhs);
    
    /// equality
    bool operator==(const Locator& rhs) const;
    /// inequality
    bool operator!=(const Locator& rhs) const;
    /// less-then
    bool operator<(const Locator& rhs) const;
    
    /// return true if this is a shared locator
    bool IsShared() const;
    /// return true if the location is valid
    bool HasValidLocation() const;
    /// get the location
    const StringAtom& Location() const;
    /// get the signature
    uint32_t Signature() const;
    
private:
    StringAtom location;
    uint32_t signature;
};

//------------------------------------------------------------------------------
inline
Locator::Locator() :
signature(DefaultSignature) {
    // empty
}

//------------------------------------------------------------------------------
inline
Locator::Locator(const Locator& rhs) :
location(rhs.location),
signature(rhs.signature) {
    // empty
}

//------------------------------------------------------------------------------
inline void
Locator::operator=(const Locator& rhs) {
    this->location = rhs.location;
    this->signature = rhs.signature;
}

//------------------------------------------------------------------------------
inline bool
Locator::operator==(const Locator& rhs) const {
    return (this->location == rhs.location) && (this->signature == rhs.signature);
}

//------------------------------------------------------------------------------
inline bool
Locator::operator!=(const Locator& rhs) const {
    return (this->location != rhs.location) || (this->signature != rhs.signature);
}

//------------------------------------------------------------------------------
inline bool
Locator::operator<(const Locator& rhs) const {
    if (this->signature < rhs.signature) {
        return true;
    }
    else {
        return this->location < rhs.location;
    }
}

//------------------------------------------------------------------------------
inline bool
Locator::IsShared() const {
    return NonSharedSignature != this->signature;
}

//------------------------------------------------------------------------------
inline bool
Locator::HasValidLocation() const {
    return this->location.IsValid();
}

//------------------------------------------------------------------------------
inline const StringAtom&
Locator::Location() const {
    return this->location;
}

//------------------------------------------------------------------------------
inline uint32_t
Locator::Signature() const {
    return this->signature;
}

} // namespace Oryol
