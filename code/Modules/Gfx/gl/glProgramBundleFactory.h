#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glProgramBundleFactory
    @ingroup _priv
    @brief private: GL implementation of programBundleFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/gl/gl_decl.h"

namespace Oryol {
namespace _priv {

class renderer;
class shaderPool;
class shaderFactory;
class programBundle;

class glProgramBundleFactory {
public:
    /// constructor
    glProgramBundleFactory();
    /// destructor
    ~glProgramBundleFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(class renderer* rendr);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    
    /// setup programBundle resource
    ResourceState::Code SetupResource(programBundle& progBundle);
    /// destroy the shader
    void DestroyResource(programBundle& progBundle);

private:
    /// compile a GL shader (return 0 if failed)
    GLuint compileShader(ShaderType::Code type, const char* sourceString, int sourceLen) const;

    class renderer* renderer;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol