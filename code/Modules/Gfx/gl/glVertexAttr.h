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
        divisor(0),
        stride(0),
        size(0),
        normalized(0),
        offset(0),
        type(0) {
        // empty
    };
    /// test for equality
    bool operator==(const glVertexAttr& rhs) const {
        return (this->index == rhs.index) &&
               (this->enabled == rhs.enabled) &&
               (this->vbIndex == rhs.vbIndex) &&
               (this->divisor == rhs.divisor) &&
               (this->stride == rhs.stride) &&
               (this->size == rhs.size) &&
               (this->normalized == rhs.normalized) &&
               (this->offset == rhs.offset) &&
               (this->type == rhs.type);
    };
    /// test for inequality
    bool operator!=(const glVertexAttr& rhs) const {
        return !operator==(rhs);
    };

    uint8 index;
    uint8 enabled;
    uint8 vbIndex;
    uint8 divisor;
    uint8 stride;
    uint8 size;
    uint8 normalized;
    uint32 offset;
    GLenum type;
};

} // namespace _priv
} // namespace Oryol
