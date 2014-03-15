//------------------------------------------------------------------------------
//  PrimitiveGroup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "PrimitiveGroup.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
PrimitiveGroup::PrimitiveGroup() :
type(PrimitiveType::InvalidPrimitiveType),
baseElement(0),
numElements(0) {
    // empty
}

//------------------------------------------------------------------------------
PrimitiveGroup::PrimitiveGroup(PrimitiveType::Code type_, int32 base_, int32 num_) :
type(type_),
baseElement(base_),
numElements(num_) {
    // empty
}

//------------------------------------------------------------------------------
void
PrimitiveGroup::SetPrimitiveType(PrimitiveType::Code t) {
    this->type = t;
}

//------------------------------------------------------------------------------
void
PrimitiveGroup::SetBaseElement(int32 base) {
    this->baseElement = base;
}

//------------------------------------------------------------------------------
void
PrimitiveGroup::SetNumElements(int32 num) {
    this->numElements = num;
}

} // namespace Render
} // namespace Oryol