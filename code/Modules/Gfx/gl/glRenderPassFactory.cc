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
ResourceState::Code
glRenderPassFactory::SetupResource(renderPass& rp) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(0 == rp.glFramebuffer);

    renderPassFactoryBase::SetupResource(rp);

    GLint glOrigFramebuffer = 0;
    ::glGetIntegerv(GL_FRAMEBUFFER_BINDING, &glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();

    // create new framebuffer object
    ::glGenFramebuffers(1, &rp.glFramebuffer);
    o_assert_dbg(0 != rp.glFramebuffer);
    ::glBindFramebuffer(GL_FRAMEBUFFER, rp.glFramebuffer);
    ORYOL_GL_CHECK_ERROR();

    // FIXME: MSAA!!!

    // attach color textures
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        texture* colorTex = rp.colorTextures[i];
        if (colorTex) {
            // FIXME: other texture types than Texture2D!
            const GLuint glTex = colorTex->glTextures[0];
            o_assert_dbg(glTex);
            const int mipLevel = rp.Setup.ColorAttachments[i].Level;
            ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, glTex, mipLevel);
            ORYOL_GL_CHECK_ERROR();
        }
        else {
            break;
        }
    }

    // attach depth-stencil buffer
    texture* dsTex = rp.depthStencilTexture;
    if (dsTex) {
        const GLuint glDepthRenderBuffer = dsTex->glDepthRenderbuffer;
        o_assert_dbg(glDepthRenderBuffer);
        ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthRenderBuffer);
        if (PixelFormat::IsDepthStencilFormat(dsTex->Setup.DepthFormat)) {
            ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glDepthRenderBuffer);
        }
    }

    // check framebuffer completeness
    if (::glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        o_warn("glTextureFactory::createRenderTarget(): framebuffer completeness check failed!\n");
    }

    // bind the default frame buffer
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
        ORYOL_GL_CHECK_ERROR();
    }

    renderPassFactoryBase::DestroyResource(rp);
}

} // namespace _priv
} // namespace Oryol
