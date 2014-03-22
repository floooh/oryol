#pragma once
//------------------------------------------------------------------------------
/**
    @class shaderBase
    @brief private: shader resource base class
*/
#include "Resource/resourceBase.h"
#include "Render/Setup/ShaderSetup.h"

namespace Oryol {
namespace Render {

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
    
} // namespace Render
} // namespace Oryol