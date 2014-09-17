//------------------------------------------------------------------------------
//  DrawStateSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "DrawStateSetup.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {

//------------------------------------------------------------------------------
DrawStateSetup::DrawStateSetup() :
Locator(Locator::NonShared()),
ProgramSelectionMask(0) {
    // empty
}

//------------------------------------------------------------------------------
DrawStateSetup
DrawStateSetup::FromMeshAndProg(const Id& msh, const Id& prg, uint32 selMask) {
    DrawStateSetup setup;
    setup.Mesh = msh;
    setup.Program = prg;
    setup.ProgramSelectionMask = selMask;
    return setup;
}

} // namespace Oryol