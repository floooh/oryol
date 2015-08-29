//------------------------------------------------------------------------------
//  d3d11DrawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11DrawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11DrawState::d3d11DrawState() :
d3d11InputLayout(nullptr),
d3d11VertexShader(nullptr),
d3d11PixelShader(nullptr),
d3d11RasterizerState(nullptr),
d3d11DepthStencilState(nullptr),
d3d11BlendState(nullptr) {
    this->d3d11IAVertexBuffers.Fill(nullptr);
    this->d3d11IAStrides.Fill(0);
    this->d3d11IAOffsets.Fill(0);
}

//------------------------------------------------------------------------------
d3d11DrawState::~d3d11DrawState() {
    #if ORYOL_DEBUG
    o_assert_dbg(nullptr == this->d3d11InputLayout);
    o_assert_dbg(nullptr == this->d3d11VertexShader);
    o_assert_dbg(nullptr == this->d3d11PixelShader);
    o_assert_dbg(nullptr == this->d3d11RasterizerState);
    o_assert_dbg(nullptr == this->d3d11DepthStencilState);
    o_assert_dbg(nullptr == this->d3d11BlendState);
    for (int i = 0; i < GfxConfig::MaxNumInputMeshes; i++) {
        o_assert_dbg(nullptr == this->d3d11IAVertexBuffers[i]);
    }
    #endif
}

//------------------------------------------------------------------------------
void
d3d11DrawState::Clear() {
    this->d3d11InputLayout = nullptr;
    this->d3d11VertexShader = nullptr;
    this->d3d11PixelShader = nullptr;
    this->d3d11RasterizerState = nullptr;
    this->d3d11DepthStencilState = nullptr;
    this->d3d11BlendState = nullptr;
    this->d3d11IAVertexBuffers.Fill(nullptr);
    this->d3d11IAStrides.Fill(0);
    this->d3d11IAOffsets.Fill(0);
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol
