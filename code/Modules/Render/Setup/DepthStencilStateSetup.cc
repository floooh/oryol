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
depthWriteEnabled(false),
stencilTestEnabled(false) {
    for (int32 i = 0; i < Face::NumSides; i++) {
        this->stencilState[i].stencilFailOp         = StencilOp::Keep;
        this->stencilState[i].depthFailOp           = StencilOp::Keep;
        this->stencilState[i].depthStencilPassOp    = StencilOp::Keep;
        this->stencilState[i].stencilCompareFunc    = CompareFunc::Always;
        this->stencilState[i].stencilReadMask       = 0xFFFFFFFF;
        this->stencilState[i].stencilWriteMask      = 0xFFFFFFFF;
        this->stencilState[i].stencilRef            = 0;
    }
}

//------------------------------------------------------------------------------
DepthStencilStateSetup::DepthStencilStateSetup(const Resource::Locator& loc_) :
DepthStencilStateSetup() {
    this->loc = loc_;
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
void
DepthStencilStateSetup::SetStencilTestEnabled(bool b) {
    this->stencilTestEnabled = b;
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

//------------------------------------------------------------------------------
void
DepthStencilStateSetup::SetStencilRef(Face::Code sides, int32 ref) {
    o_assert_dbg(sides >= 0);
    __SET_STENCIL_STATE(sides, stencilRef, ref);
}

} // namespace Render
} // namespace Oryol