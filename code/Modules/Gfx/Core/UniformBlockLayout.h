#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::UniformBlockLayout
    @ingroup Gfx
    @brief describes the layout of an uniform block

    A UniformBlockLayout describes the names and types of a group of
    related shader uniforms.
*/
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/GfxConfig.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {

class UniformBlockLayout {
public:
    /// a UniformBlockLayout component describes a single uniform in the uniform block
    class Component {
    public:
        /// default constructor
        Component();
        /// construct from name, type and number
        Component(const StringAtom& name, UniformType::Code type);

        /// return true if the component is valid
        bool IsValid() const;
        /// compute the byte size of the component
        int32 ByteSize() const;

        StringAtom Name;        ///< the uniform binding name
        UniformType::Code Type; ///< data type of the uniform
    };

    /// constructor
    UniformBlockLayout();

    /// clear the uniform layout
    void Clear();
    /// return true if the layout is empty
    bool Empty() const;

    /// a layout type hash, this is used for runtime type checking in Gfx::ApplyUniformBlock
    int64 TypeHash;

    /// add a uniform component to the layout
    UniformBlockLayout& Add(const Component& comp);
    /// add a uniform component to the layout
    UniformBlockLayout& Add(const StringAtom& name, UniformType::Code type);
    /// get number of components in the layout
    int32 NumComponents() const;
    /// get component at index
    const Component& ComponentAt(int32 componentIndex) const;
    /// get the overall byte size of the uniform layout
    int32 ByteSize() const;
    /// get byte offset of a component
    int32 ComponentByteOffset(int32 componentIndex) const;

private:
    int32 numComps;
    int32 byteSize;
    StaticArray<Component, GfxConfig::MaxNumUniformBlockLayoutComponents> comps;
    StaticArray<int32, GfxConfig::MaxNumUniformBlockLayoutComponents> byteOffsets;
};

//------------------------------------------------------------------------------
inline
UniformBlockLayout::Component::Component() :
Type(UniformType::InvalidUniformType) {
    // empty
}

//------------------------------------------------------------------------------
inline
UniformBlockLayout::Component::Component(const StringAtom& name, UniformType::Code type) :
Name(name),
Type(type) {
    o_assert_dbg(this->Name.IsValid());
    o_assert_dbg(this->Type < UniformType::NumUniformTypes);
}

//------------------------------------------------------------------------------
inline bool
UniformBlockLayout::Component::IsValid() const {
    return this->Name.IsValid();
}

//------------------------------------------------------------------------------
inline int32
UniformBlockLayout::Component::ByteSize() const {
    return UniformType::ByteSize(this->Type);
}

//------------------------------------------------------------------------------
inline bool
UniformBlockLayout::Empty() const {
    return 0 == this->numComps;
}

//------------------------------------------------------------------------------
inline int32
UniformBlockLayout::NumComponents() const {
    return this->numComps;
}

//------------------------------------------------------------------------------
inline const UniformBlockLayout::Component&
UniformBlockLayout::ComponentAt(int32 componentIndex) const {
    return this->comps[componentIndex];
}

//------------------------------------------------------------------------------
inline int32
UniformBlockLayout::ByteSize() const {
    return this->byteSize;
}

//------------------------------------------------------------------------------
inline int32
UniformBlockLayout::ComponentByteOffset(int32 componentIndex) const {
    return this->byteOffsets[componentIndex];
}

} // namespace Oryol
