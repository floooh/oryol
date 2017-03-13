//------------------------------------------------------------------------------
//  glRenderPassFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glRenderPassFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/resource.h"
#include "Gfx/gl/glTypes.h"
#include "gl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
static void
attachFramebufferTextures(const renderPass& rp) {
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        texture* colorTex = rp.colorTextures[i];
        if (colorTex) {
            const GLuint glTex = colorTex->glTextures[0];
            o_assert_dbg(glTex);
            const auto& att = rp.Setup.ColorAttachments[i];
            switch (colorTex->textureAttrs.Type) {
                case TextureType::Texture2D:
                    ::glFramebufferTexture2D(GL_FRAMEBUFFER,
                                             GL_COLOR_ATTACHMENT0+i,
                                             GL_TEXTURE_2D,
                                             glTex,
                                             att.MipLevel);
                    break;
                case TextureType::TextureCube:
                    ::glFramebufferTexture2D(GL_FRAMEBUFFER,
                                             GL_COLOR_ATTACHMENT0+i,
                                             glTypes::asGLCubeFaceTarget(att.Slice),
                                             glTex,
                                             att.MipLevel);
                    break;
                default:
                    #if !ORYOL_OPENGLES2
                    // 3D- and 2D-array-texture
                    ::glFramebufferTextureLayer(GL_FRAMEBUFFER,
                                                GL_COLOR_ATTACHMENT0,
                                                glTex,
                                                att.MipLevel,
                                                att.Slice);
                    #endif
                    break;
            }
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
static void
checkFramebufferCompleteness() {
    const GLenum status = ::glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        const char* reason;
        switch (status) {
            #if !ORYOL_OPENGLES2
            case GL_FRAMEBUFFER_UNDEFINED: reason = "GL_FRAMEBUFFER_UNDEFINED"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: reason = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
            #endif
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: reason = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: reason = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: reason = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
            #if ORYOL_OPENGLES2
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: reason = "GL_FRAMEBUFFER_IMCOMPLETE_DIMENSIONS"; break;
            #endif
            default: reason = "unknown";
        }
        o_warn("glRenderPassFactory: framebuffer completeness check failed with '%s'\n", reason);
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
    checkFramebufferCompleteness();

    // for MSAA, need to create resolve-framebuffers...
    if (isMSAA) {
        for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
            texture* colorTex = rp.colorTextures[i];
            if (colorTex) {
                ::glGenFramebuffers(1, &rp.glMSAAResolveFramebuffers[i]);
                ::glBindFramebuffer(GL_FRAMEBUFFER, rp.glMSAAResolveFramebuffers[i]);
                const GLuint glTex = colorTex->glTextures[0];
                o_assert_dbg(glTex);
                const auto& att = rp.Setup.ColorAttachments[i];
                switch (colorTex->textureAttrs.Type) {
                    case TextureType::Texture2D:
                        ::glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                 GL_COLOR_ATTACHMENT0,
                                                 GL_TEXTURE_2D,
                                                 glTex,
                                                 att.MipLevel);
                        break;
                    case TextureType::TextureCube:
                        ::glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                 GL_COLOR_ATTACHMENT0,
                                                 glTypes::asGLCubeFaceTarget(att.Slice),
                                                 glTex,
                                                 att.MipLevel);
                        break;
                    default:
                        #if !ORYOL_OPENGLES2
                        // 2D-array and 3D texture
                        ::glFramebufferTextureLayer(GL_FRAMEBUFFER,
                                                    GL_COLOR_ATTACHMENT0,
                                                    glTex,
                                                    att.MipLevel,
                                                    att.Slice);
                        #endif
                        break;
                }
                checkFramebufferCompleteness();
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
