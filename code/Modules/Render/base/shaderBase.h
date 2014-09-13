#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderBase
    @ingroup _priv
    @brief shader resource base class
*/
#include "Resource/resourceBase.h"
#include "Render/Setup/ShaderSetup.h"

namespace Oryol {
namespace _priv {

class shaderBase : public Resource::resourceBase<ShaderSetup> {
public:
    /// constructor
    shaderBase();
    
    /// get shader type
    ShaderType::Code GetShaderType() const;
    /// clear the object
    void clear();
    /// set shader type
    void setShaderType(ShaderType::Code type);
    
private:
    ShaderType::Code type;
};
    
} // namespace _priv
} // namespace Oryol