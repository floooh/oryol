#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlShaderFactory
    @ingroup _priv
    @brief Metal implementation of shaderFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class shader;

class mtlShaderFactory {
public:
    /// destructor
    ~mtlShaderFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    
    /// setup shader resource
    ResourceState::Code SetupResource(shader& shd);
    /// destroy the shader
    void DestroyResource(shader& shd);

private:
    gfxPointers pointers;
    bool isValid = false;
};
    
} // namespace _priv
} // namespace Oryol
