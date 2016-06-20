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
        /// construct from name, type and number of array entries
        Component(const StringAtom& name, UniformType::Code type, int num);

        /// return true if the component is valid
        bool IsValid() const;
        /// compute the byte size of the component
        int ByteSize() const;

        StringAtom Name;        ///< the uniform binding name
        UniformType::Code Type; ///< data type of the uniform
        int Num;              ///< >1 if a uniform array
    };

    /// constructor
    UniformBlockLayout();

    /// clear the uniform layout
    void Clear();
    /// return true if the layout is empty
    bool Empty() const;

    /// a layout type hash, this is used for runtime type checking in Gfx::ApplyUniformBlock
    int64_t TypeHash;

    /// add a uniform component to the layout
    UniformBlockLayout& Add(const Component& comp);
    /// add a scalar uniform component to the layout
    UniformBlockLayout& Add(const StringAtom& name, UniformType::Code type);
    /// add an array uniform component to the layout
    UniformBlockLayout& Add(const StringAtom& name, UniformType::Code type, int numElements);
    /// get number of components in the layout
    int NumComponents() const;
    /// get component at index
    const Component& ComponentAt(int componentIndex) const;
    /// get the overall byte size of the uniform layout
    int ByteSize() const;
    /// get byte offset of a component
    int ComponentByteOffset(int componentIndex) const;

private:
    int numComps;
    int byteSize;
    StaticArray<Component, GfxConfig::MaxNumUniformBlockLayoutComponents> comps;
    StaticArray<int, GfxConfig::MaxNumUniformBlockLayoutComponents> byteOffsets;
};

//------------------------------------------------------------------------------
inline
UniformBlockLayout::Component::Component() :
Type(UniformType::InvalidUniformType),
Num(1) {
    // empty
}

//------------------------------------------------------------------------------
inline
UniformBlockLayout::Component::Component(const StringAtom& name, UniformType::Code type) :
Name(name),
Type(type),
Num(1) {
    o_assert_dbg(this->Name.IsValid());
    o_assert_dbg(this->Type < UniformType::NumUniformTypes);
}

//------------------------------------------------------------------------------
inline
UniformBlockLayout::Component::Component(const StringAtom& name, UniformType::Code type, int num) :
Name(name),
Type(type),
Num(num) {
    o_assert_dbg(this->Name.IsValid());
    o_assert_dbg(this->Type < UniformType::NumUniformTypes);
}

//------------------------------------------------------------------------------
inline bool
UniformBlockLayout::Component::IsValid() const {
    return this->Name.IsValid();
}

//------------------------------------------------------------------------------
inline int
UniformBlockLayout::Component::ByteSize() const {
    return UniformType::ByteSize(this->Type, this->Num);
}

//------------------------------------------------------------------------------
inline bool
UniformBlockLayout::Empty() const {
    return 0 == this->numComps;
}

//------------------------------------------------------------------------------
inline int
UniformBlockLayout::NumComponents() const {
    return this->numComps;
}

//------------------------------------------------------------------------------
inline const UniformBlockLayout::Component&
UniformBlockLayout::ComponentAt(int componentIndex) const {
    return this->comps[componentIndex];
}

//------------------------------------------------------------------------------
inline int
UniformBlockLayout::ByteSize() const {
    return this->byteSize;
}

//------------------------------------------------------------------------------
inline int
UniformBlockLayout::ComponentByteOffset(int componentIndex) const {
    return this->byteOffsets[componentIndex];
}

} // namespace Oryol
