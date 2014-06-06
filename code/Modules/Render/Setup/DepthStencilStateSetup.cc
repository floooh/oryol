//------------------------------------------------------------------------------
//  DepthStencilStateSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "DepthStencilStateSetup.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
DepthStencilStateSetup::DepthStencilStateSetup() :
depthCompareFunc(CompareFunc::Always),
depthWriteEnabled(false) {
    for (int32 i = 0; i < Face::NumSides; i++) {
        this->stencilState[i].stencilTestEnabled    = false;
        this->stencilState[i].stencilFailOp         = StencilOp::Keep;
        this->stencilState[i].depthFailOp           = StencilOp::Keep;
        this->stencilState[i].depthStencilPassOp    = StencilOp::Keep;
        this->stencilState[i].stencilCompareFunc    = CompareFunc::Always;
        this->stencilState[i].stencilReadMask       = 0xFFFFFFFF;
        this->stencilState[i].stencilWriteMask      = 0xFFFFFFFF;
    }
}

//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetDepthCompareFunc(CompareFunc::Code f) {
    this->depthCompareFunc = f;
}
    
//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetDepthWriteEnabled(bool b) {
    this->depthWriteEnabled = b;
}

//------------------------------------------------------------------------------
#define __SET_STENCIL_STATE(sides, state, val) \
    if (sides > Face::Both) {\
        this->stencilState[Face::Front].state = val; \
        this->stencilState[Face::Back].state = val; \
    } \
    else { \
        this->stencilState[sides].state = val;\
    }\

//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetStencilTestEnabled(Face::Code sides, bool b) {
    o_assert_dbg(sides >= 0);
    __SET_STENCIL_STATE(sides, stencilTestEnabled, b);
}

//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetStencilFailOp(Face::Code sides, StencilOp::Code op) {
    o_assert_dbg(sides >= 0);
    __SET_STENCIL_STATE(sides, stencilFailOp, op);
}

//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetDepthFailOp(Face::Code sides, StencilOp::Code op) {
    o_assert_dbg(sides >= 0);
    __SET_STENCIL_STATE(sides, depthFailOp, op);
}

//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetDepthStencilPassOp(Face::Code sides, StencilOp::Code op) {
    o_assert_dbg(sides >= 0);
    __SET_STENCIL_STATE(sides, depthStencilPassOp, op);
}

//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetStencilCompareFunc(Face::Code sides, CompareFunc::Code func) {
    o_assert_dbg(sides >= 0);
    __SET_STENCIL_STATE(sides, stencilCompareFunc, func);
}

//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetStencilReadMask(Face::Code sides, uint32 mask) {
    o_assert_dbg(sides >= 0);
    __SET_STENCIL_STATE(sides, stencilReadMask, mask);
}

//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetStencilWriteMask(Face::Code sides, uint32 mask) {
    o_assert_dbg(sides >= 0);
    __SET_STENCIL_STATE(sides, stencilWriteMask, mask);
}

} // namespace Render
} // namespace Oryol