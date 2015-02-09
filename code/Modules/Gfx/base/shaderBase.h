#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderBase
    @ingroup _priv
    @brief shader resource base class
*/
#include "Resource/resourceBase.h"
#include "Gfx/Setup/ShaderSetup.h"

namespace Oryol {
namespace _priv {

class shaderBase : public resourceBase<ShaderSetup> {
public:
    /// constructor
    shaderBase();
    
    /// shader type
    ShaderType::Code shaderType;
    /// clear the object
    void clear();
};
    
} // namespace _priv
} // namespace Oryol