//------------------------------------------------------------------------------
//  glDrawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glDrawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glDrawState::glDrawState() :
glVAO(0) {
    // empty
}

//------------------------------------------------------------------------------
glDrawState::~glDrawState() {
    o_assert_dbg(0 == this->glVAO);
}

//------------------------------------------------------------------------------
void
glDrawState::Clear() {
    this->glVAO = 0;
    this->glAttrs.Fill(glVertexAttr());
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol