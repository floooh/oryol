//------------------------------------------------------------------------------
//  glShader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glShader.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
glShader::glShader() :
glShd(0) {
    // empty
}

//------------------------------------------------------------------------------
glShader::~glShader() {
    o_assert(0 == this->glShd);
}

//------------------------------------------------------------------------------
void
glShader::clear() {
    this->glShd = 0;
    shaderBase::clear();
}

//------------------------------------------------------------------------------
void
glShader::glSetShader(GLuint shd) {
    o_assert(0 == this->glShd);
    this->glShd = shd;
}

} // namespace Render
} // namespace Oryol