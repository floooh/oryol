#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlProgramBundleFactory
    @ingroup _priv
    @brief Metal implementation of programBundleFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class programBundle;

class mtlProgramBundleFactory {
public:
    /// constructor
    mtlProgramBundleFactory();
    /// destructor
    ~mtlProgramBundleFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    
    /// setup programBundle resource
    ResourceState::Code SetupResource(programBundle& progBundle);
    /// destroy the shader
    void DestroyResource(programBundle& progBundle);

private:
    gfxPointers pointers;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol
