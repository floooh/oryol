//------------------------------------------------------------------------------
//  glTextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "glTextureFactory.h"
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glTypes.h"
#include "Gfx/gl/glExt.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Resource/texturePool.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Attrs/DisplayAttrs.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glTextureFactory::glTextureFactory() :
renderer(nullptr),
displayManager(nullptr),
texPool(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glTextureFactory::~glTextureFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
glTextureFactory::Setup(class renderer* rendr, displayMgr* displayMgr_, texturePool* texPool_) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr != rendr);
    o_assert_dbg(nullptr != displayMgr_);
    o_assert_dbg(nullptr != texPool_);
    o_assert_dbg(Core::IsMainThread());

    this->isValid = true;
    this->renderer = rendr;
    this->displayManager = displayMgr_;
    this->texPool = texPool_;
}

//------------------------------------------------------------------------------
void
glTextureFactory::Discard() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(Core::IsMainThread());

    this->isValid = false;
    this->renderer = nullptr;
    this->displayManager = nullptr;
}

//------------------------------------------------------------------------------
bool
glTextureFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glTextureFactory::SetupResource(texture& tex) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.ShouldSetupFromPixelData());
    o_assert_dbg(Core::IsMainThread());
    
    // decide whether a loader needs to take over, or whether we handle this right here
    if (tex.Setup.ShouldSetupAsRenderTarget()) {
        return this->createRenderTarget(tex);
    }
    else {
        o_error("FIXME FIXME FIXME");
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
glTextureFactory::SetupResource(texture& tex, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.ShouldSetupAsRenderTarget());
    o_assert_dbg(!tex.Setup.ShouldSetupFromFile());
    o_assert_dbg(Core::IsMainThread());
    
    if (tex.Setup.ShouldSetupFromPixelData()) {
        return this->createFromPixelData(tex, data, size);
    }
    else {
        o_error("FIXME FIXME FIXME");
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
void
glTextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(Core::IsMainThread());
    
    this->renderer->invalidateTextureState();

    if (0 != tex.glFramebuffer) {
        ::glDeleteFramebuffers(1, &tex.glFramebuffer);
        ORYOL_GL_CHECK_ERROR();
    }
        
    if (0 != tex.glTex) {
        ::glDeleteTextures(1, &tex.glTex);
        ORYOL_GL_CHECK_ERROR();
    }
    
    if (0 != tex.glDepthRenderbuffer) {
        ::glDeleteRenderbuffers(1, &tex.glDepthRenderbuffer);
        ORYOL_GL_CHECK_ERROR();
    }
    
    if (0 != tex.glDepthTexture) {
        ::glDeleteTextures(1, &tex.glDepthTexture);
        ORYOL_GL_CHECK_ERROR();
    }
    
    tex.Clear();
}

//------------------------------------------------------------------------------
/**
 @todo: support for depth texture (depends on depth_texture extension), support
 for multiple color texture attachments.
*/
ResourceState::Code
glTextureFactory::createRenderTarget(texture& tex) {
    o_assert_dbg(0 == tex.glTex);
    o_assert_dbg(0 == tex.glFramebuffer);
    o_assert_dbg(0 == tex.glDepthRenderbuffer);
    o_assert_dbg(0 == tex.glDepthTexture);
    o_assert_dbg(Core::IsMainThread());
    
    this->renderer->invalidateTextureState();
    GLint glOrigFramebuffer = 0;
    ::glGetIntegerv(GL_FRAMEBUFFER_BINDING, &glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();
    
    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.ShouldSetupAsRenderTarget());

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
        texture* sharedDepthProvider = this->texPool->Lookup(setup.DepthRenderTarget);
        o_assert_dbg(nullptr != sharedDepthProvider);
        width = sharedDepthProvider->textureAttrs.Width;
        height = sharedDepthProvider->textureAttrs.Height;
    }
    else {
        width = setup.Width;
        height = setup.Height;
    }
    o_assert_dbg((width > 0) && (height > 0));
    
    // create new framebuffer object
    GLuint glFramebuffer = 0;
    ::glGenFramebuffers(1, &glFramebuffer);
    o_assert_dbg(0 != glFramebuffer);
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
            o_assert_dbg(PixelFormat::InvalidPixelFormat != setup.DepthFormat);
            
            ::glGenRenderbuffers(1, &glDepthRenderBuffer);
            ORYOL_GL_CHECK_ERROR();
            o_assert_dbg(0 != glDepthRenderBuffer);
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
            o_assert_dbg(nullptr != sharedDepthProvider);
            GLuint glSharedDepthBuffer = sharedDepthProvider->glDepthRenderbuffer;
            o_assert_dbg(0 != glSharedDepthBuffer);
            ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glSharedDepthBuffer);
            ORYOL_GL_CHECK_ERROR();
            if (PixelFormat::IsDepthStencilFormat(sharedDepthProvider->Setup.DepthFormat)) {
                ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glSharedDepthBuffer);
                ORYOL_GL_CHECK_ERROR();
            }
        }
    }
    
    // check framebuffer completeness
    if (::glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        o_warn("glTextureFactory::createRenderTarget(): framebuffer completeness check failed!\n");
    }
    this->renderer->invalidateTextureState();
    
    // setup texture attrs and set on texture
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = TextureType::Texture2D;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.DepthFormat = setup.DepthFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = width;
    attrs.Height = height;
    attrs.NumMipMaps = 1;
    attrs.IsRenderTarget = true;
    attrs.HasDepthBuffer = setup.HasDepth();
    attrs.HasSharedDepthBuffer = setup.HasSharedDepth();
    attrs.IsDepthTexture = false;
    
    // setup the texture object
    tex.textureAttrs = attrs;
    tex.glTex = glColorRenderTexture;
    tex.glFramebuffer = glFramebuffer;
    tex.glDepthRenderbuffer = glDepthRenderBuffer;
    tex.glTarget = GL_TEXTURE_2D;
    
    // bind the default frame buffer
    ::glBindFramebuffer(GL_FRAMEBUFFER, glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();
    
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glTextureFactory::createFromPixelData(texture& tex, const void* data, int32 size) {
    o_assert_dbg(Core::IsMainThread());
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);

    const TextureSetup& setup = tex.Setup;
    const int32 width = setup.Width;
    const int32 height = setup.Height;
    
    // test if the texture format is actually supported
    if (!glExt::IsTextureFormatSupported(setup.ColorFormat)) {
        o_warn("glTextureFactory: unsupported texture format for resource '%s'\n", setup.Locator.Location().AsCStr());
        return ResourceState::Failed;
    }
    
    // create a texture object
    GLenum glTextureTarget = setup.Type;
    const GLuint glTex = this->glGenAndBindTexture(glTextureTarget);
    
    // setup texture params
    GLenum glMinFilter = setup.MinFilter;
    GLenum glMagFilter = setup.MagFilter;
    if (1 == setup.NumMipMaps) {
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
    if (setup.Type == TextureType::TextureCube) {
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else {
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, setup.WrapU);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, setup.WrapV);
    }
    ORYOL_GL_CHECK_ERROR();
    
    // get pointer to image data
    const uint8* srcPtr = (const uint8*) data;
    const int32 numFaces = setup.Type == TextureType::TextureCube ? 6 : 1;
    const int32 numMipMaps = setup.NumMipMaps;
    const bool isCompressed = PixelFormat::IsCompressedFormat(setup.ColorFormat);
    GLenum glTexImageInternalFormat = glTypes::AsGLTexImageInternalFormat(setup.ColorFormat);
    for (int32 faceIndex = 0; faceIndex < numFaces; faceIndex++) {
        
        GLenum glImgTarget;
        if (TextureType::TextureCube == setup.Type) {
            switch (faceIndex) {
                case 0: glImgTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
                case 1: glImgTarget = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
                case 2: glImgTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
                case 3: glImgTarget = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
                case 4: glImgTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;
                default: glImgTarget = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
            }
        }
        else {
            glImgTarget = glTextureTarget;
        }
    
        for (int32 mipIndex = 0; mipIndex < numMipMaps; mipIndex++) {
            o_assert_dbg(setup.ImageSizes[faceIndex][mipIndex] > 0);
            int32 mipWidth = width >> mipIndex;
            if (mipWidth == 0) mipWidth = 1;
            int32 mipHeight = height >> mipIndex;
            if (mipHeight == 0) mipHeight = 1;
            if (isCompressed) {
                // compressed texture data
                ::glCompressedTexImage2D(glImgTarget,
                                         mipIndex,
                                         glTexImageInternalFormat,
                                         mipWidth,
                                         mipHeight,
                                         0,
                                         setup.ImageSizes[faceIndex][mipIndex],
                                         srcPtr + setup.ImageOffsets[faceIndex][mipIndex]);
                ORYOL_GL_CHECK_ERROR();
            }
            else {
                // uncompressed texture data
                GLenum glTexImageFormat = glTypes::AsGLTexImageFormat(setup.ColorFormat);
                GLenum glTexImageType = glTypes::AsGLTexImageType(setup.ColorFormat);
                ::glTexImage2D(glImgTarget,
                               mipIndex,
                               glTexImageInternalFormat,
                               mipWidth,
                               mipHeight,
                               0,
                               glTexImageFormat,
                               glTexImageType,
                               srcPtr + setup.ImageOffsets[faceIndex][mipIndex]);
                ORYOL_GL_CHECK_ERROR();
            }
        }
    }
    
    // setup texture attributes
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = setup.Type;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = width;
    attrs.Height = height;
    attrs.NumMipMaps = setup.NumMipMaps;
    
    // setup texture
    tex.textureAttrs = attrs;
    tex.glTex = glTex;
    tex.glTarget = setup.Type;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
GLuint
glTextureFactory::glGenAndBindTexture(GLenum target) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(Core::IsMainThread());

    this->renderer->invalidateTextureState();
    GLuint glTex = 0;
    ::glGenTextures(1, &glTex);
    ::glActiveTexture(GL_TEXTURE0);
    ::glBindTexture(target, glTex);
    ORYOL_GL_CHECK_ERROR();
    return glTex;
}

} // namespace _priv
} // namespace Oryol
