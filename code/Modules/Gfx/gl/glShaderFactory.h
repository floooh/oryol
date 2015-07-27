#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glShaderFactory
    @ingroup _priv
    @brief private: GL implementation of shaderFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/gl/gl_decl.h"

namespace Oryol {
namespace _priv {

class shader;

class glShaderFactory {
public:
    /// constructor
    glShaderFactory();
    /// destructor
    ~glShaderFactory();
    
    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    
    /// setup resource
    ResourceState::Code SetupResource(shader& shd);
    /// destroy resource
    void DestroyResource(shader& shd);

private:
    /// compile a GL shader (return 0 if failed)
    GLuint compileShader(ShaderType::Code type, const char* sourceString, int sourceLen) const;

    gfxPointers pointers;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol