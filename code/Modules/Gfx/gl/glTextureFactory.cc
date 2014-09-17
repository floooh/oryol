//------------------------------------------------------------------------------
//  glTextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTextureFactory.h"
#include "Gfx/base/textureLoaderBase.h"
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glTypes.h"
#include "Gfx/Core/stateWrapper.h"
#include "Gfx/Core/texture.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Core/texturePool.h"
#include "Gfx/Attrs/DisplayAttrs.h"

namespace Oryol {
namespace _priv {

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
    o_assert((tex.GetState() == ResourceState::Setup) || (tex.GetState() == ResourceState::Pending));
    o_assert(!tex.GetSetup().ShouldSetupFromPixelData());
    
    // decide whether a loader needs to take over, or whether we handle this right here
    if (tex.GetSetup().ShouldSetupAsRenderTarget()) {
        this->createRenderTarget(tex);
        o_assert((tex.GetState() == ResourceState::Valid) || (tex.GetState() == ResourceState::Failed));
    }
    else {
        // let a loader take over, parent class will take care of this
        o_assert(tex.GetSetup().ShouldSetupFromFile());
        loaderFactory<texture, textureLoaderBase>::SetupResource(tex);
    }
}

//------------------------------------------------------------------------------
void
glTextureFactory::SetupResource(texture& tex, const Ptr<Stream>& data) {
    o_assert(this->isValid);
    o_assert(tex.GetState() == ResourceState::Setup);
    o_assert(!tex.GetSetup().ShouldSetupAsRenderTarget());
    o_assert(!tex.GetSetup().ShouldSetupFromFile());
    
    if (tex.GetSetup().ShouldSetupFromPixelData()) {
        this->createFromPixelData(tex, data);
        o_assert(tex.GetState() == ResourceState::Valid);
    }
    else {
        o_assert(tex.GetSetup().ShouldSetupFromImageFileData());
        loaderFactory<texture, textureLoaderBase>::SetupResource(tex, data);
    }
}

//------------------------------------------------------------------------------
void
glTextureFactory::DestroyResource(texture& tex) {
    o_assert(this->isValid);
    
    loaderFactory::DestroyResource(tex);
    
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
    tex.setState(ResourceState::Setup);
}

//------------------------------------------------------------------------------
/**
 @todo: support for depth texture (depends on depth_texture extension), support
 for multiple color texture attachments.
*/
void
glTextureFactory::createRenderTarget(texture& tex) {
    o_assert(tex.GetState() == ResourceState::Setup);
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
        width = int32(dispAttrs.FramebufferWidth * setup.RelWidth);
        height = int32(dispAttrs.FramebufferHeight * setup.RelHeight);
    }
    else if (setup.HasSharedDepth()) {
        // a shared-depth-buffer render target, obtain width and height
        // from the original render target
        texture* sharedDepthProvider = this->texPool->Lookup(setup.DepthRenderTarget.Id());
        o_assert(nullptr != sharedDepthProvider);
        width = sharedDepthProvider->GetTextureAttrs().Width;
        height = sharedDepthProvider->GetTextureAttrs().Height;
    }
    else {
        width = setup.Width;
        height = setup.Height;
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
    GLenum glColorFormat = glTypes::AsGLTexImageFormat(setup.ColorFormat);
    GLint glColorInternalFormat = glTypes::AsGLTexImageInternalFormat(setup.ColorFormat);
    GLenum glColorType = glTypes::AsGLTexImageType(setup.ColorFormat);
    GLuint glColorRenderTexture = 0;
    ::glGenTextures(1, &glColorRenderTexture);
    ::glActiveTexture(GL_TEXTURE0);
    ::glBindTexture(GL_TEXTURE_2D, glColorRenderTexture);
    ORYOL_GL_CHECK_ERROR();
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, setup.MinFilter);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, setup.MagFilter);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, setup.WrapU);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, setup.WrapV);
    #if !ORYOL_OPENGLES2
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0); // see: http://www.opengl.org/wiki/Hardware_specifics:_NVidia
    #endif
    ORYOL_GL_CHECK_ERROR();
    ::glTexImage2D(GL_TEXTURE_2D, 0, glColorInternalFormat, width, height, 0, glColorFormat, glColorType, NULL);
    ORYOL_GL_CHECK_ERROR();
    
    // attach color render texture to framebuffer
    ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glColorRenderTexture, 0);
    ORYOL_GL_CHECK_ERROR();
    
    // create depth buffer
    GLuint glDepthRenderBuffer = 0;
    if (setup.HasDepth()) {
        if (!setup.HasSharedDepth()) {
            // FIXME: optionally create a depth texture instead of a render buffer here...
            o_assert(PixelFormat::InvalidPixelFormat != setup.DepthFormat);
            
            ::glGenRenderbuffers(1, &glDepthRenderBuffer);
            ORYOL_GL_CHECK_ERROR();
            o_assert(0 != glDepthRenderBuffer);
            ::glBindRenderbuffer(GL_RENDERBUFFER, glDepthRenderBuffer);
            ORYOL_GL_CHECK_ERROR();
            GLint glDepthFormat = glTypes::AsGLRenderbufferFormat(setup.DepthFormat);
            ::glRenderbufferStorage(GL_RENDERBUFFER, glDepthFormat, width, height);
            ORYOL_GL_CHECK_ERROR();
            ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthRenderBuffer);
            ORYOL_GL_CHECK_ERROR();
            if (PixelFormat::IsDepthStencilFormat(setup.DepthFormat)) {
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
            if (PixelFormat::IsDepthStencilFormat(sharedDepthProvider->GetSetup().DepthFormat)) {
                ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glSharedDepthBuffer);
                ORYOL_GL_CHECK_ERROR();
            }
        }
    }
    
    // check framebuffer completeness
    if (::glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log::Warn("glTextureFactory::createRenderTarget(): framebuffer completeness check failed!\n");
    }
    this->glStateWrapper->InvalidateTextureState();
    
    // setup texture attrs and set on texture
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = TextureType::Texture2D;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.DepthFormat = setup.DepthFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = width;
    attrs.Height = height;
    attrs.HasMipmaps = false;
    attrs.IsRenderTarget = true;
    attrs.HasDepthBuffer = setup.HasDepth();
    attrs.HasSharedDepthBuffer = setup.HasSharedDepth();
    attrs.IsDepthTexture = false;
    
    // setup the texture object
    tex.setTextureAttrs(attrs);
    tex.glSetTexture(glColorRenderTexture);
    tex.glSetFramebuffer(glFramebuffer);
    tex.glSetDepthRenderbuffer(glDepthRenderBuffer);
    tex.glSetTarget(GL_TEXTURE_2D);
    tex.setState(ResourceState::Valid);
    
    // bind the default frame buffer
    ::glBindFramebuffer(GL_FRAMEBUFFER, glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glTextureFactory::createFromPixelData(texture& tex, const Ptr<Stream>& data) {
    o_assert(tex.GetState() == ResourceState::Setup);

    const TextureSetup& setup = tex.GetSetup();
    const int32 width = setup.Width;
    const int32 height = setup.Height;
    
    // create a texture object
    const GLuint glTex = this->glGenAndBindTexture(GL_TEXTURE_2D);
    
    // setup texture params
    GLenum glMinFilter = setup.MinFilter;
    GLenum glMagFilter = setup.MagFilter;
    if (!setup.HasMipMaps()) {
        #if !ORYOL_OPENGLES2
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0); // see: http://www.opengl.org/wiki/Hardware_specifics:_NVidia
        #endif
        if ((glMinFilter == GL_NEAREST_MIPMAP_NEAREST) || (glMinFilter == GL_NEAREST_MIPMAP_LINEAR)) {
            glMinFilter = GL_NEAREST;
        }
        else if ((glMinFilter == GL_LINEAR_MIPMAP_NEAREST) || (glMinFilter == GL_LINEAR_MIPMAP_LINEAR)) {
            glMinFilter = GL_LINEAR;
        }
    }
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilter);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilter);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, setup.WrapU);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, setup.WrapV);
    ORYOL_GL_CHECK_ERROR();
    
    // get pointer to image data
    data->Open(OpenMode::ReadOnly);
    const uint8* endPtr = nullptr;
    const uint8* srcPtr = data->MapRead(&endPtr);
    o_assert(endPtr == srcPtr + width * height * PixelFormat::ByteSize(setup.ColorFormat));
    
    // setup the image data in the texture
    o_assert2(!setup.HasMipMaps(), "Creating mipmap textures from pixel data not yet supported");
    GLenum glTexImageFormat = glTypes::AsGLTexImageFormat(setup.ColorFormat);
    GLenum glTexImageInternalFormat = glTypes::AsGLTexImageInternalFormat(setup.ColorFormat);
    GLenum glTexImageType   = glTypes::AsGLTexImageType(setup.ColorFormat);
    ::glTexImage2D(GL_TEXTURE_2D,
                   0,
                   glTexImageInternalFormat,
                   width,
                   height,
                   0,
                   glTexImageFormat,
                   glTexImageType,
                   srcPtr);
    ORYOL_GL_CHECK_ERROR();
    data->UnmapRead();
    data->Close();
    
    // setup texture attributes
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = TextureType::Texture2D;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = width;
    attrs.Height = height;
    attrs.HasMipmaps = setup.HasMipMaps();
    
    // setup texture
    tex.setTextureAttrs(attrs);
    tex.glSetTexture(glTex);
    tex.glSetTarget(GL_TEXTURE_2D);
    tex.setState(ResourceState::Valid);
}

//------------------------------------------------------------------------------
GLuint
glTextureFactory::glGenAndBindTexture(GLenum target) {
    o_assert(this->isValid);
    this->glStateWrapper->InvalidateTextureState();
    GLuint glTex = 0;
    ::glGenTextures(1, &glTex);
    ::glActiveTexture(GL_TEXTURE0);
    ::glBindTexture(target, glTex);
    ORYOL_GL_CHECK_ERROR();
    return glTex;
}

} // namespace _priv
} // namespace Oryol
