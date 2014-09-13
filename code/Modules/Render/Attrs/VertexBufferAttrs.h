#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::VertexBufferAttrs
    @ingroup Render
    @brief attributes of one vertex buffer
*/
#include "Render/Core/VertexLayout.h"
#include "Render/Core/Enums.h"

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