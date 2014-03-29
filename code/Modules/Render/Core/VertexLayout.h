#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::VertexLayout
    @brief describes the data layout of a vertex in a vertex buffer
    @see VertexComponent
*/
#include "Render/Core/VertexComponent.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Render {
    
class VertexLayout {
public:
    /// constructor
    VertexLayout();
    /// clear the object
    void Clear();
    /// return true if layout is empty
    bool Empty() const;
    /// add a component
    void Add(const VertexComponent& comp);
    /// add component by name and format
    void Add(VertexAttr::Code attr, VertexFormat::Code format);
    /// get number of components
    int32 GetNumComponents() const;
    /// read-only get component at index
    const VertexComponent& GetComponent(int32 index) const;
    /// get component index by vertex attribute, return InvalidIndex if layout doesn't include attr
    int32 GetComponentIndexByVertexAttr(VertexAttr::Code attr) const;
    /// get byte size of vertex
    int32 GetByteSize() const;
    /// get byte offset of a component
    int32 GetComponentByteOffset(int32 componentIndex) const;
    /// test if the layout contains a specific vertex attribute
    bool Contains(VertexAttr::Code attr) const;
    
private:
    /// maximum number of components in layout
    static const int32 MaxNumVertexComponents = 16;

    VertexComponent comps[MaxNumVertexComponents];
    int32 byteOffsets[MaxNumVertexComponents];
    int32 numComps;
    int32 byteSize;
    int32 attrCompIndices[VertexAttr::NumVertexAttrs];  // maps vertex attributes to component indices
};

//------------------------------------------------------------------------------
inline int32
VertexLayout::GetComponentIndexByVertexAttr(VertexAttr::Code attr) const {
    o_assert_dbg(attr < VertexAttr::NumVertexAttrs);
    return this->attrCompIndices[attr];
}

//------------------------------------------------------------------------------
inline bool
VertexLayout::Contains(VertexAttr::Code attr) const {
    return InvalidIndex != this->GetComponentIndexByVertexAttr(attr);
}

//------------------------------------------------------------------------------
inline bool
VertexLayout::Empty() const {
    return 0 == this->numComps;
}

//------------------------------------------------------------------------------
inline int32
VertexLayout::GetNumComponents() const {
    return this->numComps;
}

//------------------------------------------------------------------------------
inline const VertexComponent&
VertexLayout::GetComponent(int32 index) const {
    o_assert((index >= 0) && (index < MaxNumVertexComponents));
    return this->comps[index];
}

//------------------------------------------------------------------------------
inline int32
VertexLayout::GetByteSize() const {
    return this->byteSize;
}

//------------------------------------------------------------------------------
inline int32
VertexLayout::GetComponentByteOffset(int32 index) const {
    o_assert_range_dbg(index, MaxNumVertexComponents);
    return this->byteOffsets[index];
}

} // namespace Render
} // namespace Oryol