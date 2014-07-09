#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glVertexAttr
    @brief private: hold GL specific vertex attr values
*/
#include "Core/Types.h"
#include "Render/gl/gl_decl.h"

namespace Oryol {
namespace Render {
    
class glVertexAttr {
public:
    /// constructor
    glVertexAttr() :
    index(0),
    size(0),
    stride(0),
    offset(0),
    type(0),
    enabled(0),
    normalized(0),
    vertexBuffer(0),
    divisor(0) {
        // empty
    };
    
    GLuint index;
    GLint size;
    GLsizei stride;
    GLuint offset;
    GLenum type;
    GLboolean enabled;
    GLboolean normalized;
    GLuint vertexBuffer;
    GLuint divisor;
};
    
} // namespace Render
} // namespace Oryol