#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::VertexLayout
    @ingroup Gfx
    @brief describes the data layout of a vertex in a vertex buffer
*/
#include "Core/Assertion.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/GfxConfig.h"

namespace Oryol {
    
class VertexLayout {
public:
    /// a component in the vertex layout
    #pragma pack(push,1)
    class Component {
    public:
        /// default constructor
        Component();
        /// construct from vertex attr and format
        Component(VertexAttr::Code attr, VertexFormat::Code format);
        /// return true if valid (attr and format set)
        bool IsValid() const;
        /// clear the component (unset attr and format)
        void Clear();
        /// get byte size of component
        int ByteSize() const;

        VertexAttr::Code Attr;
        VertexFormat::Code Format;
    };
    #pragma pack(pop)

    /// the vertex step function, used for instancing, default is 'PerVertex'
    VertexStepFunction::Code StepFunction;
    /// the vertex step rate, used for instancing
    uint8_t StepRate;

    /// constructor
    VertexLayout();
    /// clear the vertex layout, chainable
    VertexLayout& Clear();
    /// return true if layout is empty
    bool Empty() const;

    /// add a component
    VertexLayout& Add(const Component& comp);
    /// add component by name and format
    VertexLayout& Add(VertexAttr::Code attr, VertexFormat::Code format);
    /// enable layout for instancing, set StepFunction to PerInstance and StepRate to 1
    VertexLayout& EnableInstancing();

    /// get number of components
    int NumComponents() const;
    /// get component at index
    const Component& ComponentAt(int index) const;
    /// get component index by vertex attribute, return InvalidIndex if layout doesn't include attr
    int ComponentIndexByVertexAttr(VertexAttr::Code attr) const;
    /// get byte size of vertex (aka stride)
    int ByteSize() const;
    /// get byte offset of a component
    int ComponentByteOffset(int componentIndex) const;
    /// test if the layout contains a specific vertex attribute
    bool Contains(VertexAttr::Code attr) const;

private:
    StaticArray<Component, GfxConfig::MaxNumVertexLayoutComponents> comps;
    StaticArray<uint8_t, GfxConfig::MaxNumVertexLayoutComponents> byteOffsets;
    StaticArray<int8_t, VertexAttr::NumVertexAttrs> attrCompIndices;  // maps vertex attributes to component indices
    int8_t numComps;
    uint8_t byteSize;
};

//------------------------------------------------------------------------------
inline
VertexLayout::Component::Component() :
Attr(VertexAttr::InvalidVertexAttr),
Format(VertexFormat::InvalidVertexFormat) {
    // empty
}

//------------------------------------------------------------------------------
inline
VertexLayout::Component::Component(VertexAttr::Code attr, VertexFormat::Code fmt) :
Attr(attr),
Format(fmt) {
    // empty
}

//------------------------------------------------------------------------------
inline bool
VertexLayout::Component::IsValid() const {
    return (VertexAttr::InvalidVertexAttr != this->Attr);
}

//------------------------------------------------------------------------------
inline void
VertexLayout::Component::Clear() {
    this->Attr = VertexAttr::InvalidVertexAttr;
    this->Format = VertexFormat::InvalidVertexFormat;
}

//------------------------------------------------------------------------------
inline int
VertexLayout::Component::ByteSize() const {
    return VertexFormat::ByteSize(this->Format);
}

//------------------------------------------------------------------------------
inline int
VertexLayout::ComponentIndexByVertexAttr(VertexAttr::Code attr) const {
    return this->attrCompIndices[attr];
}

//------------------------------------------------------------------------------
inline bool
VertexLayout::Contains(VertexAttr::Code attr) const {
    return InvalidIndex != this->ComponentIndexByVertexAttr(attr);
}

//------------------------------------------------------------------------------
inline bool
VertexLayout::Empty() const {
    return 0 == this->numComps;
}

//------------------------------------------------------------------------------
inline int
VertexLayout::NumComponents() const {
    return this->numComps;
}

//------------------------------------------------------------------------------
inline const class VertexLayout::Component&
VertexLayout::ComponentAt(int index) const {
    return this->comps[index];
}

//------------------------------------------------------------------------------
inline int
VertexLayout::ByteSize() const {
    return this->byteSize;
}

//------------------------------------------------------------------------------
inline int
VertexLayout::ComponentByteOffset(int index) const {
    return this->byteOffsets[index];
}

} // namespace Oryol
