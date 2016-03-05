//------------------------------------------------------------------------------
//  drawStateBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawStateBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
drawStateBase::~drawStateBase() {
    o_assert_dbg(nullptr == this->shd);
}

//------------------------------------------------------------------------------
void
drawStateBase::Clear() {
    this->shd = nullptr;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol
