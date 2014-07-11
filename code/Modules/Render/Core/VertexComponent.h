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
    bool Valid() const;
    /// clear/invalidate the vertex component
    void Clear();
    /// get vertex attribute
    VertexAttr::Code Attr() const;
    /// get vertex format
    VertexFormat::Code Format() const;
    /// get byte size of component
    int32 ByteSize() const;
    
private:
    VertexAttr::Code attr;
    VertexFormat::Code format;
};

//------------------------------------------------------------------------------
inline bool
VertexComponent::Valid() const {
    return (VertexAttr::InvalidVertexAttr != this->attr) && (VertexFormat::InvalidVertexFormat != this->format);
}

//------------------------------------------------------------------------------
inline VertexAttr::Code
VertexComponent::Attr() const {
    return this->attr;
}

//------------------------------------------------------------------------------
inline VertexFormat::Code
VertexComponent::Format() const {
    return this->format;
}

//------------------------------------------------------------------------------
inline int32
VertexComponent::ByteSize() const {
    return VertexFormat::ByteSize(this->format);
}

} // namespace Render
} // namespace Oryol
 