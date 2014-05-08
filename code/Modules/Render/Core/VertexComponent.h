#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::VertexComponent
    @brief describes a single component of a vertex
    @see VertexFormat, VertexLayout
*/
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class VertexComponent {
public:
    /// default constructor
    VertexComponent();
    /// constructor
    VertexComponent(VertexAttr::Code attr, VertexFormat::Code format);
    /// return true if valid (name and format set)
    bool IsValid() const;
    /// clear/invalidate the vertex component
    void Clear();
    /// get vertex attribute
    VertexAttr::Code GetAttr() const;
    /// get vertex format
    VertexFormat::Code GetFormat() const;
    /// get byte size of component
    int32 GetByteSize() const;
    
private:
    VertexAttr::Code attr;
    VertexFormat::Code format;
};

//------------------------------------------------------------------------------
inline bool
VertexComponent::IsValid() const {
    return (VertexAttr::InvalidVertexAttr != this->attr) && (VertexFormat::InvalidVertexFormat != this->format);
}

//------------------------------------------------------------------------------
inline VertexAttr::Code
VertexComponent::GetAttr() const {
    return this->attr;
}

//------------------------------------------------------------------------------
inline VertexFormat::Code
VertexComponent::GetFormat() const {
    return this->format;
}

//------------------------------------------------------------------------------
inline int32
VertexComponent::GetByteSize() const {
    return VertexFormat::ByteSize(this->format);
}

} // namespace Render
} // namespace Oryol
 