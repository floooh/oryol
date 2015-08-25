//------------------------------------------------------------------------------
//  d3d12DrawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12DrawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12DrawState::d3d12DrawState() :
d3d12PipelineState(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
d3d12DrawState::~d3d12DrawState() {
    o_assert_dbg(nullptr == this->d3d12PipelineState);
}

//------------------------------------------------------------------------------
void
d3d12DrawState::Clear() {
    this->d3d12PipelineState = nullptr;
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol
