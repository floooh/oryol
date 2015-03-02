//------------------------------------------------------------------------------
//  ResourceLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ResourceLoader.h"

namespace Oryol {

OryolClassImpl(ResourceLoader);

//------------------------------------------------------------------------------
const class Locator&
ResourceLoader::Locator() const {
    return Locator();
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

} // namespace Oryol