//------------------------------------------------------------------------------
//  VertexComponent.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexComponent.h"
#include "Core/Assert.h"

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
    o_assert(this->attr < VertexAttr::NumVertexAttrs);
    o_assert(this->format < VertexFormat::NumVertexFormats);
}

//------------------------------------------------------------------------------
void
VertexComponent::Clear() {
    this->attr = VertexAttr::InvalidVertexAttr;
    this->format = VertexFormat::InvalidVertexFormat;
}

} // namespace Render
} // namespace Oryol