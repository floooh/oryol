//------------------------------------------------------------------------------
//  pipelineBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pipelineBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
pipelineBase::~pipelineBase() {
    o_assert_dbg(nullptr == this->shd);
}

//------------------------------------------------------------------------------
void
pipelineBase::Clear() {
    this->shd = nullptr;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol
