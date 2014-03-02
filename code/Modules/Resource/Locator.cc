//------------------------------------------------------------------------------
//  Locator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Locator.h"

namespace Oryol {
namespace Resource {

//------------------------------------------------------------------------------
Locator::Locator(const Core::StringAtom& loc) :
location(loc),
signature(DefaultSignature) {
    // empty
}

//------------------------------------------------------------------------------
Locator::Locator(const Core::StringAtom& loc, uint32 sig) :
location(loc),
signature(sig) {
    // empty
}

//------------------------------------------------------------------------------
Locator
Locator::NonShared() {
    return Locator(Core::StringAtom(), NonSharedSignature);
}

//------------------------------------------------------------------------------
Locator
Locator::NonShared(const Core::StringAtom& loc) {
    return Locator(loc, NonSharedSignature);
}
    
} // namespace Resource
} // namespace Locator