//------------------------------------------------------------------------------
//  glTextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTextureFactory.h"
#include "Render/base/textureLoaderBase.h"
#include "Render/gl/gl_impl.h"
#include "Render/gl/glTypes.h"
#include "Render/Core/StateWrapper.h"
#include "Render/Core/Texture.h"
#include "Render/Core/displayMgr.h"
#include "Render/Core/texturePool.h"
#include "Render/Attrs/DisplayAttrs.h"

namespace Oryol {
namespace Render {

using namespace Core;
using namespace IO;
    
//------------------------------------------------------------------------------
glTextureFactory::glTextureFactory() :
glStateWrapper(nullptr),
displayManager(nullptr),
texPool(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glTextureFactory::~glTextureFactory() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
glTextureFactory::Setup(stateWrapper* stateWrapper_, displayMgr* displayMgr_, texturePool* texPool_) {
    o_assert(!this->isValid);
    o_assert(nullptr != stateWrapper_);
    o_assert(nullptr != displayMgr_);
    o_assert(nullptr != texPool_);
    this->isValid = true;
    this->glStateWrapper = stateWrapper_;
    this->displayManager = displayMgr_;
    this->texPool = texPool_;
}

//------------------------------------------------------------------------------
void
glTextureFactory::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->glStateWrapper = nullptr;
    this->displayManager = nullptr;
}

//------------------------------------------------------------------------------
bool
glTextureFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
glTextureFactory::SetupResource(texture& tex) {
    o_assert(this->isValid);
    o_assert(tex.GetState() == Resource::State::Setup);
    
    // decide whether a loader needs to take over, or whether we handle this right here
    if (tex.GetSetup().ShouldSetupAsRenderTarget()) {
        this->createRenderTarget(tex);
        o_assert((tex.GetState() == Resource::State::Valid) || (tex.GetState() == Resource::State::Failed));
    }
    else {
        // let a loader take over, parent class will take care of this
        Resource::loaderFactory<texture, textureLoaderBase>::SetupResource(tex);
    }
}

//------------------------------------------------------------------------------
void
glTextureFactory::SetupResource(texture& tex, const Ptr<Stream>& data) {
    o_assert(this->isValid);
    o_assert(tex.GetState() == Resource::State::Setup);
    Resource::loaderFactory<texture, textureLoaderBase>::SetupResource(tex, data);
}

//------------------------------------------------------------------------------
void
glTextureFactory::DestroyResource(texture& tex) {
    o_assert(this->isValid);
    this->glStateWrapper->InvalidateTextureState();

    GLuint glFb = tex.glGetFramebuffer();
    if (0 != glFb) {
        ::glDeleteFramebuffers(1, &glFb);
        ORYOL_GL_CHECK_ERROR();
    }
        
    GLuint glTex = tex.glGetTexture();
    if (0 != glTex) {
        ::glDeleteTextures(1, &glTex);
        ORYOL_GL_CHECK_ERROR();
    }
    
    GLuint glDepthRenderBuffer = tex.glGetDepthRenderbuffer();
    if (0 != glDepthRenderBuffer) {
        ::glDeleteRenderbuffers(1, &glDepthRenderBuffer);
        ORYOL_GL_CHECK_ERROR();
    }
    
    GLuint glDepthTex = tex.glGetDepthTexture();
    if (0 != glDepthTex) {
        ::glDeleteTextures(1, &glDepthTex);
        ORYOL_GL_CHECK_ERROR();
    }
    
    tex.clear();
    tex.setState(Resource::State::Setup);
}

//------------------------------------------------------------------------------
/**
 @todo: support for depth texture (depends on depth_texture extension), support
 for multiple color texture attachments.
*/
void
glTextureFactory::createRenderTarget(texture& tex) {
    o_assert(tex.GetState() == Resource::State::Setup);
    o_assert(0 == tex.glGetTexture());
    o_assert(0 == tex.glGetFramebuffer());
    o_assert(0 == tex.glGetDepthRenderbuffer());
    o_assert(0 == tex.glGetDepthTexture());
    
    this->glStateWrapper->InvalidateTextureState();
    GLint glOrigFramebuffer = 0;
    ::glGetIntegerv(GL_FRAMEBUFFER_BINDING, &glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();
    
    const TextureSetup& setup = tex.GetSetup();
    o_assert(setup.ShouldSetupAsRenderTarget());

    // get size of new render target
    int32 width, height;
    texture* sharedDepthProvider = nullptr;
    if (setup.IsRelSizeRenderTarget()) {
        const DisplayAttrs& dispAttrs = this->displayManager->GetDisplayAttrs();
        width = int32(dispAttrs.GetFramebufferWidth() * setup.GetRelWidth());
        height = int32(dispAttrs.GetFramebufferHeight() * setup.GetRelHeight());
    }
    else if (setup.HasSharedDepth()) {
        // a shared-depth-buffer render target, obtain width and height
        // from the original render target
        texture* sharedDepthProvider = this->texPool->Lookup(setup.GetDepthRenderTarget());
        o_assert(nullptr != sharedDepthProvider);
        width = sharedDepthProvider->GetTextureAttrs().GetWidth();
        height = sharedDepthProvider->GetTextureAttrs().GetHeight();
    }
    else {
        width = setup.GetWidth();
        height = setup.GetHeight();
    }
    o_assert((width > 0) && (height > 0));
    
    // create new framebuffer object
    GLuint glFramebuffer = 0;
    ::glGenFramebuffers(1, &glFramebuffer);
    o_assert(0 != glFramebuffer);
    ORYOL_GL_CHECK_ERROR();
    ::glBindFramebuffer(GL_FRAMEBUFFER, glFramebuffer);
    ORYOL_GL_CHECK_ERROR();
    
    // create render target texture
    GLint glColorFormat = glTypes::AsGLTexImageFormat(setup.GetColorFormat());
    GLenum glColorType = glTypes::AsGLTexImageType(setup.GetColorFormat());
    GLuint glColorRenderTexture = 0;
    ::glGenTextures(1, &glColorRenderTexture);
    ::glBindTexture(GL_TEXTURE_2D, glColorRenderTexture);
    ::glActiveTexture(GL_TEXTURE0);
    ORYOL_GL_CHECK_ERROR();
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glTypes::AsGLTextureFilterMode(setup.GetMinFilter()));
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glTypes::AsGLTextureFilterMode(setup.GetMagFilter()));
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glTypes::AsGLTextureWrapMode(setup.GetWrapU()));
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glTypes::AsGLTextureWrapMode(setup.GetWrapV()));
    ORYOL_GL_CHECK_ERROR();
    ::glTexImage2D(GL_TEXTURE_2D, 0, glColorFormat, width, height, 0, glColorFormat, glColorType, NULL);
    ORYOL_GL_CHECK_ERROR();
    
    // attach color render texture to framebuffer
    ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glColorRenderTexture, 0);
    ORYOL_GL_CHECK_ERROR();
    
    // create depth buffer
    GLuint glDepthRenderBuffer = 0;
    if (setup.HasDepth()) {
        if (!setup.HasSharedDepth()) {
            // FIXME: optionally create a depth texture instead of a render buffer here...
            o_assert(PixelFormat::InvalidPixelFormat != setup.GetDepthFormat());
            
            ::glGenRenderbuffers(1, &glDepthRenderBuffer);
            ORYOL_GL_CHECK_ERROR();
            o_assert(0 != glDepthRenderBuffer);
            ::glBindRenderbuffer(GL_RENDERBUFFER, glDepthRenderBuffer);
            ORYOL_GL_CHECK_ERROR();
            GLint glDepthFormat = glTypes::AsGLRenderbufferFormat(setup.GetDepthFormat());
            ::glRenderbufferStorage(GL_RENDERBUFFER, glDepthFormat, width, height);
            ORYOL_GL_CHECK_ERROR();
            ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthRenderBuffer);
            ORYOL_GL_CHECK_ERROR();
            if (PixelFormat::IsDepthStencilFormat(setup.GetDepthFormat())) {
                ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glDepthRenderBuffer);
                ORYOL_GL_CHECK_ERROR();
            }
        }
        else {
            // use shared depth buffer
            o_assert(nullptr != sharedDepthProvider);
            GLuint glSharedDepthBuffer = sharedDepthProvider->glGetDepthRenderbuffer();
            o_assert(0 != glSharedDepthBuffer);
            ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glSharedDepthBuffer);
            ORYOL_GL_CHECK_ERROR();
            if (PixelFormat::IsDepthStencilFormat(sharedDepthProvider->GetSetup().GetDepthFormat())) {
                ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glSharedDepthBuffer);
                ORYOL_GL_CHECK_ERROR();
            }
        }
    }
    
    // check framebuffer completeness
    if (::glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log::Warn("glTextureFactory::createRenderTarget(): framebuffer completeness check failed!\n");
    }
    
    // setup texture attrs and set on texture
    TextureAttrs attrs;
    attrs.setLocator(setup.GetLocator());
    attrs.setType(TextureType::Texture2D);
    attrs.setColorFormat(setup.GetColorFormat());
    attrs.setDepthFormat(setup.GetDepthFormat());
    attrs.setUsage(Usage::Immutable);
    attrs.setWidth(width);
    attrs.setHeight(height);
    attrs.setMipmapsFlag(false);
    attrs.setRenderTargetFlag(true);
    attrs.setDepthBufferFlag(setup.HasDepth());
    attrs.setSharedDepthBufferFlag(setup.HasSharedDepth());
    attrs.setDepthTextureFlag(false);
    
    // setup the texture object
    tex.setTextureAttrs(attrs);
    tex.glSetTexture(glColorRenderTexture);
    tex.glSetFramebuffer(glFramebuffer);
    tex.glSetDepthRenderbuffer(glDepthRenderBuffer);
    tex.glSetTarget(GL_TEXTURE_2D);
    tex.setState(Resource::State::Valid);
    
    // bind the default frame buffer
    ::glBindFramebuffer(GL_FRAMEBUFFER, glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();
}

} // namespace Render
} // namespace Oryol