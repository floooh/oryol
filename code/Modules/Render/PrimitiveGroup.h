#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::PrimitiveGroup
    @brief describes a group of primitives to be rendered
    
    A PrimitiveGroup object describes the number and type of a set of
    primitives to be rendered by a draw call. PrimitiveGroups can be
    setup for indexed and non-indexed rendering. If indexed rendering
    is used baseElement and numElements refer to indices living
    in an index buffer, otherwise baseElement and numElements refer
    to vertices living in a vertex buffer.
*/
#include "Core/Types.h"
#include "Render/PrimitiveType.h"

namespace Oryol {
namespace Render {

class PrimitiveGroup {
public:
    /// default constructor
    PrimitiveGroup();
    /// construct for indexed or non-indexed
    PrimitiveGroup(PrimitiveType::Code type, bool indexed, int32 baseElement, int32 numElements);
    
    /// set the primitive type
    void SetPrimitiveType(PrimitiveType::Code type);
    /// get the primitive type
    PrimitiveType::Code GetPrimitiveType() const;
    /// set the indexed flag
    void SetIndexedFlag(bool b);
    /// return true if using indexed rendering
    bool IsIndexed() const;
    /// set the base element index (either a vertex-index, or an index-index)
    void SetBaseElement(int32 index);
    /// get the base element
    int32 GetBaseElement() const;
    /// set num elements in primitive group
    void SetNumElements(int32 num);
    /// get num elements
    int32 GetNumElements() const;
    
private:
    PrimitiveType::Code type;
    int32 baseElement;
    int32 numElements;
    bool indexed;
};

//------------------------------------------------------------------------------
inline PrimitiveType::Code
PrimitiveGroup::GetPrimitiveType() const {
    return this->type;
}

//------------------------------------------------------------------------------
inline bool
PrimitiveGroup::IsIndexed() const {
    return this->indexed;
}

//------------------------------------------------------------------------------
inline int32
PrimitiveGroup::GetBaseElement() const {
    return this->baseElement;
}

//------------------------------------------------------------------------------
inline int32
PrimitiveGroup::GetNumELements() const {
    return this->numELements;
}

} // namespace Render
} // namespace Oryol
 