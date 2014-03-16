//------------------------------------------------------------------------------
//  VertexComponent.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexComponent.h"

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
VertexComponent::VertexComponent() :
attr(VertexAttr::InvalidVertexAttr),
format(VertexFormat::InvalidVertexFormat) {
    // empty
}

//------------------------------------------------------------------------------
VertexComponent::VertexComponent(VertexAttr::Code att, VertexFormat::Code fmt) :
attr(att),
format(fmt) {
    // empty
}

//------------------------------------------------------------------------------
void
VertexComponent::Clear() {
    this->attr = VertexAttr::InvalidVertexAttr;
    this->format = VertexFormat::InvalidVertexFormat;
}

} // namespace Render
} // namespace Oryol