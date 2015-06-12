//------------------------------------------------------------------------------
//  glDrawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glDrawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
glDrawState::Clear() {
    this->glAttrs.Fill(glVertexAttr());
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol