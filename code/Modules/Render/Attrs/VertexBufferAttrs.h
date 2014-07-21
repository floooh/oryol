#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::VertexBufferAttrs
    @brief attributes of one vertex buffer
*/
#include "Render/Core/VertexLayout.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class VertexBufferAttrs {
public:
    /// number of vertices in buffer
    int32 NumVertices{0};
    /// vertex layout
    VertexLayout Layout;
    /// usage
    Usage::Code BufferUsage{Usage::InvalidUsage};
    /// byte size of vertex buffer
    int32 ByteSize() const {
        return NumVertices * Layout.ByteSize();
    }
};

} // namespace Render
} // namespace Oryol