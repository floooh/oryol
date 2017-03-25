//------------------------------------------------------------------------------
//  resourceBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "resourceBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
textureBase::Clear() {
    this->textureAttrs = TextureAttrs();
    resourceBase::Clear();
}

//------------------------------------------------------------------------------
void
meshBase::Clear() {
    this->vertexBufferAttrs = VertexBufferAttrs();
    this->indexBufferAttrs = IndexBufferAttrs();
    this->primGroups.Fill(PrimitiveGroup());
    this->numPrimGroups = 0;
    resourceBase::Clear();
}

//------------------------------------------------------------------------------
void
pipelineBase::Clear() {
    this->shd = nullptr;
    resourceBase::Clear();
}

//------------------------------------------------------------------------------
void
renderPassBase::Clear() {
    this->colorTextures.Fill(nullptr);
    this->depthStencilTexture = nullptr;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol
