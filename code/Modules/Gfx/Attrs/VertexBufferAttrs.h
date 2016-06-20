#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::VertexBufferAttrs
    @ingroup Gfx
    @brief attributes of one vertex buffer
*/
#include "Gfx/Core/VertexLayout.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
    
struct VertexBufferAttrs {
    /// number of vertices in the vertex buffer
    int NumVertices = 0;
    /// describes the vertex layout of a vertex in the buffer
    VertexLayout Layout;
    /// buffer usage hint
    Usage::Code BufferUsage = Usage::InvalidUsage;
    /// computes the byte size of the contained vertex buffer data
    int ByteSize() const {
        return NumVertices * Layout.ByteSize();
    }
};

} // namespace Oryol