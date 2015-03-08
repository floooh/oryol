//------------------------------------------------------------------------------
//  drawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
drawState::drawState() :
mesh(nullptr),
programBundle(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
drawState::~drawState() {
    o_assert(nullptr == this->mesh);
    o_assert(nullptr == this->programBundle);
}

//------------------------------------------------------------------------------
void
drawState::Clear() {
    this->mesh = nullptr;
    this->programBundle = nullptr;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol