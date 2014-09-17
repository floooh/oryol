#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glShader
    @ingroup _priv
    @brief private: GL implementation of class Shader
*/
#include "Gfx/base/shaderBase.h"
#include "Gfx/gl/gl_decl.h"

namespace Oryol {
namespace _priv {
    
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

} // namespace _priv
} // namespace Oryol