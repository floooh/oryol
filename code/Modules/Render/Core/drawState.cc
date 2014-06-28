//------------------------------------------------------------------------------
//  drawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawState.h"

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
drawState::drawState() :
blendState(nullptr),
depthStencilState(nullptr),
mesh(nullptr),
programBundle(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
drawState::~drawState() {
    o_assert(nullptr == blendState);
    o_assert(nullptr == depthStencilState);
    o_assert(nullptr == mesh);
    o_assert(nullptr == programBundle);
}

//------------------------------------------------------------------------------
void
drawState::clear() {
    this->blendState = nullptr;
    this->depthStencilState = nullptr;
    this->mesh = nullptr;
    this->programBundle = nullptr;
}

//------------------------------------------------------------------------------
void
drawState::setBlendState(class blendState* bs) {
    o_assert(nullptr != bs);
    this->blendState = bs;
}

//------------------------------------------------------------------------------
void
drawState::setDepthStencilState(class depthStencilState* dss) {
    o_assert(nullptr != dss);
    this->depthStencilState = dss;
}

//------------------------------------------------------------------------------
void
drawState::setMesh(class mesh* msh) {
    o_assert(nullptr != msh);
    this->mesh = msh;
}

//------------------------------------------------------------------------------
void
drawState::setProgramBundle(class programBundle* pb) {
    o_assert(nullptr != pb);
    this->programBundle = pb;
}

} // namespace Render
} // namespace Oryol