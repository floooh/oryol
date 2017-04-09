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

    uint8_t index = 0;
    uint8_t enabled = 0;
    uint8_t vbIndex = 0;
    uint8_t divisor = 0;
    uint8_t stride = 0;
    uint8_t size = 0;
    uint8_t normalized = 0;
    uint32_t offset = 0;
    GLenum type = 0;
};

} // namespace _priv
} // namespace Oryol
