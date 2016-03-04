//------------------------------------------------------------------------------
//  glDrawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glDrawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glDrawState::glDrawState() :
shdProgIndex(InvalidIndex),
glPrimType(0) {
    // empty
}

//------------------------------------------------------------------------------
void
glDrawState::Clear() {
    this->glAttrs.Fill(glVertexAttr());
    this->shdProgIndex = InvalidIndex;
    this->glPrimType = 0;
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol