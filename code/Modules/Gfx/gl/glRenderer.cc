//------------------------------------------------------------------------------
//  glRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "Gfx/gl/gl_impl.h"
#include "glRenderer.h"
#include "Gfx/gl/glInfo.h"
#include "Gfx/gl/glExt.h"
#include "Gfx/gl/glTypes.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Resource/pipeline.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Oryol {
namespace _priv {

GLenum glRenderer::mapCompareFunc[CompareFunc::NumCompareFuncs] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};
    
GLenum glRenderer::mapStencilOp[StencilOp::NumStencilOperations] = {
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_DECR,
    GL_INVERT,
    GL_INCR_WRAP,
    GL_DECR_WRAP
};
    
GLenum glRenderer::mapBlendFactor[BlendFactor::NumBlendFactors] = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_SRC_ALPHA_SATURATE,
    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA,
};
    
GLenum glRenderer::mapBlendOp[BlendOperation::NumBlendOperations] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
};
    
GLenum glRenderer::mapCullFace[Face::NumFaceCodes] = {
    GL_FRONT,
    GL_BACK,
    GL_FRONT_AND_BACK,
};

//------------------------------------------------------------------------------
glRenderer::glRenderer() :
valid(false),
#if !ORYOL_OPENGLES2
globalVAO(0),
#endif
rtValid(false),
frameIndex(0),
curRenderTarget(nullptr),
curPipeline(nullptr),
curPrimaryMesh(nullptr),
scissorX(0),
scissorY(0),
scissorWidth(0),
scissorHeight(0),
blendColor(1.0f, 1.0f, 1.0f, 1.0f),
viewPortX(0),
viewPortY(0),
viewPortWidth(0),
viewPortHeight(0),
vertexBuffer(0),
indexBuffer(0),
program(0) {
    for (int32 i = 0; i < MaxTextureSamplers; i++) {
        this->samplers2D[i] = 0;
        this->samplersCube[i] = 0;
    }
    for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        this->glAttrVBs[i] = 0;
    }
}

//------------------------------------------------------------------------------
glRenderer::~glRenderer() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
glRenderer::setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert_dbg(!this->valid);
    
    this->valid = true;
    this->pointers = ptrs;
    this->gfxSetup = setup;

    #if ORYOL_GL_USE_GETATTRIBLOCATION
    o_warn("glStateWrapper: ORYOL_GL_USE_GETATTRIBLOCATION is ON\n");
    #endif

    // in case we are on a Core Profile, create a global Vertex Array Object
    #if !ORYOL_OPENGLES2
    ::glGenVertexArrays(1, &this->globalVAO);
    ::glBindVertexArray(this->globalVAO);
    #endif
    
    this->setupDepthStencilState();
    this->setupBlendState();
    this->setupRasterizerState();
    this->invalidateMeshState();
}

//------------------------------------------------------------------------------
void
glRenderer::discard() {
    o_assert_dbg(this->valid);
    
    this->invalidateMeshState();
    this->invalidateShaderState();
    this->invalidateTextureState();
    this->curRenderTarget = nullptr;
    this->curPipeline = nullptr;

    #if !ORYOL_OPENGLES2
    ::glDeleteVertexArrays(1, &this->globalVAO);
    this->globalVAO = 0;
    #endif

    this->pointers = gfxPointers();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
glRenderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
glRenderer::resetStateCache() {
    o_assert_dbg(this->valid);

    this->setupDepthStencilState();
    this->setupBlendState();
    this->setupRasterizerState();
    this->invalidateMeshState();
    this->invalidateShaderState();
    this->invalidateTextureState();
}

//------------------------------------------------------------------------------
bool
glRenderer::queryFeature(GfxFeature::Code feat) const {
    o_assert_dbg(this->valid);

    switch (feat) {
        case GfxFeature::TextureCompressionDXT:
            return glExt::HasExtension(glExt::TextureCompressionDXT);
        case GfxFeature::TextureCompressionPVRTC:
            return glExt::HasExtension(glExt::TextureCompressionPVRTC);
        case GfxFeature::TextureCompressionATC:
            return glExt::HasExtension(glExt::TextureCompressionATC);
        case GfxFeature::TextureCompressionETC2:
            #if ORYOL_OPENGLES3
            return true;
            #else
            return false;
            #endif
        case GfxFeature::TextureFloat:
            return glExt::HasExtension(glExt::TextureFloat);
        case GfxFeature::TextureHalfFloat:
            return glExt::HasExtension(glExt::TextureHalfFloat);
        case GfxFeature::Instancing:
            return glExt::HasExtension(glExt::InstancedArrays);
        case GfxFeature::OriginBottomLeft:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
void
glRenderer::commitFrame() {
    o_assert_dbg(this->valid);    
    this->rtValid = false;
    this->curRenderTarget = nullptr;
    this->curPipeline = nullptr;
    this->curPrimaryMesh = nullptr;
    this->frameIndex++;
}

//------------------------------------------------------------------------------
void
glRenderer::applyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(this->valid);

    // flip origin top/bottom if requested (this is a D3D/GL compatibility thing)
    y = originTopLeft ? (this->rtAttrs.FramebufferHeight - (y + height)) : y;

    if ((x != this->viewPortX) ||
        (y != this->viewPortY) ||
        (width != this->viewPortWidth) ||
        (height != this->viewPortHeight)) {
        
        this->viewPortX = x;
        this->viewPortY = y;
        this->viewPortWidth = width;
        this->viewPortHeight = height;
        #if ORYOL_IOS
        // fix iOS high-dpi coordinates (only for default rendertarget)
        if (!this->curRenderTarget && this->gfxSetup.HighDPI) {
            x*=2; y*=2; width*=2; height*=2;
        }
        #endif
        ::glViewport(x, y, width, height);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(this->valid);

    // flip origin top/bottom if requested (this is a D3D/GL compatibility thing)
    y = originTopLeft ? (this->rtAttrs.FramebufferHeight - (y + height)) : y;

    if ((x != this->scissorX) ||
        (y != this->scissorY) ||
        (width != this->scissorWidth) ||
        (height != this->scissorHeight)) {

        this->scissorX = x;
        this->scissorY = y;
        this->scissorWidth = width;
        this->scissorHeight = height;
        #if ORYOL_IOS
        // fix iOS high-dpi coordinates (only for default rendertarget)
        if (!this->curRenderTarget && this->gfxSetup.HighDPI) {
            x*=2; y*=2; width*=2; height*=2;
        }
        #endif
        ::glScissor(x, y, width, height);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyRenderTarget(texture* rt, const ClearState& clearState) {
    o_assert_dbg(this->valid);
    
    if (nullptr == rt) {
        this->rtAttrs = this->pointers.displayMgr->GetDisplayAttrs();
    }
    else {
        this->rtAttrs = DisplayAttrs::FromTextureAttrs(rt->textureAttrs);
    }
    
    // apply the frame buffer
    if (rt != this->curRenderTarget) {
        // default render target?
        if (nullptr == rt) {
            this->pointers.displayMgr->glBindDefaultFramebuffer();
        }
        else {
            ::glBindFramebuffer(GL_FRAMEBUFFER, rt->glFramebuffer);
            ORYOL_GL_CHECK_ERROR();
        }
    }
    this->curRenderTarget = rt;
    this->rtValid = true;
    
    // set viewport to cover whole screen
    this->applyViewPort(0, 0, this->rtAttrs.FramebufferWidth, this->rtAttrs.FramebufferHeight, false);
    
    // reset scissor test
    if (this->rasterizerState.ScissorTestEnabled) {
        this->rasterizerState.ScissorTestEnabled = false;
        ::glDisable(GL_SCISSOR_TEST);
    }

    // perform clear actions
    GLbitfield glClearMask = 0;
    
    // update GL state
    if (clearState.Actions & ClearState::ColorBit) {
        glClearMask |= GL_COLOR_BUFFER_BIT;
        ::glClearColor(clearState.Color.x, clearState.Color.y, clearState.Color.z, clearState.Color.w);
        if (PixelChannel::RGBA != this->blendState.ColorWriteMask) {
            this->blendState.ColorWriteMask = PixelChannel::RGBA;
            ::glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }
    }
    if (clearState.Actions & ClearState::DepthBit) {
        glClearMask |= GL_DEPTH_BUFFER_BIT;
        #if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
        ::glClearDepthf(clearState.Depth);
        #else
        ::glClearDepth(clearState.Depth);
        #endif
        if (!this->depthStencilState.DepthWriteEnabled) {
            this->depthStencilState.DepthWriteEnabled = true;
            ::glDepthMask(GL_TRUE);
        }
    }
    if (clearState.Actions & ClearState::StencilBit) {
        glClearMask |= GL_STENCIL_BUFFER_BIT;
        ::glClearStencil(clearState.Stencil);
        if (this->depthStencilState.StencilWriteMask != 0xFF) {
            this->depthStencilState.StencilWriteMask = 0xFF;
            ::glStencilMask(0xFF);
        }
    }

    // finally do the actual clear
    if (0 != glClearMask) {
        ::glClear(glClearMask);
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::applyMeshes(pipeline* pip, mesh** meshes, int numMeshes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != pip);
    o_assert_dbg(nullptr != pip->shd);
    o_assert_dbg(meshes && meshes[0] && (numMeshes > 0));
    ORYOL_GL_CHECK_ERROR();

    // need to store primary mesh with primitive group defs for later draw call
    this->curPrimaryMesh = meshes[0];

    #if !ORYOL_GL_USE_GETATTRIBLOCATION
    // this is the default vertex attribute code path for most desktop and mobile platforms
    const auto& ib = this->curPrimaryMesh->buffers[mesh::ib];
    this->bindIndexBuffer(ib.glBuffers[ib.activeSlot]); // can be 0 if mesh has no index buffer
    for (int attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
        const glVertexAttr& attr = pip->glAttrs[attrIndex];
        glVertexAttr& curAttr = this->glAttrs[attrIndex];
        const mesh* msh = meshes[attr.vbIndex];
        const auto& vb = msh->buffers[mesh::vb];
        const GLuint glVB = vb.glBuffers[vb.activeSlot];

        bool vbChanged = (glVB != this->glAttrVBs[attrIndex]);
        bool attrChanged = (attr != curAttr);
        if (vbChanged || attrChanged) {
            if (attr.enabled) {
                this->glAttrVBs[attrIndex] = glVB;
                this->bindVertexBuffer(glVB);
                ::glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, (const GLvoid*)(GLintptr)attr.offset);
                ORYOL_GL_CHECK_ERROR();
                if (!curAttr.enabled) {
                    ::glEnableVertexAttribArray(attr.index);
                    ORYOL_GL_CHECK_ERROR();
                }
            }
            else {
                if (curAttr.enabled) {
                    ::glDisableVertexAttribArray(attr.index);
                    ORYOL_GL_CHECK_ERROR();
                }
            }
            if (curAttr.divisor != attr.divisor) {
                glExt::VertexAttribDivisor(attr.index, attr.divisor);
                ORYOL_GL_CHECK_ERROR();
            }
            curAttr = attr;
        }
    }
    #else
    // this uses glGetAttribLocation for platforms which don't support
    // glBindAttribLocation (e.g. RaspberryPi)
    // FIXME: currently this doesn't use state-caching
    const auto& ib = this->curPrimaryMesh->buffers[mesh::ib];
    this->bindIndexBuffer(ib.glBuffers[ib.activeSlot]);    // can be 0
    int maxUsedAttrib = 0;
    for (int attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
        const glVertexAttr& attr = ds->glAttrs[attrIndex];
        const GLint glAttribIndex = ds->shd->getAttribLocation((VertexAttr::Code)attrIndex);
        if (glAttribIndex >= 0) {
            o_assert_dbg(attr.enabled);
            const mesh* msh = meshes[attr.vbIndex];
            const auto& vb = msh->buffers[mesh::vb];
            const GLuint glVB = vb.glBuffers[vb.activeSlot];
            this->bindVertexBuffer(glVB);
            ::glVertexAttribPointer(glAttribIndex, attr.size, attr.type, attr.normalized, attr.stride, (const GLvoid*)(GLintptr)attr.offset);
            ORYOL_GL_CHECK_ERROR();
            ::glEnableVertexAttribArray(glAttribIndex);
            ORYOL_GL_CHECK_ERROR();
            glExt::VertexAttribDivisor(glAttribIndex, attr.divisor);
            ORYOL_GL_CHECK_ERROR();
            maxUsedAttrib++;
        }
    }
    int maxNumAttribs = glInfo::Int(glInfo::MaxVertexAttribs);
    if (VertexAttr::NumVertexAttrs < maxNumAttribs) {
        maxNumAttribs = VertexAttr::NumVertexAttrs;
    }
    for (int i = maxUsedAttrib; i < maxNumAttribs; i++) {
        ::glDisableVertexAttribArray(i);
        ORYOL_GL_CHECK_ERROR();
    }
    #endif
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::applyDrawState(pipeline* pip, mesh** meshes, int numMeshes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(pip);
    o_assert_dbg(meshes && (numMeshes > 0));

    // if any of the meshes is still loading, cancel the next draw state
    for (int i = 0; i < numMeshes; i++) {
        if (nullptr == meshes[i]) {
            this->curPipeline = nullptr;
            return;
        }
    }

    // draw state is valid, ready for rendering
    this->curPipeline = pip;
    o_assert_dbg(pip->shd);

    const PipelineSetup& setup = pip->Setup;
    o_assert2(setup.BlendState.ColorFormat == this->rtAttrs.ColorPixelFormat, "ColorFormat in BlendState must match current render target!\n");
    o_assert2(setup.BlendState.DepthFormat == this->rtAttrs.DepthPixelFormat, "DepthFormat in BlendState must match current render target!\n");
    o_assert2(setup.RasterizerState.SampleCount == this->rtAttrs.SampleCount, "SampleCount in RasterizerState must match current render target!\n");
    if (setup.DepthStencilState != this->depthStencilState) {
        this->applyDepthStencilState(setup.DepthStencilState);
    }
    if (setup.BlendState != this->blendState) {
        this->applyBlendState(setup.BlendState);
    }
    if (setup.BlendColor != this->blendColor) {
        this->blendColor = setup.BlendColor;
        ::glBlendColor(this->blendColor.x, this->blendColor.y, this->blendColor.z, this->blendColor.w);
    }
    if (setup.RasterizerState != this->rasterizerState) {
        this->applyRasterizerState(setup.RasterizerState);
    }
    this->useProgram(pip->shd->glProgram);
    this->applyMeshes(pip, meshes, numMeshes);
}

//------------------------------------------------------------------------------
void
glRenderer::draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");
    if (nullptr == this->curPipeline) {
        return;
    }
    ORYOL_GL_CHECK_ERROR();
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    const IndexType::Code indexType = msh->indexBufferAttrs.Type;
    const GLenum glPrimType = this->curPipeline->glPrimType;
    if (IndexType::None != indexType) {
        // indexed geometry
        const int32 indexByteSize = IndexType::ByteSize(indexType);
        const GLvoid* indices = (const GLvoid*) (GLintptr) (primGroup.BaseElement * indexByteSize);
        const GLenum glIndexType = glTypes::asGLIndexType(indexType);
        ::glDrawElements(glPrimType, primGroup.NumElements, glIndexType, indices);
    }
    else {
        // non-indexed geometry
        ::glDrawArrays(glPrimType, primGroup.BaseElement, primGroup.NumElements);
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::draw(int32 primGroupIndex) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");
    if (nullptr == this->curPipeline) {
        return;
    }
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    if (primGroupIndex >= msh->numPrimGroups) {
        // this may happen if trying to render a placeholder which doesn't
        // have as many materials as the original mesh, anyway, this isn't
        // a serious error
        return;
    }
    const PrimitiveGroup& primGroup = msh->primGroups[primGroupIndex];
    this->draw(primGroup);
}

//------------------------------------------------------------------------------
void
glRenderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");
    if (nullptr == this->curPipeline) {
        return;
    }
    ORYOL_GL_CHECK_ERROR();
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    const IndexType::Code indexType = msh->indexBufferAttrs.Type;
    const GLenum glPrimType = this->curPipeline->glPrimType;
    if (IndexType::None != indexType) {
        // indexed geometry
        const int32 indexByteSize = IndexType::ByteSize(indexType);
        const GLvoid* indices = (const GLvoid*) (GLintptr) (primGroup.BaseElement * indexByteSize);
        const GLenum glIndexType = glTypes::asGLIndexType(indexType);
        glExt::DrawElementsInstanced(glPrimType, primGroup.NumElements, glIndexType, indices, numInstances);
    }
    else {
        // non-indexed geometry
        glExt::DrawArraysInstanced(glPrimType, primGroup.BaseElement, primGroup.NumElements, numInstances);
    }
    ORYOL_GL_CHECK_ERROR();
}
    
//------------------------------------------------------------------------------
void
glRenderer::drawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");
    if (nullptr == this->curPipeline) {
        return;
    }
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    if (primGroupIndex >= msh->numPrimGroups) {
        // this may happen if trying to render a placeholder which doesn't
        // have as many materials as the original mesh, anyway, this isn't
        // a serious error
        return;
    }
    const PrimitiveGroup& primGroup = msh->primGroups[primGroupIndex];
    this->drawInstanced(primGroup, numInstances);
}

//------------------------------------------------------------------------------
static GLuint
obtainUpdateBuffer(mesh::buffer& buf, int frameIndex) {
    // helper function to get the right GL buffer for a vertex-
    // or index-buffer update, this is implemented with
    // double-buffer to prevent a sync-stall with the GPU
    
    // restrict buffer updates to once per frame per mesh, this isn't
    // strictly required on GL, but we want the same restrictions across all 3D APIs
    o_assert2(buf.updateFrameIndex != frameIndex, "Only one data update allowed per buffer and frame!\n");
    buf.updateFrameIndex = frameIndex;

    // rotate slot index to next dynamic vertex buffer
    // to implement double/multi-buffering because the previous buffer
    // might still be in-flight on the GPU
    // NOTE: buf.numSlots can also be 1 if this is a Dynamic buffer (not a Stream buffer)
    if (++buf.activeSlot >= buf.numSlots) {
        buf.activeSlot = 0;
    }
    return buf.glBuffers[buf.activeSlot];
}

//------------------------------------------------------------------------------
void
glRenderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(nullptr != data);
    o_assert_dbg((numBytes > 0) && (numBytes <= msh->vertexBufferAttrs.ByteSize()));
    o_assert_dbg(Usage::Immutable != msh->vertexBufferAttrs.BufferUsage);

    auto& vb = msh->buffers[mesh::vb];
    GLuint glBuffer = obtainUpdateBuffer(vb, this->frameIndex);
    o_assert_dbg(0 != glBuffer);
    this->bindVertexBuffer(glBuffer);
    ::glBufferSubData(GL_ARRAY_BUFFER, 0, numBytes, data);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::updateIndices(mesh* msh, const void* data, int32 numBytes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(nullptr != data);

    o_assert_dbg(IndexType::None != msh->indexBufferAttrs.Type);
    o_assert_dbg((numBytes > 0) && (numBytes <= msh->indexBufferAttrs.ByteSize()));
    o_assert_dbg(Usage::Immutable != msh->indexBufferAttrs.BufferUsage);

    auto& ib = msh->buffers[mesh::ib];
    GLuint glBuffer = obtainUpdateBuffer(ib, this->frameIndex);
    o_assert_dbg(0 != glBuffer);
    this->bindIndexBuffer(glBuffer);
    ::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numBytes, data);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
static GLuint
obtainUpdateTexture(texture* tex, int frameIndex) {
    // same as obtainUpdateBuffer, but for texture
    o_assert2(tex->updateFrameIndex != frameIndex, "Only one data update allowed per texture and frame!\n");
    tex->updateFrameIndex = frameIndex;
    o_assert_dbg(tex->numSlots > 1);
    if (++tex->activeSlot >= tex->numSlots) {
        tex->activeSlot = 0;
    }
    return tex->glTextures[tex->activeSlot];
}

//------------------------------------------------------------------------------
void
glRenderer::updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != tex);
    o_assert_dbg(nullptr != data);
    ORYOL_GL_CHECK_ERROR();

    // only accept 2D textures for now
    const TextureAttrs& attrs = tex->textureAttrs;
    o_assert_dbg(TextureType::Texture2D == attrs.Type);
    o_assert_dbg(Usage::Immutable != attrs.TextureUsage);
    o_assert_dbg(!PixelFormat::IsCompressedFormat(attrs.ColorFormat));
    o_assert_dbg(offsetsAndSizes.NumMipMaps == attrs.NumMipMaps);
    o_assert_dbg(offsetsAndSizes.NumFaces == 1);

    GLuint glTex = obtainUpdateTexture(tex, this->frameIndex);
    this->bindTexture(0, tex->glTarget, glTex);
    uint8* srcPtr = (uint8*)data;
    GLenum glTexImageFormat = glTypes::asGLTexImageFormat(attrs.ColorFormat);
    GLenum glTexImageType = glTypes::asGLTexImageType(attrs.ColorFormat);
    for (int32 mipIndex = 0; mipIndex < attrs.NumMipMaps; mipIndex++) {
        o_assert_dbg(offsetsAndSizes.Sizes[0][mipIndex] > 0);
        int32 mipWidth = attrs.Width >> mipIndex;
        if (mipWidth == 0) mipWidth = 1;
        int32 mipHeight = attrs.Height >> mipIndex;
        if (mipHeight == 0) mipHeight = 1;
        ::glTexSubImage2D(tex->glTarget,    // target
                          mipIndex,         // level
                          0,                // xoffset
                          0,                // yoffset
                          mipWidth,         // width
                          mipHeight,        // height
                          glTexImageFormat, // format
                          glTexImageType,   // type
                          srcPtr + offsetsAndSizes.Offsets[0][mipIndex]);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glRenderer::readPixels(void* buf, int32 bufNumBytes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->pointers.displayMgr);
    o_assert_dbg((nullptr != buf) && (bufNumBytes > 0));
    
    GLsizei width, height;
    GLenum format, type;
    if (nullptr == this->curRenderTarget) {
        const DisplayAttrs& attrs = this->pointers.displayMgr->GetDisplayAttrs();
        width  = attrs.FramebufferWidth;
        height = attrs.FramebufferHeight;
        format = glTypes::asGLTexImageFormat(attrs.ColorPixelFormat);
        type   = glTypes::asGLTexImageType(attrs.ColorPixelFormat);
        o_assert((width & 3) == 0);
        o_assert(bufNumBytes >= (width * height * PixelFormat::ByteSize(attrs.ColorPixelFormat)));
    }
    else {
        const TextureAttrs& attrs = this->curRenderTarget->textureAttrs;
        width  = attrs.Width;
        height = attrs.Height;
        format = glTypes::asGLTexImageFormat(attrs.ColorFormat);
        type   = glTypes::asGLTexImageType(attrs.ColorFormat);
        o_assert((width & 3) == 0);
        o_assert(bufNumBytes >= (width * height * PixelFormat::ByteSize(attrs.ColorFormat)));
    }
    ::glReadPixels(0, 0, width, height, format, type, buf);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::invalidateMeshState() {
    o_assert_dbg(this->valid);

    ::glBindBuffer(GL_ARRAY_BUFFER, 0);
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    this->vertexBuffer = 0;
    this->indexBuffer = 0;
    for (int i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        this->glAttrs[i] = glVertexAttr();
        this->glAttrVBs[i] = 0;
    }
}

//------------------------------------------------------------------------------
void
glRenderer::bindVertexBuffer(GLuint vb) {
    o_assert_dbg(this->valid);

    if (vb != this->vertexBuffer) {
        this->vertexBuffer = vb;
        ::glBindBuffer(GL_ARRAY_BUFFER, vb);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glRenderer::bindIndexBuffer(GLuint ib) {
    o_assert_dbg(this->valid);

    if (ib != this->indexBuffer) {
        this->indexBuffer = ib;
        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        ORYOL_GL_CHECK_ERROR();
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::invalidateShaderState() {
    o_assert_dbg(this->valid);

    ::glUseProgram(0);
    ORYOL_GL_CHECK_ERROR();
    this->program = 0;
}
    
//------------------------------------------------------------------------------
void
glRenderer::useProgram(GLuint prog) {
    o_assert_dbg(this->valid);
    if (prog != this->program) {
        this->program = prog;
        ::glUseProgram(prog);
        ORYOL_GL_CHECK_ERROR();
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::invalidateTextureState() {
    o_assert_dbg(this->valid);

    for (int32 i = 0; i < MaxTextureSamplers; i++) {
        this->samplers2D[i] = 0;
        this->samplersCube[i] = 0;
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::bindTexture(int32 samplerIndex, GLenum target, GLuint tex) {
    o_assert_dbg(this->valid);
    o_assert_range_dbg(samplerIndex, MaxTextureSamplers);
    o_assert_dbg((target == GL_TEXTURE_2D) || (target == GL_TEXTURE_CUBE_MAP));
    
    GLuint* samplers = (GL_TEXTURE_2D == target) ? this->samplers2D : this->samplersCube;
    if (tex != samplers[samplerIndex]) {
        samplers[samplerIndex] = tex;
        ::glActiveTexture(GL_TEXTURE0 + samplerIndex);
        ORYOL_GL_CHECK_ERROR();
        ::glBindTexture(target, tex);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glRenderer::setupDepthStencilState() {
    o_assert_dbg(this->valid);
    
    this->depthStencilState = DepthStencilState();
    
    ::glEnable(GL_DEPTH_TEST);
    ::glDepthFunc(GL_ALWAYS);
    ::glDepthMask(GL_FALSE);
    ::glDisable(GL_STENCIL_TEST);
    ::glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
    ::glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    ::glStencilMask(0xFFFFFFFF);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::applyStencilState(const DepthStencilState& newState, const DepthStencilState& curState, GLenum glFace) {
    o_assert_dbg(this->valid);
    
    const StencilState& newStencilState = (glFace == GL_FRONT) ? newState.StencilFront : newState.StencilBack;
    const StencilState& curStencilState = (glFace == GL_FRONT) ? curState.StencilFront : curState.StencilBack;

    const CompareFunc::Code cmpFunc = newStencilState.CmpFunc;
    const uint32 readMask = newState.StencilReadMask;
    const int32 stencilRef = newState.StencilRef;
    if ((cmpFunc != curStencilState.CmpFunc) || (readMask != curState.StencilReadMask) || (stencilRef != curState.StencilRef)) {
        o_assert_range_dbg(int(cmpFunc), CompareFunc::NumCompareFuncs);
        ::glStencilFuncSeparate(glFace, mapCompareFunc[cmpFunc], stencilRef, readMask);
    }

    const StencilOp::Code sFailOp = newStencilState.FailOp;
    const StencilOp::Code dFailOp = newStencilState.DepthFailOp;
    const StencilOp::Code passOp = newStencilState.PassOp;
    if ((sFailOp != curStencilState.FailOp) || (dFailOp != curStencilState.DepthFailOp) || (passOp  != curStencilState.PassOp)) {
        o_assert_range_dbg(int(sFailOp), StencilOp::NumStencilOperations);
        o_assert_range_dbg(int(dFailOp), StencilOp::NumStencilOperations);
        o_assert_range_dbg(int(passOp), StencilOp::NumStencilOperations);
        ::glStencilOpSeparate(glFace, mapStencilOp[sFailOp], mapStencilOp[dFailOp], mapStencilOp[passOp]);
    }
    
    const uint32 writeMask = newState.StencilWriteMask;
    if (writeMask != curState.StencilWriteMask) {
        ::glStencilMaskSeparate(glFace, writeMask);
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::applyDepthStencilState(const DepthStencilState& newState) {
    o_assert_dbg(this->valid);
    
    const DepthStencilState& curState = this->depthStencilState;
    
    // apply common depth-stencil state if changed
    bool depthStencilChanged = false;
    if (curState.Hash != newState.Hash) {
        const CompareFunc::Code depthCmpFunc = newState.DepthCmpFunc;
        if (depthCmpFunc != curState.DepthCmpFunc) {
            o_assert_range_dbg(int(depthCmpFunc), CompareFunc::NumCompareFuncs);
            ::glDepthFunc(mapCompareFunc[depthCmpFunc]);
        }
        const bool depthWriteEnabled = newState.DepthWriteEnabled;
        if (depthWriteEnabled != curState.DepthWriteEnabled) {
            ::glDepthMask(depthWriteEnabled);
        }
        const bool stencilEnabled = newState.StencilEnabled;
        if (stencilEnabled != curState.StencilEnabled) {
            if (stencilEnabled) {
                ::glEnable(GL_STENCIL_TEST);
            }
            else {
                ::glDisable(GL_STENCIL_TEST);
            }
        }
        depthStencilChanged = true;
    }
    
    // apply front and back stencil state
    bool frontChanged = false;
    const StencilState& newFront = newState.StencilFront;
    const StencilState& curFront = curState.StencilFront;
    if (curFront.Hash != newFront.Hash) {
        frontChanged = true;
        this->applyStencilState(newState, curState, GL_FRONT);
    }
    bool backChanged = false;
    const StencilState& newBack = newState.StencilBack;
    const StencilState& curBack = curState.StencilBack;
    if (curBack.Hash != newBack.Hash) {
        backChanged = true;
        this->applyStencilState(newState, curState, GL_BACK);
    }
    
    // update state cache
    if (depthStencilChanged || frontChanged || backChanged) {
        this->depthStencilState = newState;
    }
}

//------------------------------------------------------------------------------
void
glRenderer::setupBlendState() {
    o_assert_dbg(this->valid);

    this->blendState = BlendState();
    ::glDisable(GL_BLEND);
    ::glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    ::glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    ::glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    this->blendColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
    ORYOL_GL_CHECK_ERROR();
}
    
//------------------------------------------------------------------------------
void
glRenderer::applyBlendState(const BlendState& bs) {
    o_assert_dbg(this->valid);
    
    if (bs.BlendEnabled != this->blendState.BlendEnabled) {
        if (bs.BlendEnabled) {
            ::glEnable(GL_BLEND);
        }
        else {
            ::glDisable(GL_BLEND);
        }
    }
    
    if ((bs.SrcFactorRGB != this->blendState.SrcFactorRGB) ||
        (bs.DstFactorRGB != this->blendState.DstFactorRGB) ||
        (bs.SrcFactorAlpha != this->blendState.SrcFactorAlpha) ||
        (bs.DstFactorAlpha != this->blendState.DstFactorAlpha)) {
        
        o_assert_dbg(bs.SrcFactorRGB < BlendFactor::NumBlendFactors);
        o_assert_dbg(bs.DstFactorRGB < BlendFactor::NumBlendFactors);
        o_assert_dbg(bs.SrcFactorAlpha < BlendFactor::NumBlendFactors);
        o_assert_dbg(bs.DstFactorAlpha < BlendFactor::NumBlendFactors);
        
        ::glBlendFuncSeparate(mapBlendFactor[bs.SrcFactorRGB],
                              mapBlendFactor[bs.DstFactorRGB],
                              mapBlendFactor[bs.SrcFactorAlpha],
                              mapBlendFactor[bs.DstFactorAlpha]);
    }
    if ((bs.OpRGB != this->blendState.OpRGB) ||
        (bs.OpAlpha != this->blendState.OpAlpha)) {
        
        o_assert_dbg(bs.OpRGB < BlendOperation::NumBlendOperations);
        o_assert_dbg(bs.OpAlpha < BlendOperation::NumBlendOperations);
        
        ::glBlendEquationSeparate(mapBlendOp[bs.OpRGB], mapBlendOp[bs.OpAlpha]);
    }
    
    if (bs.ColorWriteMask != this->blendState.ColorWriteMask) {
        ::glColorMask((bs.ColorWriteMask & PixelChannel::R) != 0,
                      (bs.ColorWriteMask & PixelChannel::G) != 0,
                      (bs.ColorWriteMask & PixelChannel::B) != 0,
                      (bs.ColorWriteMask & PixelChannel::A) != 0);
    }
    
    this->blendState = bs;
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::setupRasterizerState() {
    o_assert_dbg(this->valid);
    
    this->rasterizerState = RasterizerState();
    
    ::glDisable(GL_CULL_FACE);
    ::glFrontFace(GL_CW);
    ::glCullFace(GL_BACK);
    ::glDisable(GL_POLYGON_OFFSET_FILL);
    ::glDisable(GL_SCISSOR_TEST);
    ::glEnable(GL_DITHER);
    #if !(ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    ::glEnable(GL_MULTISAMPLE);
    #endif
    ORYOL_GL_CHECK_ERROR();
}
    
//------------------------------------------------------------------------------
void
glRenderer::applyRasterizerState(const RasterizerState& newState) {
    o_assert_dbg(this->valid);

    const RasterizerState& curState = this->rasterizerState;

    const bool cullFaceEnabled = newState.CullFaceEnabled;
    if (cullFaceEnabled != curState.CullFaceEnabled) {
        if (cullFaceEnabled) {
            ::glEnable(GL_CULL_FACE);
        }
        else {
            ::glDisable(GL_CULL_FACE);
        }
    }
    const Face::Code cullFace = newState.CullFace;
    if (cullFace != curState.CullFace) {
        o_assert_range_dbg(cullFace, Face::NumFaceCodes);
        ::glCullFace(mapCullFace[cullFace]);
    }
    const bool depthOffsetEnabled = newState.DepthOffsetEnabled;
    if (depthOffsetEnabled != curState.DepthOffsetEnabled) {
        if (depthOffsetEnabled) {
            ::glEnable(GL_POLYGON_OFFSET_FILL);
        }
        else {
            ::glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }
    const bool scissorTestEnabled = newState.ScissorTestEnabled;
    if (scissorTestEnabled != curState.ScissorTestEnabled) {
        if (scissorTestEnabled) {
            ::glEnable(GL_SCISSOR_TEST);
        }
        else {
            ::glDisable(GL_SCISSOR_TEST);
        }
    }
    const bool ditherEnabled = newState.DitherEnabled;
    if (ditherEnabled != curState.DitherEnabled) {
        if (ditherEnabled) {
            ::glEnable(GL_DITHER);
        }
        else {
            ::glDisable(GL_DITHER);
        }
    }
    #if !(ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    const uint16 sampleCount = newState.SampleCount;
    if (sampleCount != curState.SampleCount) {
        if (sampleCount > 1) {
            ::glEnable(GL_MULTISAMPLE);
        }
        else {
            ::glDisable(GL_MULTISAMPLE);
        }
    }
    #endif
    this->rasterizerState = newState;
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::applyUniformBlock(ShaderStage::Code bindStage, int32 bindSlot, int64 layoutHash, const uint8* ptr, int32 byteSize) {
    o_assert_dbg(this->valid);
    o_assert_dbg(0 != layoutHash);
    if (!this->curPipeline) {
        // currently no valid draw state set
        return;
    }

    // get the uniform layout object for this uniform block
    const shader* shd = this->curPipeline->shd;
    o_assert_dbg(shd);
    int32 ubIndex = shd->Setup.UniformBlockIndexByStageAndSlot(bindStage, bindSlot);
    o_assert_dbg(InvalidIndex != ubIndex);
    const UniformBlockLayout& layout = shd->Setup.UniformBlockLayout(ubIndex);

    // check whether the provided struct is type-compatible with the
    // expected uniform-block-layout, the size-check shouldn't be necessary
    // since the hash should already bail out, but it doesn't hurt either
    o_assert2(layout.TypeHash == layoutHash, "incompatible uniform block!\n");
    #if !ORYOL_WIN32 // NOTE: VS 32-bit sometimes adds useless padding bytes at end of structs
    o_assert_dbg(layout.ByteSize() == byteSize);
    #endif

    // for each uniform in the uniform block:
    const int numUniforms = layout.NumComponents();
    for (int uniformIndex = 0; uniformIndex < numUniforms; uniformIndex++) {
        const auto& comp = layout.ComponentAt(uniformIndex);
        const uint8* valuePtr = ptr + layout.ComponentByteOffset(uniformIndex);
        GLint glLoc = shd->getUniformLocation(bindStage, bindSlot, uniformIndex);
        if (-1 != glLoc) {
            switch (comp.Type) {
                case UniformType::Float:
                    {
                        o_assert_dbg(1 == comp.Num);
                        const float32 val = *(const float32*)valuePtr;
                        ::glUniform1f(glLoc, val);
                    }
                    break;

                case UniformType::Vec2:
                    {
                        o_assert_dbg(1 == comp.Num);
                        const glm::vec2& val = *(const glm::vec2*) valuePtr;
                        ::glUniform2f(glLoc, val.x, val.y);
                    }
                    break;

                case UniformType::Vec3:
                    {
                        o_assert_dbg(1 == comp.Num);
                        const glm::vec3& val = *(const glm::vec3*)valuePtr;
                        ::glUniform3f(glLoc, val.x, val.y, val.z);
                    }
                    break;

                case UniformType::Vec4:
                    {
                        const glm::vec4& val = *(const glm::vec4*)valuePtr;
                        if (comp.Num > 1) {
                            ::glUniform4fv(glLoc, comp.Num, glm::value_ptr(val));
                        }
                        else {
                            ::glUniform4f(glLoc, val.x, val.y, val.z, val.w);
                        }
                    }
                    break;

                case UniformType::Mat2:
                    {
                        o_assert_dbg(1 == comp.Num);
                        const glm::mat2& val = *(const glm::mat2*)valuePtr;
                        ::glUniformMatrix2fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
                    }
                    break;

                case UniformType::Mat3:
                    {
                        o_assert_dbg(1 == comp.Num);
                        const glm::mat3& val = *(const glm::mat3*)valuePtr;
                        ::glUniformMatrix3fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
                    }
                    break;

                case UniformType::Mat4:
                    {
                        const glm::mat4& val = *(const glm::mat4*)valuePtr;
                        ::glUniformMatrix4fv(glLoc, comp.Num, GL_FALSE, glm::value_ptr(val));
                    }
                    break;

                case UniformType::Bool:
                    {
                        // NOTE: bools are actually stored as int32 in the uniform block struct
                        const int32 val = *(const int32*)valuePtr;
                        ::glUniform1i(glLoc, val);
                    }
                    break;

                default:
                    o_error("FIXME: invalid uniform type!\n");
                    break;
            }
        }
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyTextures(ShaderStage::Code bindStage, Oryol::_priv::texture **textures, int32 numTextures) {
    o_assert_dbg(this->valid);
    o_assert_dbg(((ShaderStage::VS == bindStage) && (numTextures <= GfxConfig::MaxNumVertexTextures)) ||
                 ((ShaderStage::FS == bindStage) && (numTextures <= GfxConfig::MaxNumFragmentTextures)));
    if (nullptr == this->curPipeline) {
        return;
    }

    // if any of the provided texture pointers are not valid, this means
    // that a texture hasn't been loaded yet (or has failed loading), in this
    // case, disable rendering for next draw call
    for (int i = 0; i < numTextures; i++) {
        if (nullptr == textures[i]) {
            this->curPipeline = nullptr;
            return;
        }
    }

    // apply textures and samplers
    const shader* shd = this->curPipeline->shd;
    o_assert_dbg(shd);
    for (int i = 0; i < numTextures; i++) {
        const texture* tex = textures[i];
        const int32 samplerIndex = shd->getSamplerIndex(bindStage, i);
        this->bindTexture(samplerIndex, tex->glTarget, tex->glTextures[tex->activeSlot]);
    }
}

} // namespace _priv
} // namespace Oryol
