//------------------------------------------------------------------------------
//  drawStateBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawStateBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
drawStateBase::drawStateBase() :
msh(nullptr),
prog(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
drawStateBase::~drawStateBase() {
    o_assert(nullptr == this->msh);
    o_assert(nullptr == this->prog);
}

//------------------------------------------------------------------------------
void
drawStateBase::Clear() {
    this->msh = nullptr;
    this->prog = nullptr;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol
