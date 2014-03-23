#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glProgramBundleFactory
    @brief private: GL implementation of programBundleFactory
*/
#include "Resource/simpleFactory.h"
#include "Render/Core/programBundle.h"
#include "Render/Core/shaderPool.h"

namespace Oryol {
namespace Render {

class stateWrapper;
    
class glProgramBundleFactory : public Resource::simpleFactory<programBundle> {
public:
    /// constructor
    glProgramBundleFactory();
    /// destructor
    ~glProgramBundleFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(stateWrapper* stWrapper, shaderPool* pool);
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
    bool isValid;
};
    
} // namespace Render
} // namespace Oryol