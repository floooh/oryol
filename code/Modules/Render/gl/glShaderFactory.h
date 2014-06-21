#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glShaderFactory
    @brief private: GL implementation of class shaderFactory
*/
#include "Render/base/simpleFactory.h"
#include "Render/Core/mesh.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class shader;
    
class glShaderFactory : public simpleFactory<shader, ResourceType::Shader> {
public:
    /// constructor
    glShaderFactory();
    /// destructor
    ~glShaderFactory();
    
    /// setup the factory
    void Setup();
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup shader resource (directly setup shader, no loaders)
    void SetupResource(shader& shd);
    /// destroy the shader
    void DestroyResource(shader& shd);
    
    /// compile a GL shader (return 0 if failed)
    GLuint compileShader(ShaderType::Code type, const Core::String& src) const;
    
private:
    bool isValid;
};
    
} // namespace Render
} // namespace Oryol