#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::PrimitiveGroup
    @ingroup Gfx
    @brief describes a group of primitives to be rendered
    
    A PrimitiveGroup object describes the number and type of a set of
    primitives to be rendered by a draw call. The baseElement and
    numElements either refer to vertices, or indices as element, this 
    depends on the Mesh being rendered (whether it has indices or not).
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {

class PrimitiveGroup {
public:
    int32 BaseElement;
    int32 NumElements;

    /// default constructor
    PrimitiveGroup() :
        BaseElement(0),
        NumElements(0) {
        // empty
    }
    /// construct for indexed or non-indexed
    PrimitiveGroup(int32 baseElement, int32 numElements) :
        BaseElement(baseElement),
        NumElements(numElements) {
        // empty
    }
};

} // namespace Oryol
 