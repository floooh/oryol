#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glProgramBundleFactory
    @brief private: GL implementation of programBundleFactory
*/
#include "Render/base/simpleFactory.h"
#include "Render/Core/programBundle.h"

namespace Oryol {
namespace Render {

class stateWrapper;
class shaderPool;
class shaderFactory;
    
class glProgramBundleFactory : public simpleFactory<programBundle, ResourceType::ProgramBundle> {
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
    
} // namespace Render
} // namespace Oryol