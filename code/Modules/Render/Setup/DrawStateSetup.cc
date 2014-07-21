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
DrawStateSetup::DrawStateSetup() {
    // empty
}

//------------------------------------------------------------------------------
DrawStateSetup::DrawStateSetup(const class Locator& loc) :
Locator(loc) {
    // empty
}

//------------------------------------------------------------------------------
DrawStateSetup::DrawStateSetup(const class Locator& loc, const Id& msh, const Id& prg, uint32 selMask) :
Locator(loc),
Mesh(msh),
Program(prg),
ProgramSelectionMask(selMask) {
    // empty
}

} // namespace Render
} // namespace Oryol