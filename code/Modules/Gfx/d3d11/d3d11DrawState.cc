//------------------------------------------------------------------------------
//  d3d11DrawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11DrawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11DrawState::d3d11DrawState() :
d3d11RasterizerState(nullptr),
d3d11DepthStencilState(nullptr),
d3d11BlendState(nullptr) {
    for (int i = 0; i < d3d11ProgramBundle::MaxNumPrograms; i++) {
        this->d3d11InputLayouts[i] = nullptr;
    }
}

//------------------------------------------------------------------------------
d3d11DrawState::~d3d11DrawState() {
    #if ORYOL_DEBUG
    for (int i = 0; i < d3d11ProgramBundle::MaxNumPrograms; i++) {
        o_assert_dbg(nullptr == this->d3d11InputLayouts[i]);
    }
    o_assert_dbg(nullptr == this->d3d11RasterizerState);
    o_assert_dbg(nullptr == this->d3d11DepthStencilState);
    o_assert_dbg(nullptr == this->d3d11BlendState);
    #endif
}

//------------------------------------------------------------------------------
void
d3d11DrawState::Clear() {
    for (int i = 0; i < d3d11ProgramBundle::MaxNumPrograms; i++) {
        this->d3d11InputLayouts[i] = nullptr;
    }
    this->d3d11RasterizerState = nullptr;
    this->d3d11DepthStencilState = nullptr;
    this->d3d11BlendState = nullptr;
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol
