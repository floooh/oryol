//------------------------------------------------------------------------------
//  d3d11Pipeline.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Pipeline.h"
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11Pipeline::d3d11Pipeline() :
d3d11InputLayout(nullptr),
d3d11PrimTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED),
d3d11RasterizerState(nullptr),
d3d11DepthStencilState(nullptr),
d3d11BlendState(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
d3d11Pipeline::~d3d11Pipeline() {
    #if ORYOL_DEBUG
    o_assert_dbg(nullptr == this->d3d11InputLayout);
    o_assert_dbg(nullptr == this->d3d11RasterizerState);
    o_assert_dbg(nullptr == this->d3d11DepthStencilState);
    o_assert_dbg(nullptr == this->d3d11BlendState);
    #endif
}

//------------------------------------------------------------------------------
void
d3d11Pipeline::Clear() {
    this->d3d11InputLayout = nullptr;
    this->d3d11PrimTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    this->d3d11RasterizerState = nullptr;
    this->d3d11DepthStencilState = nullptr;
    this->d3d11BlendState = nullptr;
    pipelineBase::Clear();
}

} // namespace _priv
} // namespace Oryol
