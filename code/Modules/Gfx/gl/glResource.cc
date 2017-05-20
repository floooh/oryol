//------------------------------------------------------------------------------
//  glResource.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glResource.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glMesh::~glMesh() {
    #if ORYOL_DEBUG
    for (const auto& buf : this->buffers) {
        for (int i = 0; i < MaxNumSlots; i++) {
            o_assert_dbg(0 == buf.glBuffers[i]);
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
glMesh::Clear() {
    for (auto& buf : this->buffers) {
        buf = buffer();
    }
    meshBase::Clear();
}

//------------------------------------------------------------------------------
void
glPipeline::Clear() {
    this->glAttrs.Fill(glPipeline::vertexAttr());
    this->glPrimType = 0;
    pipelineBase::Clear();
}

//------------------------------------------------------------------------------
glShader::glShader() {
    this->Clear();
}

//------------------------------------------------------------------------------
glShader::~glShader() {
    o_assert_dbg(0 == this->glProgram);
}

//------------------------------------------------------------------------------
void
glShader::Clear() {
    this->glProgram = 0;
    this->samplerMappings.Fill(InvalidIndex);
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    this->attribMapping.Fill(-1);
    #endif
    this->uniformBlockMappings.Fill(0);
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
void
glShader::bindUniformBlock(ShaderStage::Code bindStage, int bindSlot, GLint glUniformLocation) {
    this->uniformBlockMappings[uniformBlockArrayIndex(bindStage, bindSlot)] = glUniformLocation;
}

//------------------------------------------------------------------------------
void
glShader::bindSampler(ShaderStage::Code bindStage, int bindSlot, int textureIndex) {
    this->samplerMappings[samplerArrayIndex(bindStage, bindSlot)] = textureIndex;
}

//------------------------------------------------------------------------------
#if ORYOL_GL_USE_GETATTRIBLOCATION
void
glShader::bindAttribLocation(VertexAttr::Code attr, GLint location) {
    this->attribMapping[attr] = location;
}
#endif

//------------------------------------------------------------------------------
glTexture::glTexture() {
    this->glTextures.Fill(0);
}

//------------------------------------------------------------------------------
glTexture::~glTexture() {
    o_assert_dbg(0 == this->glTarget);
    o_assert_dbg(0 == this->glDepthRenderbuffer);
    o_assert_dbg(0 == this->glMSAARenderbuffer);
    #if ORYOL_DEBUG
    for (const auto& glTex : this->glTextures) {
        o_assert_dbg(0 == glTex);
    }
    #endif
}

//------------------------------------------------------------------------------
void
glTexture::Clear() {
    textureBase::Clear();
    this->glTarget = 0;
    this->glDepthRenderbuffer = 0;
    this->glMSAARenderbuffer = 0;
    this->updateFrameIndex = -1;
    this->numSlots = 1;
    this->activeSlot = 0;
    this->glTextures.Fill(0);
}

//------------------------------------------------------------------------------
glRenderPass::glRenderPass() {
    this->glMSAAResolveFramebuffers.Fill(0);
}

//------------------------------------------------------------------------------
glRenderPass::~glRenderPass() {
    o_assert_dbg(0 == this->glFramebuffer);
}

//------------------------------------------------------------------------------
void
glRenderPass::Clear() {
    this->glFramebuffer = 0;
    this->glMSAAResolveFramebuffers.Fill(0);
    renderPassBase::Clear();
}

} // namespace _priv
} // namespace Oryol

