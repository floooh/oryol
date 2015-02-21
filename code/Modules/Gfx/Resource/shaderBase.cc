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
shaderBase::Clear() {
    this->shaderType = ShaderType::InvalidShaderType;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol