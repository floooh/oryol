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
BlendColor(1.0f, 1.0f, 1.0f, 1.0f),
ProgramSelectionMask(0) {
    // empty
}

//------------------------------------------------------------------------------
DrawStateSetup
DrawStateSetup::FromMeshAndProg(const Id& msh, const Id& prg, uint32 selMask) {
    DrawStateSetup setup;
    setup.Meshes[0] = msh;
    setup.Program = prg;
    setup.ProgramSelectionMask = selMask;
    return setup;
}

} // namespace Oryol