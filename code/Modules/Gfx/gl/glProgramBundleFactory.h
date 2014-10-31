#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glProgramBundleFactory
    @ingroup _priv
    @brief private: GL implementation of programBundleFactory
*/
#include "Gfx/base/renderSimpleFactory.h"
#include "Gfx/Core/programBundle.h"

namespace Oryol {
namespace _priv {

class renderer;
class shaderPool;
class shaderFactory;
    
class glProgramBundleFactory : public renderSimpleFactory<programBundle, ResourceType::ProgramBundle> {
public:
    /// constructor
    glProgramBundleFactory();
    /// destructor
    ~glProgramBundleFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(class renderer* rendr, shaderPool* shdPool, shaderFactory* shdFactory);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    
    /// setup programBundle resource
    void SetupResource(programBundle& progBundle);
    /// destroy the shader
    void DestroyResource(programBundle& progBundle);

private:
    class renderer* renderer;
    shaderPool* shdPool;
    shaderFactory* shdFactory;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol