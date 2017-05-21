//------------------------------------------------------------------------------
//  ResourceLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ResourceLoader.h"

namespace Oryol {

//------------------------------------------------------------------------------
class Locator
ResourceLoader::Locator() const {
    class Locator loc;
    return loc;
}

//------------------------------------------------------------------------------
Id
ResourceLoader::Start() {
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
ResourceState::Code
ResourceLoader::Continue() {
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
ResourceLoader::Cancel() {
    // empty
}

} // namespace Oryol
