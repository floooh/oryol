#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glShader
    @brief private: GL implementation of class Shader
*/
#include "Render/base/shaderBase.h"
#include "Render/gl/gl_decl.h"

namespace Oryol {
namespace Render {
    
class glShader : public shaderBase {
public:
    /// constructor
    glShader();
    /// destructor
    ~glShader();
    
    /// clear the object
    void clear();

    /// set the GL shader object
    void glSetShader(GLuint shd);
    /// get the GL shader object
    GLuint glGetShader() const;
    
protected:
    GLuint glShd;
};

//------------------------------------------------------------------------------
inline GLuint
glShader::glGetShader() const {
    return this->glShd;
}

} // namespace Render
} // namespace Oryol