//------------------------------------------------------------------------------
//  drawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
drawState::drawState() :
prog(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
drawState::~drawState() {
    o_assert(!this->msh.IsValid());
    o_assert(nullptr == this->prog);
}

//------------------------------------------------------------------------------
void
drawState::Clear() {
    this->msh.Invalidate();
    this->prog = nullptr;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol