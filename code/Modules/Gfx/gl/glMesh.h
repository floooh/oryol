#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glMesh
    @ingroup _priv
    @brief GL implementation of mesh
*/
#include "Gfx/Resource/meshBase.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/gl/gl_decl.h"

namespace Oryol {
namespace _priv {
    
class glMesh : public meshBase {
public:
    /// constructor
    glMesh();
    /// destructor
    ~glMesh();

    /// clear the object (called from meshFactory::DestroyResource())
    void Clear();

    static const int32 MaxNumSlots = 2;
    struct buffer {
        buffer() : updateFrameIndex(-1), numSlots(1), activeSlot(0) {
            this->glBuffers.Fill(0);
        }
        int32 updateFrameIndex;
        uint8 numSlots;
        uint8 activeSlot;
        StaticArray<GLuint, MaxNumSlots> glBuffers;
    };
    static const int vb = 0;
    static const int ib = 1;
    StaticArray<buffer, 2> buffers;
    GLenum glPrimType;
};

} // namespace _priv
} // namespace Oryol
