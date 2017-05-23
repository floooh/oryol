//------------------------------------------------------------------------------
//  resourceBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "resourceBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
shaderBase::Clear() {
    this->Setup = ShaderSetup();
}

//------------------------------------------------------------------------------
void
textureBase::Clear() {
    this->Setup = TextureSetup();
    this->textureAttrs = TextureAttrs();
}

//------------------------------------------------------------------------------
void
meshBase::Clear() {
    this->Setup = MeshSetup();
    this->vertexBufferAttrs = VertexBufferAttrs();
    this->indexBufferAttrs = IndexBufferAttrs();
    this->primGroups.Fill(PrimitiveGroup());
    this->numPrimGroups = 0;
}

//------------------------------------------------------------------------------
void
pipelineBase::Clear() {
    this->Setup = PipelineSetup();
    this->shd = nullptr;
}

//------------------------------------------------------------------------------
void
renderPassBase::Clear() {
    this->Setup = PassSetup();
    this->colorTextures.Fill(nullptr);
    this->depthStencilTexture = nullptr;
}

} // namespace _priv
} // namespace Oryol
