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
glPipeline::glPipeline() :
glPrimType(0) {
    // empty
}

//------------------------------------------------------------------------------
void
glPipeline::Clear() {
    this->glAttrs.Fill(glVertexAttr());
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
    this->uniformMappings.Fill(-1);
    this->samplerMappings.Fill(InvalidIndex);
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    this->attribMapping.Fill(-1);
    #endif
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
void
glShader::bindUniform(ShaderStage::Code bindStage, int bindSlot, int uniformIndex, GLint glUniformLocation) {
    this->uniformMappings[uniformArrayIndex(bindStage, bindSlot, uniformIndex)] = glUniformLocation;
}

//------------------------------------------------------------------------------
void
glShader::bindUniformBlock(ShaderStage::Code bindStage, int bindSlot, GLuint glUBBindPoint, GLint glUBDataSize) {
    auto& ubMapping = this->uniformBlockMappings[uniformBlockArrayIndex(bindStage, bindSlot)];
    ubMapping.bindLocation = glUBBindPoint;
    ubMapping.blockDataSize = glUBDataSize;
}

//------------------------------------------------------------------------------
void
glShader::bindSampler(ShaderStage::Code bindStage, int textureIndex, int samplerIndex) {
    this->samplerMappings[samplerArrayIndex(bindStage, textureIndex)] = samplerIndex;
}

//------------------------------------------------------------------------------
#if ORYOL_GL_USE_GETATTRIBLOCATION
void
glShader::bindAttribLocation(VertexAttr::Code attr, GLint location) {
    this->attribMapping[attr] = location;
}
#endif

//------------------------------------------------------------------------------
glTexture::glTexture() :
glTarget(0),
glDepthRenderbuffer(0),
glMSAARenderbuffer(0),
updateFrameIndex(-1),
numSlots(1),
activeSlot(0) {
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
glRenderPass::glRenderPass() :
glFramebuffer(0) {
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

