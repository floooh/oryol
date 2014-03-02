#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Resource::Locator
    @brief used to locate and share resource data
    
    A Locator describes where the source data for a resource object
    is located. It usually contains a string that can be resolved
    to an URL. Locators are also used for sharing resources. If 2 Locators
    are completely identical, the same resource will be returned for them.
    The optional signature can be used to forbid sharing between
    2 otherwise identical Locators. Only Locators with identical location
    and signature will be shared.
    
    Note that the special signature reserved signature values
    NonSharedSignature (0xFFFFFFFF) and DefaultSignature(0xFFFFFFFE).
*/
#include "Core/Types.h"
#include "Core/String/StringAtom.h"

namespace Oryol {
namespace Resource {
class Locator {
public:
    /// the signature for non-shared locators
    static const uint32 NonSharedSignature = 0xFFFFFFFF;
    /// the default-sharing signature
    static const uint32 DefaultSignature = 0xFFFFFFFE;

    /// default constructor
    Locator();
    /// shared locator from location
    Locator(const Core::StringAtom& location);
    /// shared locator from location and signature
    Locator(const Core::StringAtom& location, uint32 signature);
    
    /// create a unique Locator without location
    static Locator NonShared();
    /// create a unique Locator with location
    static Locator NonShared(const Core::StringAtom& location);
    
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
    const Core::StringAtom& Location() const;
    /// get the signature
    uint32 Signature() const;
    
private:
    Core::StringAtom location;
    uint32 signature;
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
inline const Core::StringAtom&
Locator::Location() const {
    return this->location;
}

//------------------------------------------------------------------------------
inline uint32
Locator::Signature() const {
    return this->signature;
}

} // namespace Resource
} // namespace Oryol
