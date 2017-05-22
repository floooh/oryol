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
    ResourceBase::Clear();
}

//------------------------------------------------------------------------------
void
meshBase::Clear() {
    this->vertexBufferAttrs = VertexBufferAttrs();
    this->indexBufferAttrs = IndexBufferAttrs();
    this->primGroups.Fill(PrimitiveGroup());
    this->numPrimGroups = 0;
    ResourceBase::Clear();
}

//------------------------------------------------------------------------------
void
pipelineBase::Clear() {
    this->shd = nullptr;
    ResourceBase::Clear();
}

//------------------------------------------------------------------------------
void
renderPassBase::Clear() {
    this->colorTextures.Fill(nullptr);
    this->depthStencilTexture = nullptr;
    ResourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol
