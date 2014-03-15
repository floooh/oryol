//------------------------------------------------------------------------------
//  VertexComponent.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexComponent.h"

namespace Oryol {
namespace Render {

using namespace Core;
    
//------------------------------------------------------------------------------
VertexComponent::VertexComponent() :
format(VertexFormat::InvalidVertexFormat) {
    // empty
}

//------------------------------------------------------------------------------
VertexComponent::VertexComponent(const StringAtom& name, VertexFormat::Code fmt) :
attrName(name),
format(fmt) {
    // empty
}

//------------------------------------------------------------------------------
void
VertexComponent::Clear() {
    this->attrName.Clear();
    this->format = VertexFormat::InvalidVertexFormat;
}

} // namespace Render
} // namespace Oryol