//------------------------------------------------------------------------------
//  shaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "shaderBase.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
shaderBase::shaderBase() :
type(ShaderType::InvalidShaderType) {
    // empty
}

//------------------------------------------------------------------------------
ShaderType::Code
shaderBase::GetShaderType() const {
    return this->type;
}

//------------------------------------------------------------------------------
void
shaderBase::clear() {
    this->type = ShaderType::InvalidShaderType;
    resourceBase::clear();    
}

//------------------------------------------------------------------------------
void
shaderBase::setShaderType(ShaderType::Code t) {
    this->type = t;
}

} // namespace _priv
} // namespace Oryol