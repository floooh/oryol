#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glVertexAttr
    @brief private: hold GL specific vertex attr values
*/
#include "Core/Types.h"
#include "Gfx/gl/gl_decl.h"
#include <cstring>

namespace Oryol {
namespace _priv {
    
class glVertexAttr {
public:
    /// constructor
    glVertexAttr() :
        index(0),
        enabled(0),
        vbIndex(0),
        streaming(false),
        divisor(0),
        stride(0),
        offset(0),
        size(0),
        type(0),
        normalized(0) {
        // empty
    };
    /// test for equality
    bool operator==(const glVertexAttr& rhs) const {
        return 0 == std::memcmp(this, &rhs, sizeof(glVertexAttr));
    };
    /// test for inequality
    bool operator!=(const glVertexAttr& rhs) const {
        return 0 != std::memcmp(this, &rhs, sizeof(glVertexAttr));
    };
    
    GLuint index;
    GLboolean enabled;
    int32 vbIndex;
    bool streaming;
    GLuint divisor;
    GLsizei stride;
    GLuint offset;
    GLint size;
    GLenum type;
    GLboolean normalized;
};
    
} // namespace _priv
} // namespace Oryol
