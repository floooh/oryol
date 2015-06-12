//------------------------------------------------------------------------------
//  drawStateBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawStateBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
drawStateBase::drawStateBase() :
hasStreamingMeshes(false),
prog(nullptr) {
    this->meshes.Fill(nullptr);
    // empty
}

//------------------------------------------------------------------------------
drawStateBase::~drawStateBase() {
    o_assert_dbg(nullptr == this->prog);
    #if ORYOL_DEBUG
    for (const mesh* msh : this->meshes) {
        o_assert_dbg(nullptr == msh);
    }
    #endif
}

//------------------------------------------------------------------------------
void
drawStateBase::Clear() {
    this->meshes.Fill(nullptr);
    this->prog = nullptr;
    this->hasStreamingMeshes = false;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol
