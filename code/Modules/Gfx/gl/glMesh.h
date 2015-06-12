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

    /// GL index buffer
    GLuint glIndexBuffer;
    /// max number of vertex buffer slots (currently used for double-buffering dynamic vertex data)
    static const int32 MaxNumSlots = 2;
    /// number of vertex buffer slots
    uint8 numVertexBufferSlots;
    /// active vertex buffer slot
    uint8 activeVertexBufferSlot;
    /// GL vertex buffers
    StaticArray<GLuint, MaxNumSlots> glVertexBuffers;
};

} // namespace _priv
} // namespace Oryol
