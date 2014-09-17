#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::VertexComponent
    @ingroup Gfx
    @brief describes a single component of a vertex
    @see VertexFormat, VertexLayout
*/
#include "Gfx/Core/Enums.h"

namespace Oryol {
    
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
    /// get byte size of component
    int32 ByteSize() const;
    
    VertexAttr::Code Attr;
    VertexFormat::Code Format;
};

//------------------------------------------------------------------------------
inline VertexComponent::VertexComponent() :
Attr(VertexAttr::InvalidVertexAttr),
Format(VertexFormat::InvalidVertexFormat) {
    // empty
}

//------------------------------------------------------------------------------
inline VertexComponent::VertexComponent(VertexAttr::Code att, VertexFormat::Code fmt) :
Attr(att),
Format(fmt) {
    // empty
}

//------------------------------------------------------------------------------
inline void
VertexComponent::Clear() {
    this->Attr = VertexAttr::InvalidVertexAttr;
    this->Format = VertexFormat::InvalidVertexFormat;
}
    
//------------------------------------------------------------------------------
inline bool
VertexComponent::IsValid() const {
    return (VertexAttr::InvalidVertexAttr != this->Attr) && (VertexFormat::InvalidVertexFormat != this->Format);
}

//------------------------------------------------------------------------------
inline int32
VertexComponent::ByteSize() const {
    return VertexFormat::ByteSize(this->Format);
}

} // namespace Oryol
 