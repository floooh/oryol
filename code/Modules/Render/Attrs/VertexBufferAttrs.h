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
    /// constructor
    VertexBufferAttrs();
    
    /// get number of vertices in buffer
    int32 GetNumVertices() const;
    /// get vertex layout
    const VertexLayout& GetVertexLayout() const;
    /// get usage
    Usage::Code GetUsage() const;
    /// get byte size of the vertex buffer
    int32 GetByteSize() const;
    
    /// set number of vertices
    void setNumVertices(int32 num);
    /// set vertex layout
    void setVertexLayout(const VertexLayout& vl);
    /// set usage
    void setUsage(Usage::Code usg);
private:
    int32 numVertices;
    VertexLayout vertexLayout;
    Usage::Code usage;
};

//------------------------------------------------------------------------------
inline int32
VertexBufferAttrs::GetNumVertices() const {
    return this->numVertices;
}

//------------------------------------------------------------------------------
inline const VertexLayout&
VertexBufferAttrs::GetVertexLayout() const {
    return this->vertexLayout;
}

//------------------------------------------------------------------------------
inline Usage::Code
VertexBufferAttrs::GetUsage() const {
    return this->usage;
}

//------------------------------------------------------------------------------
inline int32
VertexBufferAttrs::GetByteSize() const {
    return this->numVertices * this->vertexLayout.ByteSize();
}

} // namespace Render
} // namespace Oryol