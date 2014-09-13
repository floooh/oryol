#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glProgramBundleFactory
    @ingroup _priv
    @brief private: GL implementation of programBundleFactory
*/
#include "Render/base/renderSimpleFactory.h"
#include "Render/Core/programBundle.h"

namespace Oryol {
namespace _priv {

class stateWrapper;
class shaderPool;
class shaderFactory;
    
class glProgramBundleFactory : public renderSimpleFactory<programBundle, ResourceType::ProgramBundle> {
public:
    /// constructor
    glProgramBundleFactory();
    /// destructor
    ~glProgramBundleFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(stateWrapper* stWrapper, shaderPool* shdPool, shaderFactory* shdFactory);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    
    /// setup programBundle resource
    void SetupResource(programBundle& progBundle);
    /// destroy the shader
    void DestroyResource(programBundle& progBundle);

private:
    stateWrapper* glStateWrapper;
    shaderPool* shdPool;
    shaderFactory* shdFactory;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol