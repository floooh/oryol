//------------------------------------------------------------------------------
//  Locator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Locator.h"

namespace Oryol {

//------------------------------------------------------------------------------
Locator::Locator(const StringAtom& loc) :
location(loc),
signature(DefaultSignature) {
    // empty
}

//------------------------------------------------------------------------------
Locator::Locator(const char* loc) :
location(loc),
signature(DefaultSignature) {
    // empty
}

//------------------------------------------------------------------------------
Locator::Locator(const StringAtom& loc, uint32_t sig) :
location(loc),
signature(sig) {
    // empty
}

//------------------------------------------------------------------------------
Locator::Locator(const char* loc, uint32_t sig) :
location(loc),
signature(sig) {
    // empty
}

//------------------------------------------------------------------------------
Locator
Locator::NonShared() {
    return Locator(StringAtom(), NonSharedSignature);
}

//------------------------------------------------------------------------------
Locator
Locator::NonShared(const StringAtom& loc) {
    return Locator(loc, NonSharedSignature);
}
    
} // namespace Locator
