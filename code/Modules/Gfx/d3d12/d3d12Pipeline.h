#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Pipeline
    @ingroup _priv
    @brief D3D12 implementation of pipeline
*/
#include "Gfx/Resource/pipelineBase.h"
#include "Gfx/d3d12/d3d12_decl.h"

namespace Oryol {
namespace _priv {

class d3d12Pipeline : public pipelineBase {
public:
    /// constructor
    d3d12Pipeline();
    /// destructor
    ~d3d12Pipeline();

    /// clear the object (called from pipelineFactory::DestroyResource())
    void Clear();

    ID3D12PipelineState* d3d12PipelineState;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d12PrimTopologyType;
    D3D12_PRIMITIVE_TOPOLOGY d3d12PrimTopology;
};

} // namespace _priv
} // namespace Oryol
