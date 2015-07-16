#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::UniformLayout
    @ingroup Gfx
    @brief describes the layout of an uniform block
*/
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/GfxConfig.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {

class UniformLayout {
public:
    /// a UniformLayout component describes a single uniform in the uniform block
    class Component {
    public:
        /// default constructor
        Component();
        /// construct from name, type and number
        Component(const StringAtom& name, UniformType::Code type, uint8 num, int8 bindSlotIndex);

        /// return true if the component is valid
        bool IsValid() const;
        /// clear the component
        void Clear();
        /// compute the byte size of the component
        int32 ByteSize() const;

        StringAtom Name;        ///< the uniform binding name
        UniformType::Code Type; ///< data type of the uniform
        uint8 Num;              ///< number of uniforms (if >1 it is an array)
        int8 BindSlotIndex;     ///< bind slot index (used for texture uniforms)
    };

    /// constructor
    UniformLayout();

    /// clear the uniform layout
    void Clear();
    /// return true if the layout is empty
    bool Empty() const;

    /// a layout type hash, this is used for runtime type checking in Gfx::ApplyUniformBlock
    int64 TypeHash;

    /// add a uniform component to the layout
    UniformLayout& Add(const Component& comp);
    /// add a uniform component to the layout
    UniformLayout& Add(const StringAtom& name, UniformType::Code type, uint8 num, int8 bindSlotIndex);
    /// get number of components in the layout
    int32 NumComponents() const;
    /// get component at index
    const Component& ComponentAt(int32 componentIndex) const;
    /// get the overall byte size of the uniform layout
    int32 ByteSize() const;
    /// return the byte size without texture components
    int32 ByteSizeWithoutTextures() const;
    /// get byte offset of a component
    int32 ComponentByteOffset(int32 componentIndex) const;

private:
    int32 numComps;
    int32 byteSize;
    StaticArray<Component, GfxConfig::MaxNumUniformLayoutComponents> comps;
    StaticArray<int32, GfxConfig::MaxNumUniformLayoutComponents> byteOffsets;
};

//------------------------------------------------------------------------------
inline
UniformLayout::Component::Component() :
Type(UniformType::InvalidUniformType),
Num(0) {
    // empty
}

//------------------------------------------------------------------------------
inline
UniformLayout::Component::Component(const StringAtom& name, UniformType::Code type, uint8 num, int8 bindSlotIndex) :
Name(name),
Type(type),
Num(num),
BindSlotIndex(bindSlotIndex) {
    o_assert_dbg(this->Name.IsValid());
    o_assert_dbg(this->Type < UniformType::NumUniformTypes);
    o_assert_dbg(this->Num > 0);
}

//------------------------------------------------------------------------------
inline bool
UniformLayout::Component::IsValid() const {
    return this->Name.IsValid();
}

//------------------------------------------------------------------------------
inline void
UniformLayout::Component::Clear() {
    this->Name.Clear();
    this->Type = UniformType::InvalidUniformType;
    this->Num = 0;
    this->BindSlotIndex = InvalidIndex;
}

//------------------------------------------------------------------------------
inline int32
UniformLayout::Component::ByteSize() const {
    return UniformType::ByteSize(this->Type);
}

//------------------------------------------------------------------------------
inline bool
UniformLayout::Empty() const {
    return 0 == this->numComps;
}

//------------------------------------------------------------------------------
inline int32
UniformLayout::NumComponents() const {
    return this->numComps;
}

//------------------------------------------------------------------------------
inline const UniformLayout::Component&
UniformLayout::ComponentAt(int32 componentIndex) const {
    return this->comps[componentIndex];
}

//------------------------------------------------------------------------------
inline int32
UniformLayout::ByteSize() const {
    return this->byteSize;
}

//------------------------------------------------------------------------------
inline int32
UniformLayout::ComponentByteOffset(int32 componentIndex) const {
    return this->byteOffsets[componentIndex];
}

} // namespace Oryol
