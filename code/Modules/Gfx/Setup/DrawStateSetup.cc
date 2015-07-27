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
ShaderSelectionMask(0) {
    // empty
}

//------------------------------------------------------------------------------
DrawStateSetup
DrawStateSetup::FromMeshAndShader(const Id& msh, const Id& shd, uint32 selMask) {
    DrawStateSetup setup;
    setup.Meshes[0] = msh;
    setup.Shader = shd;
    setup.ShaderSelectionMask = selMask;
    return setup;
}

} // namespace Oryol