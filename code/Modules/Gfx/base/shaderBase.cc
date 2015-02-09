//------------------------------------------------------------------------------
//  shaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "shaderBase.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
shaderBase::shaderBase() :
shaderType(ShaderType::InvalidShaderType) {
    // empty
}

//------------------------------------------------------------------------------
void
shaderBase::clear() {
    this->shaderType = ShaderType::InvalidShaderType;
    resourceBase::clear();    
}

} // namespace _priv
} // namespace Oryol