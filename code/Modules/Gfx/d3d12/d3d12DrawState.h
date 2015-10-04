#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12DrawState
    @ingroup _priv
    @brief D3D12 implementation of drawState
*/
#include "Gfx/Resource/drawStateBase.h"
#include "Gfx/d3d12/d3d12_decl.h"

namespace Oryol {
namespace _priv {

class d3d12DrawState : public drawStateBase {
public:
    /// constructor
    d3d12DrawState();
    /// destructor
    ~d3d12DrawState();

    /// clear the object (called from drawStateFactory::DestroyResource())
    void Clear();

    /// the d3d12 pipeline state object
    ID3D12PipelineState* d3d12PipelineState;
};

} // namespace _priv
} // namespace Oryol
