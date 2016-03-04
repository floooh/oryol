//------------------------------------------------------------------------------
//  d3d12DrawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12DrawState.h"
#include "d3d12_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12DrawState::d3d12DrawState() :
d3d12PipelineState(nullptr),
d3d12PrimTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED),
d3d12PrimTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED) {
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
    this->d3d12PrimTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    this->d3d12PrimTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol
