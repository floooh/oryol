//------------------------------------------------------------------------------
//  drawStateBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawStateBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
drawStateBase::drawStateBase() :
prog(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
drawStateBase::~drawStateBase() {
    o_assert(!this->msh.IsValid());
    o_assert(nullptr == this->prog);
}

//------------------------------------------------------------------------------
void
drawStateBase::Clear() {
    this->msh.Invalidate();
    this->prog = nullptr;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol
