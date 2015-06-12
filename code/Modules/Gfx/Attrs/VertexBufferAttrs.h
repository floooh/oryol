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
    int32 NumVertices = 0;
    /// describes the vertex layout of a vertex in the buffer
    VertexLayout Layout;
    /// buffer usage hint
    Usage::Code BufferUsage = Usage::InvalidUsage;
    /// vertex data step function (per-vertex or per-instance)
    VertexStepFunction::Code StepFunction = VertexStepFunction::PerVertex;
    /// vertex data step rate
    int32 StepRate = 1;
    /// computes the byte size of the contained vertex buffer data
    int32 ByteSize() const {
        return NumVertices * Layout.ByteSize();
    }
};

} // namespace Oryol