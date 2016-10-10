//------------------------------------------------------------------------------
//  glRenderPassFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glRenderPassFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/resource.h"
#include "gl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
static void
attachFramebufferTextures(const renderPass& rp) {
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        texture* colorTex = rp.colorTextures[i];
        if (colorTex) {
            // FIXME: other texture types than Texture2D!
            const GLuint glTex = colorTex->glTextures[0];
            o_assert_dbg(glTex);
            const int mipLevel = rp.Setup.ColorAttachments[i].Level;
            ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, glTex, mipLevel);
        }
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
static void
attachFramebufferRenderbuffers(const renderPass& rp) {
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        texture* colorTex = rp.colorTextures[i];
        if (colorTex) {
            const GLuint glRenderBuf = colorTex->glMSAARenderbuffer;
            o_assert_dbg(glRenderBuf);
            ::glBindRenderbuffer(GL_RENDERBUFFER, glRenderBuf);
            ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_RENDERBUFFER, glRenderBuf);
        }
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
static void
attachFramebufferDepthStencilBuffer(const renderPass& rp) {
    texture* dsTex = rp.depthStencilTexture;
    if (dsTex) {
        const GLuint glDepthRenderBuffer = dsTex->glDepthRenderbuffer;
        o_assert_dbg(glDepthRenderBuffer);
        ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthRenderBuffer);
        if (PixelFormat::IsDepthStencilFormat(dsTex->Setup.DepthFormat)) {
            ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glDepthRenderBuffer);
        }
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
glRenderPassFactory::SetupResource(renderPass& rp) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(0 == rp.glFramebuffer);

    renderPassFactoryBase::SetupResource(rp);
    o_assert_dbg(rp.colorTextures[0]);
    const bool isMSAA = 0 != rp.colorTextures[0]->glMSAARenderbuffer;

    GLint glOrigFramebuffer = 0;
    ::glGetIntegerv(GL_FRAMEBUFFER_BINDING, &glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();

    // create new framebuffer object
    ::glGenFramebuffers(1, &rp.glFramebuffer);
    o_assert_dbg(0 != rp.glFramebuffer);
    ::glBindFramebuffer(GL_FRAMEBUFFER, rp.glFramebuffer);
    ORYOL_GL_CHECK_ERROR();

    // attach color textures or MSAA render buffers, and depth-stencil buffer
    if (isMSAA) {
        attachFramebufferRenderbuffers(rp);
    }
    else {
        attachFramebufferTextures(rp);
    }
    attachFramebufferDepthStencilBuffer(rp);

    // check framebuffer completeness
    if (::glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        o_warn("glRenderPassFactory: framebuffer completeness check failed!\n");
    }

    // for MSAA, need to create resolve-framebuffers...
    if (isMSAA) {
        for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
            texture* colorTex = rp.colorTextures[i];
            if (colorTex) {
                ::glGenFramebuffers(1, &rp.glMSAAResolveFramebuffers[i]);
                ::glBindFramebuffer(GL_FRAMEBUFFER, rp.glMSAAResolveFramebuffers[i]);
                const GLuint glTex = colorTex->glTextures[0];
                o_assert_dbg(glTex);
                const int mipLevel = rp.Setup.ColorAttachments[i].Level;
                ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glTex, mipLevel);
                if (::glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                    o_warn("glRenderPassFactory: MSAA resolve framebuffer completeness check failed!\n");
                }
            }
        }
    }

    // restore default framebuffer binding
    ::glBindFramebuffer(GL_FRAMEBUFFER, glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();
    this->pointers.renderer->invalidateTextureState();

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glRenderPassFactory::DestroyResource(renderPass& rp) {
    o_assert_dbg(this->isValid);

    if (0 != rp.glFramebuffer) {
        ::glDeleteFramebuffers(1, &rp.glFramebuffer);
    }
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        if (rp.glMSAAResolveFramebuffers[i]) {
            ::glDeleteFramebuffers(1, &(rp.glMSAAResolveFramebuffers[i]));
        }
    }
    ORYOL_GL_CHECK_ERROR();

    renderPassFactoryBase::DestroyResource(rp);
}

} // namespace _priv
} // namespace Oryol
