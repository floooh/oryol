#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glProgramBundleFactory
    @ingroup _priv
    @brief private: GL implementation of programBundleFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/gl/gl_decl.h"

namespace Oryol {
namespace _priv {

class programBundle;

class glProgramBundleFactory {
public:
    /// constructor
    glProgramBundleFactory();
    /// destructor
    ~glProgramBundleFactory();
    
    /// setup the factory
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
    /// compile a GL shader (return 0 if failed)
    GLuint compileShader(ShaderType::Code type, const char* sourceString, int sourceLen) const;

    gfxPointers pointers;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol