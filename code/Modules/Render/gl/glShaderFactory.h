#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glShaderFactory
    @brief private: GL implementation of class shaderFactory
*/
#include "Resource/simpleFactory.h"
#include "Render/Core/mesh.h"

namespace Oryol {
namespace Render {
    
class shader;
    
class glShaderFactory : public Resource::simpleFactory<shader> {
public:
    /// constructor
    glShaderFactory();
    /// destructor
    ~glShaderFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup();
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup shader resource (directly setup shader, no loaders)
    void SetupResource(shader& shd);
    /// setup with input data (directly setup shader, no loaders)
    void SetupResource(shader& shd, const Core::Ptr<IO::Stream>& data);
    /// destroy the shader
    void DestroyResource(shader& shd);
    
private:
    static const uint32 MaxNumShaderSources = 32;

    bool isValid;
};
    
} // namespace Render
} // namespace Oryol