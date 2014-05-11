//------------------------------------------------------------------------------
//  stateBlockFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "stateBlockFactory.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
stateBlockFactory::stateBlockFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
stateBlockFactory::~stateBlockFactory() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
uint16
stateBlockFactory::GetResourceType() const {
    return ResourceType::StateBlock;
}

//------------------------------------------------------------------------------
void
stateBlockFactory::Setup() {
    o_assert(!this->isValid);
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
stateBlockFactory::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
stateBlockFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
stateBlockFactory::SetupResource(stateBlock& sb) {
    o_assert(this->isValid);
    o_assert(sb.GetState() == Resource::State::Setup);

    // we actually don't do anything here, since we're directly
    // using the state info from the setup object, this may change
    // when we support some sort of rendering API state blocks
    sb.setState(Resource::State::Valid);
}

//------------------------------------------------------------------------------
void
stateBlockFactory::DestroyResource(stateBlock& sb) {
    o_assert(this->isValid);
    o_assert(Resource::State::Valid == sb.GetState());
    sb.clear();
    sb.setState(Resource::State::Setup);
}
   
} // namespace Render
} // namespace Oryol