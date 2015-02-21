#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glShader
    @ingroup _priv
    @brief private: GL implementation of class Shader
*/
#include "Gfx/Resource/shaderBase.h"
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
    void Clear();
    /// GL shader id
    GLuint glShd;
};

} // namespace _priv
} // namespace Oryol