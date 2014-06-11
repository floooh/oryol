//------------------------------------------------------------------------------
//  depthStencilStateFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "depthStencilStateFactory.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
depthStencilStateFactory::depthStencilStateFactory() :
isValid(false) {
    // empty
}
    
//------------------------------------------------------------------------------
depthStencilStateFactory::~depthStencilStateFactory() {
    o_assert(!this->isValid);
}
    
//------------------------------------------------------------------------------
uint16
depthStencilStateFactory::GetResourceType() const {
    return ResourceType::DepthStencilState;
}
    
//------------------------------------------------------------------------------
void
depthStencilStateFactory::Setup() {
    o_assert(!this->isValid);
    this->isValid = true;
}
    
//------------------------------------------------------------------------------
void
depthStencilStateFactory::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
}
    
//------------------------------------------------------------------------------
bool
depthStencilStateFactory::IsValid() const {
    return this->isValid;
}
    
//------------------------------------------------------------------------------
void
depthStencilStateFactory::SetupResource(depthStencilState& dds) {
    o_assert(this->isValid);
    o_assert(dds.GetState() == Resource::State::Setup);
    
    // we actually don't do anything here, since we're directly
    // using the state info from the setup object
    dds.setState(Resource::State::Valid);
}
    
//------------------------------------------------------------------------------
void
depthStencilStateFactory::DestroyResource(depthStencilState& dds) {
    o_assert(this->isValid);
    o_assert(Resource::State::Valid == dds.GetState());
    dds.clear();
    dds.setState(Resource::State::Setup);
}
    
} // namespace Render
} // namespace Oryol
