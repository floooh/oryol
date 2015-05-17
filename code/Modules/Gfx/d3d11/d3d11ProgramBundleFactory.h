#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11ProgramBundleFactory
    @ingroup _priv
    @brief D3D11 implementation of programBundleFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Resource/programBundle.h"

namespace Oryol {
namespace _priv {

class renderer;
class shaderPool;
class shaderFactory;
    
class d3d11ProgramBundleFactory {
public:
    /// constructor
    d3d11ProgramBundleFactory();
    /// destructor
    ~d3d11ProgramBundleFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(class renderer* rendr, shaderPool* shdPool, shaderFactory* shdFactory);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    
    /// setup programBundle resource
    ResourceState::Code SetupResource(programBundle& progBundle);
    /// destroy the shader
    void DestroyResource(programBundle& progBundle);
};
    
} // namespace _priv
} // namespace Oryol
