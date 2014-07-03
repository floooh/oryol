//------------------------------------------------------------------------------
//  DrawStateSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "DrawStateSetup.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {

using namespace Resource;

//------------------------------------------------------------------------------
DrawStateSetup::DrawStateSetup() :
progSelMask(0) {
    // empty
}

//------------------------------------------------------------------------------
DrawStateSetup::DrawStateSetup(const Locator& loc_) :
loc(loc_) {
    // empty
}

//------------------------------------------------------------------------------
DrawStateSetup::DrawStateSetup(const Locator& loc_, const Id& msh, const Id& prg, uint32 selMask) :
loc(loc_),
mesh(msh),
prog(prg),
progSelMask(selMask) {

    o_assert(msh.Type() == ResourceType::Mesh);
    o_assert(prg.Type() == ResourceType::ProgramBundle);
}

//------------------------------------------------------------------------------
void
DrawStateSetup::SetMesh(const Id& msh) {
    o_assert(msh.Type() == ResourceType::Mesh);
    this->mesh = msh;
}

//------------------------------------------------------------------------------
void
DrawStateSetup::SetProgram(const Id& prg, uint32 selMask) {
    o_assert(prg.Type() == ResourceType::ProgramBundle);
    this->prog = prg;
    this->progSelMask = selMask;
}

} // namespace Render
} // namespace Oryol