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
PrimType(PrimitiveType::Triangles) {
    // empty
}

//------------------------------------------------------------------------------
DrawStateSetup
DrawStateSetup::FromShader(const Id& shd) {
    o_assert_dbg(shd.IsValid());
    DrawStateSetup setup;
    setup.Shader = shd;
    return setup;
}

//------------------------------------------------------------------------------
DrawStateSetup
DrawStateSetup::FromLayoutAndShader(const VertexLayout& layout, const Id& shd) {
    o_assert_dbg(!layout.Empty());
    o_assert_dbg(shd.IsValid());
    DrawStateSetup setup;
    setup.Layouts[0] = layout;
    setup.Shader = shd;
    return setup;
}

} // namespace Oryol