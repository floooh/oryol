//------------------------------------------------------------------------------
//  glDrawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glDrawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glDrawState::glDrawState() :
shdProgIndex(InvalidIndex) {
    // empty
}

//------------------------------------------------------------------------------
void
glDrawState::Clear() {
    this->glAttrs.Fill(glVertexAttr());
    this->shdProgIndex = InvalidIndex;
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol