#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::VertexComponent
    @brief describes a single component of a vertex
    @see VertexFormat, VertexLayout
*/
#include "Render/Types/VertexFormat.h"
#include "Core/String/StringAtom.h"

namespace Oryol {
namespace Render {
    
class VertexComponent {
public:
    /// default constructor
    VertexComponent();
    /// constructor
    VertexComponent(const Core::StringAtom& attrName, VertexFormat::Code format);
    /// return true if valid (name and format set)
    bool IsValid() const;
    /// clear/invalidate the vertex component
    void Clear();
    /// get vertex attribute name
    const Core::StringAtom& GetAttrName() const;
    /// get vertex format
    VertexFormat::Code GetFormat() const;
    /// get byte size of component
    int32 GetByteSize() const;
    
private:
    Core::StringAtom attrName;
    VertexFormat::Code format;
};

//------------------------------------------------------------------------------
inline bool
VertexComponent::IsValid() const {
    return this->attrName.IsValid() && (VertexFormat::InvalidVertexFormat != this->format);
}

//------------------------------------------------------------------------------
inline const Core::StringAtom&
VertexComponent::GetAttrName() const {
    return this->attrName;
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
 