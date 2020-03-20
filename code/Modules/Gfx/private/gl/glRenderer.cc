//------------------------------------------------------------------------------
//  glRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "Gfx/private/displayMgr.h"
#include "Gfx/private/resourcePools.h"
#include "Gfx/private/resource.h"
#include "gl_impl.h"
#include "glRenderer.h"
#include "glTypes.h"
#include "glCaps.h"
#include "glm/vec4.hpp"
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
glRenderer::glRenderer() {
    this->blendColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->samplers.Fill(0);
    this->glAttrVBs.Fill(0);
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
    this->frameIndex = 0;

    #if ORYOL_GL_USE_GETATTRIBLOCATION
    o_warn("glRenderer: ORYOL_GL_USE_GETATTRIBLOCATION is ON\n");
    #endif

    // in case we are on a Core Profile, create a global Vertex Array Object
    #if !ORYOL_OPENGLES2
    if (!glCaps::IsFlavour(glCaps::GLES2)) {
        ::glGenVertexArrays(1, &this->globalVAO);
        ::glBindVertexArray(this->globalVAO);
        ORYOL_GL_CHECK_ERROR();
    }
    #endif

    #if !(ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    ::glEnable(GL_PROGRAM_POINT_SIZE);
    ORYOL_GL_CHECK_ERROR();
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
    this->curRenderPass = nullptr;
    this->curPipeline = nullptr;

    #if !ORYOL_OPENGLES2
    if (!glCaps::IsFlavour(glCaps::GLES2)) {
        ::glDeleteVertexArrays(1, &this->globalVAO);
        this->globalVAO = 0;
    }
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
            return glCaps::HasFeature(glCaps::TextureCompressionDXT);
        case GfxFeature::TextureCompressionPVRTC:
            return glCaps::HasFeature(glCaps::TextureCompressionPVRTC);
        case GfxFeature::TextureCompressionATC:
            return glCaps::HasFeature(glCaps::TextureCompressionATC);
        case GfxFeature::TextureCompressionETC2:
            return glCaps::HasFeature(glCaps::TextureCompressionETC2);
        case GfxFeature::TextureFloat:
            return glCaps::HasFeature(glCaps::TextureFloat);
        case GfxFeature::TextureHalfFloat:
            return glCaps::HasFeature(glCaps::TextureHalfFloat);
        case GfxFeature::Instancing:
            return glCaps::HasFeature(glCaps::InstancedArrays);
        case GfxFeature::OriginBottomLeft:
        case GfxFeature::NativeTexture:
            return true;
        case GfxFeature::MSAARenderTargets:
            return glCaps::HasFeature(glCaps::MSAARenderTargets);
        case GfxFeature::PackedVertexFormat_10_2:
            return glCaps::HasFeature(glCaps::PackedVertexFormat_10_2);
        case GfxFeature::MultipleRenderTarget:
            return glCaps::HasFeature(glCaps::MultipleRenderTarget);
        case GfxFeature::Texture3D:
            return glCaps::HasFeature(glCaps::Texture3D);
        case GfxFeature::TextureArray:
            return glCaps::HasFeature(glCaps::TextureArray);
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
void
glRenderer::commitFrame() {
    o_assert_dbg(this->valid);
    this->rpValid = false;
    this->curRenderPass = nullptr;
    this->curPipeline = nullptr;
    this->curPrimaryMesh = nullptr;
    this->frameIndex++;
}

//------------------------------------------------------------------------------
void
glRenderer::applyViewPort(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(this->valid);

    // flip origin top/bottom if requested (this is a D3D/GL compatibility thing)
    y = originTopLeft ? (this->rpAttrs.FramebufferHeight - (y + height)) : y;

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
        if (!this->curRenderPass && this->gfxSetup.HighDPI) {
            x*=2; y*=2; width*=2; height*=2;
        }
        #endif
        ::glViewport(x, y, width, height);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyScissorRect(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(this->valid);

    // flip origin top/bottom if requested (this is a D3D/GL compatibility thing)
    y = originTopLeft ? (this->rpAttrs.FramebufferHeight - (y + height)) : y;

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
        if (!this->curRenderPass && this->gfxSetup.HighDPI) {
            x*=2; y*=2; width*=2; height*=2;
        }
        #endif
        ::glScissor(x, y, width, height);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::beginPass(renderPass* pass, const PassAction* action) {
    o_assert_dbg(this->valid);
    o_assert_dbg(action);
    ORYOL_GL_CHECK_ERROR();

    if (nullptr == pass) {
        this->rpAttrs = this->pointers.displayMgr->GetDisplayAttrs();
    }
    else {
        o_assert_dbg(pass->colorTextures[0]);
        this->rpAttrs = DisplayAttrs::FromTextureAttrs(pass->colorTextures[0]->textureAttrs);
    }

    o_assert_dbg(nullptr == this->curRenderPass);
    if (nullptr == pass) {
        this->pointers.displayMgr->glBindDefaultFramebuffer();
    }
    else {
        ::glBindFramebuffer(GL_FRAMEBUFFER, pass->glFramebuffer);
        ORYOL_GL_CHECK_ERROR();
        #if !ORYOL_OPENGLES2
        if (!glCaps::IsFlavour(glCaps::GLES2)) {
            int numAtts = 0;
            GLenum att[GfxConfig::MaxNumColorAttachments] = { };
            for (; numAtts < GfxConfig::MaxNumColorAttachments; numAtts++) {
                if (pass->colorTextures[numAtts]) {
                    att[numAtts] = GL_COLOR_ATTACHMENT0 + numAtts;
                }
                else {
                    break;
                }
            }
            ::glDrawBuffers(numAtts, att);
            ORYOL_GL_CHECK_ERROR();
        }
        #endif
    }
    ORYOL_GL_CHECK_ERROR();
    this->curRenderPass = pass;
    this->rpValid = true;

    // prepare state for clear operations
    this->applyViewPort(0, 0, this->rpAttrs.FramebufferWidth, this->rpAttrs.FramebufferHeight, false);
    if (this->rasterizerState.ScissorTestEnabled) {
        this->rasterizerState.ScissorTestEnabled = false;
        ::glDisable(GL_SCISSOR_TEST);
    }
    if (PixelChannel::RGBA != this->blendState.ColorWriteMask) {
        this->blendState.ColorWriteMask = PixelChannel::RGBA;
        ::glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    if (!this->depthStencilState.DepthWriteEnabled) {
        this->depthStencilState.DepthWriteEnabled = true;
        ::glDepthMask(GL_TRUE);
    }
    if (this->depthStencilState.StencilWriteMask != 0xFF) {
        this->depthStencilState.StencilWriteMask = 0xFF;
        ::glStencilMask(0xFF);
    }
    ORYOL_GL_CHECK_ERROR();

    // perform clear actions on render targets
    // FIXME: GL_EXT_discard_framebuffer for DontCare
    if ((nullptr == pass) || glCaps::IsFlavour(glCaps::GLES2)) {
        // special case: default render pass or no MRT support
        GLbitfield clearMask = 0;
        if (action->Flags & PassAction::ClearC0) {
            clearMask |= GL_COLOR_BUFFER_BIT;
            const auto& c = action->Color[0];
            ::glClearColor(c.x, c.y, c.z, c.w);
        }
        if (action->Flags & PassAction::ClearDS) {
            clearMask |= GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT;
            #if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
            ::glClearDepthf(action->Depth);
            #else
            ::glClearDepth(action->Depth);
            #endif
            ::glClearStencil(action->Stencil);
        }
        if (0 != clearMask) {
            ::glClear(clearMask);
        }
        ORYOL_GL_CHECK_ERROR();
    }
    #if !ORYOL_OPENGLES2
    else {
        o_assert_dbg(pass);
        // GLES3 / GL3 potential MRT
        for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
            if (pass->colorTextures[i]) {
                if (action->Flags & (PassAction::ClearC0<<i)) {
                    const GLfloat* c = glm::value_ptr(action->Color[i]);
                    ::glClearBufferfv(GL_COLOR, i, c);
                }
            }
        }
        if (pass->depthStencilTexture && (action->Flags & PassAction::ClearDS)) {
            ::glClearBufferfi(GL_DEPTH_STENCIL, 0, action->Depth, action->Stencil);
        }
        ORYOL_GL_CHECK_ERROR();
    }
    #endif
}

//------------------------------------------------------------------------------
void
glRenderer::beginPassNoFbBind (renderPass* pass, const PassAction* action) {
  o_assert_dbg (this->valid);
  o_assert_dbg (action);
  ORYOL_GL_CHECK_ERROR ();

  if (nullptr == pass) {
    this->rpAttrs = this->pointers.displayMgr->GetDisplayAttrs ();
  }
  else {
    o_assert_dbg (pass->colorTextures[0]);
    this->rpAttrs = DisplayAttrs::FromTextureAttrs (pass->colorTextures[0]->textureAttrs);
  }

  o_assert_dbg (nullptr == this->curRenderPass);
  if (nullptr == pass) {
    //this->pointers.displayMgr->glBindDefaultFramebuffer ();
  }
  else {
    //::glBindFramebuffer (GL_FRAMEBUFFER, pass->glFramebuffer);
    ORYOL_GL_CHECK_ERROR ();
  #if !ORYOL_OPENGLES2
    if (!glCaps::IsFlavour (glCaps::GLES2)) {
      int numAtts = 0;
      GLenum att[GfxConfig::MaxNumColorAttachments] = {};
      for (; numAtts < GfxConfig::MaxNumColorAttachments; numAtts++) {
        if (pass->colorTextures[numAtts]) {
          att[numAtts] = GL_COLOR_ATTACHMENT0 + numAtts;
        }
        else {
          break;
        }
      }
      ::glDrawBuffers (numAtts, att);
      ORYOL_GL_CHECK_ERROR ();
    }
  #endif
  }
  ORYOL_GL_CHECK_ERROR ();
  this->curRenderPass = pass;
  this->rpValid = true;

  // prepare state for clear operations
  this->applyViewPort (0, 0, this->rpAttrs.FramebufferWidth, this->rpAttrs.FramebufferHeight, false);
  if (this->rasterizerState.ScissorTestEnabled) {
    this->rasterizerState.ScissorTestEnabled = false;
    ::glDisable (GL_SCISSOR_TEST);
  }
  if (PixelChannel::RGBA != this->blendState.ColorWriteMask) {
    this->blendState.ColorWriteMask = PixelChannel::RGBA;
    ::glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  }
  if (!this->depthStencilState.DepthWriteEnabled) {
    this->depthStencilState.DepthWriteEnabled = true;
    ::glDepthMask (GL_TRUE);
  }
  if (this->depthStencilState.StencilWriteMask != 0xFF) {
    this->depthStencilState.StencilWriteMask = 0xFF;
    ::glStencilMask (0xFF);
  }
  ORYOL_GL_CHECK_ERROR ();

  // perform clear actions on render targets
  // FIXME: GL_EXT_discard_framebuffer for DontCare
  if ((nullptr == pass) || glCaps::IsFlavour (glCaps::GLES2)) {
    // special case: default render pass or no MRT support
    GLbitfield clearMask = 0;
    if (action->Flags & PassAction::ClearC0) {
      clearMask |= GL_COLOR_BUFFER_BIT;
      const auto& c = action->Color[0];
      ::glClearColor (c.x, c.y, c.z, c.w);
    }
    if (action->Flags & PassAction::ClearDS) {
      clearMask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
    #if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
      ::glClearDepthf (action->Depth);
    #else
      ::glClearDepth (action->Depth);
    #endif
      ::glClearStencil (action->Stencil);
    }
    if (0 != clearMask) {
      ::glClear (clearMask);
    }
    ORYOL_GL_CHECK_ERROR ();
  }
#if !ORYOL_OPENGLES2
  else {
    o_assert_dbg (pass);
    // GLES3 / GL3 potential MRT
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
      if (pass->colorTextures[i]) {
        if (action->Flags & (PassAction::ClearC0 << i)) {
          const GLfloat* c = glm::value_ptr (action->Color[i]);
          ::glClearBufferfv (GL_COLOR, i, c);
        }
      }
    }
    if (pass->depthStencilTexture && (action->Flags & PassAction::ClearDS)) {
      ::glClearBufferfi (GL_DEPTH_STENCIL, 0, action->Depth, action->Stencil);
    }
    ORYOL_GL_CHECK_ERROR ();
  }
#endif
}

//------------------------------------------------------------------------------
void
glRenderer::endPass() {
    o_assert_dbg(this->valid);

    // perform the MSAA resolve if necessary
    #if !ORYOL_OPENGLES2
    if (!glCaps::IsFlavour(glCaps::GLES2)) {
        const renderPass* rp = this->curRenderPass;
        if (rp) {
            const bool isMSAA = 0 != rp->colorTextures[0]->glMSAARenderbuffer;
            if (isMSAA) {
                ::glBindFramebuffer(GL_READ_FRAMEBUFFER, rp->glFramebuffer);
                o_assert_dbg(rp->colorTextures[0]);
                const int w = rp->colorTextures[0]->textureAttrs.Width;
                const int h = rp->colorTextures[0]->textureAttrs.Height;
                for (int attIndex = 0; attIndex < GfxConfig::MaxNumColorAttachments; attIndex++) {
                    if (rp->colorTextures[attIndex]) {
                        o_assert_dbg(rp->glMSAAResolveFramebuffers[attIndex]);
                        ::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rp->glMSAAResolveFramebuffers[attIndex]);
                        ::glReadBuffer(GL_COLOR_ATTACHMENT0+attIndex);
                        const GLenum att = GL_COLOR_ATTACHMENT0;
                        ::glDrawBuffers(1, &att);
                        ::glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                    }
                    else {
                        break;
                    }
                }
                ORYOL_GL_CHECK_ERROR();
            }
        }
    }
    #endif
    this->pointers.displayMgr->glBindDefaultFramebuffer();
    ORYOL_GL_CHECK_ERROR();
    this->curRenderPass = nullptr;
    this->rpValid = false;
}

//------------------------------------------------------------------------------
void
glRenderer::applyDrawState(pipeline* pip, mesh** meshes, int numMeshes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(pip);
    o_assert_dbg(meshes && (numMeshes > 0));

    // do debug validation before record/playback, simplifies debugging
    const PipelineSetup& setup = pip->Setup;
    #if ORYOL_DEBUG
    o_assert2(setup.BlendState.ColorFormat == this->rpAttrs.ColorPixelFormat, "ColorFormat in BlendState must match current render target!\n");
    o_assert2(setup.BlendState.DepthFormat == this->rpAttrs.DepthPixelFormat, "DepthFormat in BlendState must match current render target!\n");
    o_assert2(setup.RasterizerState.SampleCount == this->rpAttrs.SampleCount, "SampleCount in RasterizerState must match current render target!\n");
    if (this->curRenderPass) {
        for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
            const texture* tex = this->curRenderPass->colorTextures[i];
            if (tex) {
                o_assert2(setup.BlendState.ColorFormat == tex->textureAttrs.ColorFormat, "ColorFormat in BlendState must match MRT color attachments!\n");
                o_assert2(setup.RasterizerState.SampleCount == tex->textureAttrs.SampleCount, "SampleCount in RasterizerState must match MRT color attachments!\n");
            }
        }
        const texture* dsTex = this->curRenderPass->depthStencilTexture;
        if (dsTex) {
            o_assert2(setup.BlendState.DepthFormat == dsTex->textureAttrs.DepthFormat, "DepthFormat in BlendState must match depth/stencil attachment!\n");
        }
    }
    #endif

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

    // apply DepthStencilState changes
    if (setup.DepthStencilState != this->depthStencilState) {
    
        const DepthStencilState& curState = this->depthStencilState;
        const DepthStencilState& newState = setup.DepthStencilState;
    
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
                if (stencilEnabled) ::glEnable(GL_STENCIL_TEST);
                else                ::glDisable(GL_STENCIL_TEST);
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
    if (setup.BlendState != this->blendState) {

        const BlendState& curState = this->blendState;
        const BlendState& newState = setup.BlendState;

        if (newState.BlendEnabled != curState.BlendEnabled) {
            if (newState.BlendEnabled) ::glEnable(GL_BLEND);
            else                       ::glDisable(GL_BLEND);
        }
    
        if ((newState.SrcFactorRGB != curState.SrcFactorRGB) ||
            (newState.DstFactorRGB != curState.DstFactorRGB) ||
            (newState.SrcFactorAlpha != curState.SrcFactorAlpha) ||
            (newState.DstFactorAlpha != curState.DstFactorAlpha)) {
            
            o_assert_dbg(newState.SrcFactorRGB < BlendFactor::NumBlendFactors);
            o_assert_dbg(newState.DstFactorRGB < BlendFactor::NumBlendFactors);
            o_assert_dbg(newState.SrcFactorAlpha < BlendFactor::NumBlendFactors);
            o_assert_dbg(newState.DstFactorAlpha < BlendFactor::NumBlendFactors);
            
            ::glBlendFuncSeparate(mapBlendFactor[newState.SrcFactorRGB],
                                  mapBlendFactor[newState.DstFactorRGB],
                                  mapBlendFactor[newState.SrcFactorAlpha],
                                  mapBlendFactor[newState.DstFactorAlpha]);
        }
        if ((newState.OpRGB != curState.OpRGB) ||
            (newState.OpAlpha != curState.OpAlpha)) {
            
            o_assert_dbg(curState.OpRGB < BlendOperation::NumBlendOperations);
            o_assert_dbg(curState.OpAlpha < BlendOperation::NumBlendOperations);
            
            ::glBlendEquationSeparate(mapBlendOp[newState.OpRGB], mapBlendOp[newState.OpAlpha]);
        }
        
        if (newState.ColorWriteMask != curState.ColorWriteMask) {
            ::glColorMask((newState.ColorWriteMask & PixelChannel::R) != 0,
                          (newState.ColorWriteMask & PixelChannel::G) != 0,
                          (newState.ColorWriteMask & PixelChannel::B) != 0,
                          (newState.ColorWriteMask & PixelChannel::A) != 0);
        }
        
        this->blendState = newState;
        ORYOL_GL_CHECK_ERROR();
    }
    if (setup.BlendColor != this->blendColor) {
        this->blendColor = setup.BlendColor;
        ::glBlendColor(this->blendColor.x, this->blendColor.y, this->blendColor.z, this->blendColor.w);
    }
    if (setup.RasterizerState != this->rasterizerState) {

        const RasterizerState& curState = this->rasterizerState;
        const RasterizerState& newState = setup.RasterizerState;

        const bool cullFaceEnabled = newState.CullFaceEnabled;
        if (cullFaceEnabled != curState.CullFaceEnabled) {
            if (cullFaceEnabled) ::glEnable(GL_CULL_FACE);
            else                 ::glDisable(GL_CULL_FACE);
        }
        const Face::Code cullFace = newState.CullFace;
        if (cullFace != curState.CullFace) {
            o_assert_range_dbg(cullFace, Face::NumFaceCodes);
            ::glCullFace(mapCullFace[cullFace]);
        }
        const bool scissorTestEnabled = newState.ScissorTestEnabled;
        if (scissorTestEnabled != curState.ScissorTestEnabled) {
            if (scissorTestEnabled) ::glEnable(GL_SCISSOR_TEST);
            else                    ::glDisable(GL_SCISSOR_TEST);
        }
        const bool ditherEnabled = newState.DitherEnabled;
        if (ditherEnabled != curState.DitherEnabled) {
            if (ditherEnabled) ::glEnable(GL_DITHER);
            else               ::glDisable(GL_DITHER);
        }
        #if !(ORYOL_OPENGLES2 || ORYOL_OPENGLES3) 
        const uint16_t sampleCount = newState.SampleCount;
        if (sampleCount != curState.SampleCount) {
            if (sampleCount > 1) ::glEnable(GL_MULTISAMPLE);
            else                 ::glDisable(GL_MULTISAMPLE);
        }
        #endif
        this->rasterizerState = newState;
        ORYOL_GL_CHECK_ERROR();
    }

    // bind program and uniform buffers
    this->useProgram(pip->shd->glProgram);

    // need to store primary mesh with primitive group defs for later draw call
    this->curPrimaryMesh = meshes[0];

    // apply meshes
    #if !ORYOL_GL_USE_GETATTRIBLOCATION
    // this is the default vertex attribute code path for most desktop and mobile platforms
    const auto& ib = this->curPrimaryMesh->buffers[mesh::ib];
    this->bindIndexBuffer(ib.glBuffers[ib.activeSlot]); // can be 0 if mesh has no index buffer
    for (int attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
        const auto& attr = pip->glAttrs[attrIndex];
        o_assert_dbg(attr.vbIndex < numMeshes);
        auto& curAttr = this->glAttrs[attrIndex];
        const mesh* msh = meshes[attr.vbIndex];
        o_assert_dbg(msh);
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
                glCaps::VertexAttribDivisor(attr.index, attr.divisor);
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
        const auto& attr = pip->glAttrs[attrIndex];
        const GLint glAttribIndex = pip->shd->getAttribLocation((VertexAttr::Code)attrIndex);
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
            glCaps::VertexAttribDivisor(glAttribIndex, attr.divisor);
            ORYOL_GL_CHECK_ERROR();
            maxUsedAttrib++;
        }
    }
    int maxNumAttribs = glCaps::IntLimit(glCaps::MaxVertexAttribs);
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
glRenderer::draw(int baseElementIndex, int numElements, int numInstances) {
    o_assert_dbg(this->valid);
    o_assert_dbg(numInstances >= 1);

    o_assert2_dbg(this->rpValid, "Not inside BeginPass / EndPass!");
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
        const int indexByteSize = IndexType::ByteSize(indexType);
        const GLvoid* indices = (const GLvoid*) (GLintptr) (baseElementIndex * indexByteSize);
        const GLenum glIndexType = glTypes::asGLIndexType(indexType);
        if (numInstances == 1) {
            ::glDrawElements(glPrimType, numElements, glIndexType, indices);
        }
        else {
            glCaps::DrawElementsInstanced(glPrimType, numElements, glIndexType, indices, numInstances);
        }
    }
    else {
        // non-indexed geometry
        if (numInstances == 1) {
            ::glDrawArrays(glPrimType, baseElementIndex, numElements);
        }
        else {
            glCaps::DrawArraysInstanced(glPrimType, baseElementIndex, numElements, numInstances);
        }
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::draw(int primGroupIndex, int numInstances) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rpValid, "Not inside BeginPass / EndPass!");
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
    this->draw(primGroup.BaseElement, primGroup.NumElements, numInstances);
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
glRenderer::updateVertices(mesh* msh, const void* data, int numBytes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(nullptr != data);
    o_assert_dbg((numBytes > 0) && (numBytes <= msh->vertexBufferAttrs.ByteSize()));
    o_assert_dbg(Usage::Immutable != msh->vertexBufferAttrs.BufferUsage);

    auto& vb = msh->buffers[mesh::vb];
    GLuint glBuffer = obtainUpdateBuffer(vb, (int)this->frameIndex);
    o_assert_dbg(0 != glBuffer);
    this->bindVertexBuffer(glBuffer);
    ::glBufferSubData(GL_ARRAY_BUFFER, 0, numBytes, data);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::updateIndices(mesh* msh, const void* data, int numBytes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(nullptr != data);

    o_assert_dbg(IndexType::None != msh->indexBufferAttrs.Type);
    o_assert_dbg((numBytes > 0) && (numBytes <= msh->indexBufferAttrs.ByteSize()));
    o_assert_dbg(Usage::Immutable != msh->indexBufferAttrs.BufferUsage);

    auto& ib = msh->buffers[mesh::ib];
    GLuint glBuffer = obtainUpdateBuffer(ib, (int)this->frameIndex);
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
    o_assert_dbg(offsetsAndSizes.NumMipMaps <= attrs.NumMipMaps);
    o_assert_dbg(offsetsAndSizes.NumFaces == 1);

    GLuint glTex = obtainUpdateTexture(tex, int(this->frameIndex));
    this->bindTexture(0, tex->glTarget, glTex);
    uint8_t* srcPtr = (uint8_t*)data;
    GLenum glTexImageFormat = glTypes::asGLTexImageFormat(attrs.ColorFormat);
    GLenum glTexImageType = glTypes::asGLTexImageType(attrs.ColorFormat);
    for (int mipIndex = 0; mipIndex < offsetsAndSizes.NumMipMaps; mipIndex++) {
        o_assert_dbg(offsetsAndSizes.Sizes[0][mipIndex] > 0);
        int mipWidth = attrs.Width >> mipIndex;
        if (mipWidth == 0) mipWidth = 1;
        int mipHeight = attrs.Height >> mipIndex;
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
glRenderer::invalidateMeshState() {
    o_assert_dbg(this->valid);
    
    ORYOL_GL_CHECK_ERROR();
    ::glBindBuffer(GL_ARRAY_BUFFER, 0);
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    this->vertexBuffer = 0;
    this->indexBuffer = 0;
    for (int i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        ::glDisableVertexAttribArray(i);
        this->glAttrs[i] = pipeline::vertexAttr();
        this->glAttrVBs[i] = 0;
    }
    ORYOL_GL_CHECK_ERROR();
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

    ORYOL_GL_CHECK_ERROR();
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
    for (int i = 0; i < MaxTextureSamplers; i++) {
        this->samplers[i] = 0;
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::bindTexture(int samplerIndex, GLenum target, GLuint tex) {
    o_assert_dbg(this->valid);
    o_assert_range_dbg(samplerIndex, MaxTextureSamplers);
    #if ORYOL_OPENGLES2
    o_assert_dbg((target == GL_TEXTURE_2D) || (target == GL_TEXTURE_CUBE_MAP));
    #else
    o_assert_dbg((target == GL_TEXTURE_2D) || (target == GL_TEXTURE_CUBE_MAP) ||
                 (target == GL_TEXTURE_3D) || (target == GL_TEXTURE_2D_ARRAY));
    #endif

    if (tex != this->samplers[samplerIndex]) {
        this->samplers[samplerIndex] = tex;
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
    const uint32_t readMask = newState.StencilReadMask;
    const int stencilRef = newState.StencilRef;
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
    
    const uint32_t writeMask = newState.StencilWriteMask;
    if (writeMask != curState.StencilWriteMask) {
        ::glStencilMaskSeparate(glFace, writeMask);
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
glRenderer::applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t typeHash, const uint8_t* ptr, int byteSize) {
    o_assert_dbg(this->valid);
    // bytesize must be a multiple of sizeof(vec4)
    o_assert_dbg((byteSize & 15) == 0);
    if (!this->curPipeline) {
        // currently no valid draw state set
        return;
    }

    // get the uniform layout object for this uniform block
    const shader* shd = this->curPipeline->shd;
    o_assert_dbg(shd);

    #if ORYOL_DEBUG
    // check whether the provided struct is type-compatible with the
    // expected uniform-block-layout, the size-check shouldn't be necessary
    // since the hash should already bail out, but it doesn't hurt either
    int ubIndex = shd->Setup.UniformBlockIndexByStageAndSlot(bindStage, bindSlot);
    o_assert(InvalidIndex != ubIndex);
    const uint32_t ubTypeHash = shd->Setup.UniformBlockTypeHash(ubIndex);
    const int ubByteSize = shd->Setup.UniformBlockByteSize(ubIndex);
    o_assert(ubTypeHash == typeHash);
    o_assert(ubByteSize >= byteSize);
    #endif

    GLint glLoc = shd->getUniformBlockLocation(bindStage, bindSlot);
    if (-1 != glLoc) {
        int vec4Count = byteSize / 16;
        ::glUniform4fv(glLoc, vec4Count, (const GLfloat*)ptr);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyTextures(ShaderStage::Code bindStage, Oryol::_priv::texture **textures, int numTextures) {
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
        const int samplerIndex = shd->getSamplerIndex(bindStage, i);
        if (-1 != samplerIndex) {
            this->bindTexture(samplerIndex, tex->glTarget, tex->glTextures[tex->activeSlot]);
        }
    }
}

} // namespace _priv
} // namespace Oryol
