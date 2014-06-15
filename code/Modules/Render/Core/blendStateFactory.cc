//------------------------------------------------------------------------------
//  blendStateFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "blendStateFactory.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
blendStateFactory::blendStateFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
blendStateFactory::~blendStateFactory() {
    o_assert(!this->isValid);
}
    
//------------------------------------------------------------------------------
uint16
blendStateFactory::GetResourceType() const {
    return ResourceType::BlendState;
}
    
//------------------------------------------------------------------------------
void
blendStateFactory::Setup() {
    o_assert(!this->isValid);
    this->isValid = true;
}
    
//------------------------------------------------------------------------------
void
blendStateFactory::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
}
    
//------------------------------------------------------------------------------
bool
blendStateFactory::IsValid() const {
    return this->isValid;
}
    
//------------------------------------------------------------------------------
void
blendStateFactory::SetupResource(blendState& bs) {
    o_assert(this->isValid);
    o_assert(bs.GetState() == Resource::State::Setup);
    
    // we actually don't do anything here, since we're directly
    // using the state info from the setup object
    bs.setState(Resource::State::Valid);
}
    
//------------------------------------------------------------------------------
void
blendStateFactory::DestroyResource(blendState& bs) {
    o_assert(this->isValid);
    o_assert(bs.GetState() == Resource::State::Valid);
    bs.clear();
    bs.setState(Resource::State::Setup);
}

} // namespace Render
} // namespace Oryol
