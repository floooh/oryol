#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::PrimitiveGroup
    @brief describes a group of primitives to be rendered
    
    A PrimitiveGroup object describes the number and type of a set of
    primitives to be rendered by a draw call. The baseElement and
    numElements either refer to vertices, or indices as element, this 
    depends on the Mesh being rendered (whether it has indices or not).
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {

class PrimitiveGroup {
public:
    /// default constructor
    PrimitiveGroup();
    /// construct for indexed or non-indexed
    PrimitiveGroup(PrimitiveType::Code type, int32 baseElement, int32 numElements);
    
    /// set the primitive type
    void SetPrimitiveType(PrimitiveType::Code type);
    /// get the primitive type
    PrimitiveType::Code GetPrimitiveType() const;
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
};

//------------------------------------------------------------------------------
inline PrimitiveType::Code
PrimitiveGroup::GetPrimitiveType() const {
    return this->type;
}

//------------------------------------------------------------------------------
inline int32
PrimitiveGroup::GetBaseElement() const {
    return this->baseElement;
}

//------------------------------------------------------------------------------
inline int32
PrimitiveGroup::GetNumElements() const {
    return this->numElements;
}

} // namespace Render
} // namespace Oryol
 