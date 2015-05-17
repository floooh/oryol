//------------------------------------------------------------------------------
//  d3d11ProgramBundleFactory
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11ProgramBundleFactory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11ProgramBundleFactory::d3d11ProgramBundleFactory() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
d3d11ProgramBundleFactory::~d3d11ProgramBundleFactory() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11ProgramBundleFactory::Setup(class renderer* rendr, shaderPool* shdPool, shaderFactory* shdFactory) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11ProgramBundleFactory::Discard() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
bool
d3d11ProgramBundleFactory::IsValid() const {
    o_error("FIXME!\n");
    return false;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11ProgramBundleFactory::SetupResource(programBundle& progBundle) {
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
d3d11ProgramBundleFactory::DestroyResource(programBundle& progBundle) {
    o_error("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol
