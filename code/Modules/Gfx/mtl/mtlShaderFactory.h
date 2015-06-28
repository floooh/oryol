#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlShaderFactory
    @ingroup _priv
    @brief Metal implementation of shaderFactory
*/
#include "Resource/ResourceState.h"

namespace Oryol {
namespace _priv {
    
class shader;
class renderer;
    
class mtlShaderFactory {
public:
    /// constructor
    mtlShaderFactory();
    /// destructor
    ~mtlShaderFactory();
    
    /// setup the factory
    void Setup(renderer* rendr);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup shader resource (directly setup shader, no loaders)
    ResourceState::Code SetupResource(shader& shd);
    /// destroy the shader
    void DestroyResource(shader& shd);
    
private:
    bool isValid;
};

} // namespace _priv
} // namespace Oryol
