//------------------------------------------------------------------------------
//  drawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawState.h"

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
drawState::drawState() :
depthStencilState_(nullptr),
mesh_(nullptr),
programBundle_(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
drawState::~drawState() {
    o_assert(nullptr == this->depthStencilState_);
    o_assert(nullptr == this->mesh_);
    o_assert(nullptr == this->programBundle_);
}

//------------------------------------------------------------------------------
void
drawState::clear() {
    this->depthStencilState_ = nullptr;
    this->mesh_ = nullptr;
    this->programBundle_ = nullptr;
}

//------------------------------------------------------------------------------
void
drawState::setDepthStencilState(depthStencilState* dss) {
    o_assert(nullptr != dss);
    this->depthStencilState_ = dss;
}

//------------------------------------------------------------------------------
void
drawState::setMesh(mesh* msh) {
    o_assert(nullptr != msh);
    this->mesh_ = msh;
}

//------------------------------------------------------------------------------
void
drawState::setProgramBundle(programBundle* pb) {
    o_assert(nullptr != pb);
    this->programBundle_ = pb;
}

} // namespace Render
} // namespace Oryol