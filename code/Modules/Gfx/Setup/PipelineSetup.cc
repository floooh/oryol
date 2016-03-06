//------------------------------------------------------------------------------
//  PipelineSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "PipelineSetup.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {

//------------------------------------------------------------------------------
PipelineSetup::PipelineSetup() :
Locator(Locator::NonShared()),
BlendColor(1.0f, 1.0f, 1.0f, 1.0f),
PrimType(PrimitiveType::Triangles) {
    // empty
}

//------------------------------------------------------------------------------
PipelineSetup
PipelineSetup::FromShader(const Id& shd) {
    o_assert_dbg(shd.IsValid());
    PipelineSetup setup;
    setup.Shader = shd;
    return setup;
}

//------------------------------------------------------------------------------
PipelineSetup
PipelineSetup::FromLayoutAndShader(const VertexLayout& layout, const Id& shd) {
    o_assert_dbg(!layout.Empty());
    o_assert_dbg(shd.IsValid());
    PipelineSetup setup;
    setup.Layouts[0] = layout;
    setup.Shader = shd;
    return setup;
}

} // namespace Oryol