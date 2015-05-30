//------------------------------------------------------------------------------
//  d3d11DrawState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11DrawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11DrawState::d3d11DrawState() {
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
    #endif
}

//------------------------------------------------------------------------------
void
d3d11DrawState::Clear() {
    for (int i = 0; i < d3d11ProgramBundle::MaxNumPrograms; i++) {
        this->d3d11InputLayouts[i] = nullptr;
    }
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol
