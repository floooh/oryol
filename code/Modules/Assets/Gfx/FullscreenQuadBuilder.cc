//------------------------------------------------------------------------------
//  FullscreenQuadBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "FullscreenQuadBuilder.h"

namespace Oryol {

static const float vertsNoFlip[20] = {
    -1.0f, +1.0f, 0.0f, 0.0f, 1.0f,
    +1.0f, +1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    +1.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

static const float vertsFlip[20] = {
    -1.0f, +1.0f, 0.0f, 0.0f, 0.0f,
    +1.0f, +1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    +1.0f, -1.0f, 0.0f, 1.0f, 1.0f
};

static_assert(sizeof(vertsNoFlip) == sizeof(vertsFlip), "FullscreenQuadBuilder");

FullscreenQuadBuilder::Result
FullscreenQuadBuilder::Build() {
    Result res;
    res.VertexBufferDesc
        .Type(BufferType::VertexBuffer)
        .Size(sizeof(vertsNoFlip))
        .Content(this->flipV ? vertsFlip : vertsNoFlip);
    res.PipelineDesc
        .Layout(0, {
            { "position", VertexFormat::Float3 },
            { "texcoord0", VertexFormat::Float2 }
        })
        .PrimitiveType(PrimitiveType::TriangleStrip);
    return res;
}

} // namespace Oryol
