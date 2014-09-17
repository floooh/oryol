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
    int32 NumVertices{0};
    VertexLayout Layout;
    Usage::Code BufferUsage{Usage::InvalidUsage};
    int32 ByteSize() const {
        return NumVertices * Layout.ByteSize();
    }
};

} // namespace Oryol