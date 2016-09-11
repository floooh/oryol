//------------------------------------------------------------------------------
//  glTextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "glTextureFactory.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Resource/resource.h"
#include "gl_impl.h"
#include "glTypes.h"
#include "glCaps.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glTextureFactory::glTextureFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glTextureFactory::~glTextureFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
glTextureFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
glTextureFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
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
    o_assert_dbg(!tex.Setup.ShouldSetupFromFile());

    if (tex.Setup.ShouldSetupAsRenderTarget()) {
        return this->createRenderTarget(tex);
    }
    else if (tex.Setup.ShouldSetupEmpty()) {
        return this->createEmptyTexture(tex);
    }
    else {
        // here would go more ways to create textures without image data
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
glTextureFactory::SetupResource(texture& tex, const void* data, int size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.ShouldSetupAsRenderTarget());
    
    if (tex.Setup.ShouldSetupFromPixelData()) {
        return this->createFromPixelData(tex, data, size);
    }
    else {
        // here would go more ways to create textures with image data
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
void
glTextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);
    
    this->pointers.renderer->invalidateTextureState();

    if (0 != tex.glFramebuffer) {
        ::glDeleteFramebuffers(1, &tex.glFramebuffer);
        ORYOL_GL_CHECK_ERROR();
    }

    if (0 != tex.glMSAAResolveFramebuffer) {
        ::glDeleteFramebuffers(1, &tex.glMSAAResolveFramebuffer);
        ORYOL_GL_CHECK_ERROR();
    }

    if (0 != tex.glMSAARenderbuffer) {
        ::glDeleteRenderbuffers(1, &tex.glMSAARenderbuffer);
        ORYOL_GL_CHECK_ERROR();
    }

    for (auto& glTex : tex.glTextures) {
        if (0 != glTex) {
            ::glDeleteTextures(1, &glTex);
            ORYOL_GL_CHECK_ERROR();
        }
    }
    
    if (!tex.textureAttrs.HasSharedDepthBuffer) {
        if (0 != tex.glDepthRenderbuffer) {
            ::glDeleteRenderbuffers(1, &tex.glDepthRenderbuffer);
            ORYOL_GL_CHECK_ERROR();
        }
    }
    
    tex.Clear();
}

//------------------------------------------------------------------------------
/**
 @todo:
    - multisample render targets (except GLES2)
    - multiple color attachments
*/
ResourceState::Code
glTextureFactory::createRenderTarget(texture& tex) {
    o_assert_dbg(0 == tex.glFramebuffer);
    o_assert_dbg(0 == tex.glDepthRenderbuffer);
    o_assert_dbg(0 == tex.glTextures[0]);

    GLint glOrigFramebuffer = 0;
    ::glGetIntegerv(GL_FRAMEBUFFER_BINDING, &glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();
    
    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.ShouldSetupAsRenderTarget());
    o_assert_dbg(setup.TextureUsage == Usage::Immutable);
    o_assert_dbg(setup.NumMipMaps == 1);
    o_assert_dbg(setup.Type == TextureType::Texture2D);
    o_assert_dbg(PixelFormat::IsValidRenderTargetColorFormat(setup.ColorFormat));

    // get size of new render target
    int width = setup.Width;
    int height = setup.Height;
    texture* sharedDepthProvider = nullptr;
    if (setup.HasSharedDepth()) {
        // a shared-depth-buffer render target, obtain width and height
        // from the original render target
        texture* sharedDepthProvider = this->pointers.texturePool->Lookup(setup.DepthRenderTarget);
        o_assert_dbg(nullptr != sharedDepthProvider);
        width = sharedDepthProvider->textureAttrs.Width;
        height = sharedDepthProvider->textureAttrs.Height;
    }
    o_assert_dbg((width > 0) && (height > 0));
    
    // create new framebuffer object
    GLuint glFramebuffer = 0;
    ::glGenFramebuffers(1, &glFramebuffer);
    o_assert_dbg(0 != glFramebuffer);
    ::glBindFramebuffer(GL_FRAMEBUFFER, glFramebuffer);
    ORYOL_GL_CHECK_ERROR();

    // create a backing texture, for non-MSAA render targets this is also the actual render target,
    // for MSAA render targets, the MSAA image will be resolved into this backing texture
    const GLenum glColorInternalFormat = glTypes::asGLTexImageInternalFormat(setup.ColorFormat);
    const GLuint glColorTexture = this->glGenAndBindTexture(GL_TEXTURE_2D);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glTypes::asGLTexFilterMode(setup.Sampler.MinFilter));
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glTypes::asGLTexFilterMode(setup.Sampler.MagFilter));
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glTypes::asGLTexWrapMode(setup.Sampler.WrapU));
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glTypes::asGLTexWrapMode(setup.Sampler.WrapV));
    #if !ORYOL_OPENGLES2
    if (!glCaps::IsFlavour(glCaps::GLES2)) {
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0); // see: http://www.opengl.org/wiki/Hardware_specifics:_NVidia
    }
    #endif
    ORYOL_GL_CHECK_ERROR();
    #if ORYOL_OPENGLES3    
    if (!glCaps::IsFlavour(glCaps::GLES2)) {
        ::glTexStorage2D(GL_TEXTURE_2D, 1, glColorInternalFormat, width, height);
    }
    else
    #endif
    {
        GLenum glColorFormat = glTypes::asGLTexImageFormat(setup.ColorFormat);
        GLenum glColorType = glTypes::asGLTexImageType(setup.ColorFormat);
        ::glTexImage2D(GL_TEXTURE_2D, 0, glColorInternalFormat, width, height, 0, glColorFormat, glColorType, NULL);
    }
    ORYOL_GL_CHECK_ERROR();

    // create MSAA render target if requested and possible
    #if !ORYOL_OPENGLES2
    const bool msaa = (setup.SampleCount > 1) && glCaps::HasFeature(glCaps::MSAARenderTargets);
    if (msaa) {
        // create a framebuffer 
        ::glGenRenderbuffers(1, &tex.glMSAARenderbuffer);
        o_assert_dbg(0 != tex.glMSAARenderbuffer);
        ::glBindRenderbuffer(GL_RENDERBUFFER, tex.glMSAARenderbuffer);
        ::glRenderbufferStorageMultisample(GL_RENDERBUFFER, setup.SampleCount, glColorInternalFormat, width, height);
        ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, tex.glMSAARenderbuffer);

        // create an extra framebuffer for the MSAA resolve operation
        ::glGenFramebuffers(1, &tex.glMSAAResolveFramebuffer);
        ::glBindFramebuffer(GL_FRAMEBUFFER, tex.glMSAAResolveFramebuffer);
        ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glColorTexture, 0);
        ::glBindFramebuffer(GL_FRAMEBUFFER, glFramebuffer);
    }
    else
    #endif
    {
        // non-MSAA: directly attach the backing texture to frame buffer
        ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glColorTexture, 0);
    }
    ORYOL_GL_CHECK_ERROR();
    
    // create depth buffer
    GLuint glDepthRenderBuffer = 0;
    if (setup.HasDepth()) {
        if (!setup.HasSharedDepth()) {
            o_assert_dbg(PixelFormat::IsValidTextureDepthFormat(setup.DepthFormat));

            // FIXME: optionally create a depth texture instead of a render buffer here...
            o_assert_dbg(PixelFormat::InvalidPixelFormat != setup.DepthFormat);
            
            ::glGenRenderbuffers(1, &glDepthRenderBuffer);
            o_assert_dbg(0 != glDepthRenderBuffer);
            ::glBindRenderbuffer(GL_RENDERBUFFER, glDepthRenderBuffer);
            GLint glDepthFormat = glTypes::asGLDepthAttachmentFormat(setup.DepthFormat);
            #if !ORYOL_OPENGLES2
            if (msaa) {
                ::glRenderbufferStorageMultisample(GL_RENDERBUFFER, setup.SampleCount, glDepthFormat, width, height);
            }
            else
            #endif
            {
                ::glRenderbufferStorage(GL_RENDERBUFFER, glDepthFormat, width, height);
            }
            ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthRenderBuffer);
            if (PixelFormat::IsDepthStencilFormat(setup.DepthFormat)) {
                ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glDepthRenderBuffer);
            }
        }
        else {
            // use shared depth buffer
            o_assert_dbg(nullptr != sharedDepthProvider);
            o_assert(setup.SampleCount == sharedDepthProvider->Setup.SampleCount);
            GLuint glSharedDepthBuffer = sharedDepthProvider->glDepthRenderbuffer;
            o_assert_dbg(0 != glSharedDepthBuffer);
            ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glSharedDepthBuffer);
            if (PixelFormat::IsDepthStencilFormat(sharedDepthProvider->Setup.DepthFormat)) {
                ::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glSharedDepthBuffer);
            }
        }
        ORYOL_GL_CHECK_ERROR();
    }
    
    // check framebuffer completeness
    if (::glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        o_warn("glTextureFactory::createRenderTarget(): framebuffer completeness check failed!\n");
    }
    this->pointers.renderer->invalidateTextureState();
    
    // setup texture attrs and set on texture
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = TextureType::Texture2D;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.DepthFormat = setup.DepthFormat;
    attrs.SampleCount = setup.SampleCount;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = width;
    attrs.Height = height;
    attrs.NumMipMaps = 1;
    attrs.IsRenderTarget = true;
    attrs.HasDepthBuffer = setup.HasDepth();
    attrs.HasSharedDepthBuffer = setup.HasSharedDepth();
    
    // setup the texture object
    tex.textureAttrs = attrs;
    tex.glTextures[0] = glColorTexture;
    tex.glFramebuffer = glFramebuffer;
    tex.glDepthRenderbuffer = glDepthRenderBuffer;
    tex.glTarget = GL_TEXTURE_2D;
    
    // bind the default frame buffer
    ::glBindFramebuffer(GL_FRAMEBUFFER, glOrigFramebuffer);
    ORYOL_GL_CHECK_ERROR();
    
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glTextureFactory::setupTextureParams(const TextureSetup& setup, GLenum glTexTarget, GLuint glTex) {
    GLenum glMinFilter = glTypes::asGLTexFilterMode(setup.Sampler.MinFilter);
    GLenum glMagFilter = glTypes::asGLTexFilterMode(setup.Sampler.MagFilter);
    if (1 == setup.NumMipMaps) {
        #if !ORYOL_OPENGLES2
        if (!glCaps::IsFlavour(glCaps::GLES2)) {
            ::glTexParameteri(glTexTarget, GL_TEXTURE_MAX_LEVEL, 0); // see: http://www.opengl.org/wiki/Hardware_specifics:_NVidia
        }
        #endif
        if ((glMinFilter == GL_NEAREST_MIPMAP_NEAREST) || (glMinFilter == GL_NEAREST_MIPMAP_LINEAR)) {
            glMinFilter = GL_NEAREST;
        }
        else if ((glMinFilter == GL_LINEAR_MIPMAP_NEAREST) || (glMinFilter == GL_LINEAR_MIPMAP_LINEAR)) {
            glMinFilter = GL_LINEAR;
        }
    }
    ::glTexParameteri(glTexTarget, GL_TEXTURE_MIN_FILTER, glMinFilter);
    ::glTexParameteri(glTexTarget, GL_TEXTURE_MAG_FILTER, glMagFilter);
    if (setup.Type == TextureType::TextureCube) {
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else {
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_S, glTypes::asGLTexWrapMode(setup.Sampler.WrapU));
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_T, glTypes::asGLTexWrapMode(setup.Sampler.WrapV));
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glTextureFactory::setupTextureAttrs(texture& tex) {
    TextureAttrs attrs;
    attrs.Locator = tex.Setup.Locator;
    attrs.Type = tex.Setup.Type;
    attrs.ColorFormat = tex.Setup.ColorFormat;
    attrs.TextureUsage = tex.Setup.TextureUsage;
    attrs.Width = tex.Setup.Width;
    attrs.Height = tex.Setup.Height;
    attrs.NumMipMaps = tex.Setup.NumMipMaps;
    tex.textureAttrs = attrs;
}

//------------------------------------------------------------------------------
ResourceState::Code
glTextureFactory::createFromPixelData(texture& tex, const void* data, int size) {
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);
    o_assert_dbg(0 == tex.glTextures[0]);

    const TextureSetup& setup = tex.Setup;
    const int width = setup.Width;
    const int height = setup.Height;
    o_assert_dbg(setup.TextureUsage == Usage::Immutable);
    
    // test if the texture format is actually supported
    if (!glCaps::HasTextureFormat(setup.ColorFormat)) {
        o_warn("glTextureFactory: unsupported texture format for resource '%s'\n", setup.Locator.Location().AsCStr());
        return ResourceState::Failed;
    }
    
    // create a texture object
    const GLenum glTextureTarget = glTypes::asGLTextureTarget(setup.Type);
    const GLuint glTex = this->glGenAndBindTexture(glTextureTarget);
    
    // setup texture params
    this->setupTextureParams(setup, glTextureTarget, glTex);

    // define texture storage (only if available)
    const int numFaces = setup.Type == TextureType::TextureCube ? 6 : 1;
    const int numMipMaps = setup.NumMipMaps;
    const bool isCompressed = PixelFormat::IsCompressedFormat(setup.ColorFormat);
    const GLenum glTexImageFormat = glTypes::asGLTexImageFormat(setup.ColorFormat);
    const GLenum glTexImageInternalFormat = glTypes::asGLTexImageInternalFormat(setup.ColorFormat);
    #if ORYOL_OPENGLES3
    if (!glCaps::IsFlavour(glCaps::GLES2)) {
        ::glTexStorage2D(glTextureTarget, numMipMaps, glTexImageInternalFormat, width, height);
        ORYOL_GL_CHECK_ERROR();
    }
    #endif

    // copy image data intp texture
    const uint8_t* srcPtr = (const uint8_t*) data;
    for (int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
        
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
    
        for (int mipIndex = 0; mipIndex < numMipMaps; mipIndex++) {
            o_assert_dbg(setup.ImageData.Sizes[faceIndex][mipIndex] > 0);
            int mipWidth = width >> mipIndex;
            if (mipWidth == 0) {
                mipWidth = 1;
            }
            int mipHeight = height >> mipIndex;
            if (mipHeight == 0) {
                mipHeight = 1;
            }
            GLvoid* mipDataPtr = (GLvoid*)(srcPtr + setup.ImageData.Offsets[faceIndex][mipIndex]);
            if (isCompressed) {
                // compressed texture data
                const GLsizei mipDataSize = setup.ImageData.Sizes[faceIndex][mipIndex];
                #if ORYOL_OPENGLES3
                if (!glCaps::IsFlavour(glCaps::GLES2)) {
                    ::glCompressedTexSubImage2D(glImgTarget,
                                                mipIndex, 0, 0, mipWidth, mipHeight,
                                                glTexImageFormat,
                                                mipDataSize, mipDataPtr);
                }
                else
                #endif
                {
                    ::glCompressedTexImage2D(glImgTarget,
                                             mipIndex,
                                             glTexImageInternalFormat,
                                             mipWidth,
                                             mipHeight,
                                             0,
                                             mipDataSize, mipDataPtr);
                }
                ORYOL_GL_CHECK_ERROR();
            }
            else {
                // uncompressed texture data
                const GLenum glTexImageType = glTypes::asGLTexImageType(setup.ColorFormat);
                #if ORYOL_OPENGLES3
                if (!glCaps::IsFlavour(glCaps::GLES2)) {
                    ::glTexSubImage2D(glImgTarget,
                                      mipIndex, 0, 0, mipWidth, mipHeight,
                                      glTexImageFormat,
                                      glTexImageType, mipDataPtr);
                }
                else
                #endif
                {
                    ::glTexImage2D(glImgTarget,
                                   mipIndex,
                                   glTexImageInternalFormat,
                                   mipWidth,
                                   mipHeight,
                                   0,
                                   glTexImageFormat,
                                   glTexImageType,
                                   mipDataPtr);
                }
                ORYOL_GL_CHECK_ERROR();
            }
        }
    }
    
    // setup texture object
    this->setupTextureAttrs(tex);
    tex.glTextures[0] = glTex;
    tex.glTarget = glTextureTarget;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glTextureFactory::createEmptyTexture(texture& tex) {
    o_assert_dbg(0 == tex.glTextures[0]);
    o_assert_dbg(0 == tex.glTextures[1]);

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.TextureUsage != Usage::Immutable);
    o_assert_dbg(setup.Type == TextureType::Texture2D);
    o_assert_dbg(!PixelFormat::IsCompressedFormat(setup.ColorFormat));
    const int width = setup.Width;
    const int height = setup.Height;
    const GLenum glTextureTarget = glTypes::asGLTextureTarget(setup.Type);
    const GLenum glTexImageInternalFormat = glTypes::asGLTexImageInternalFormat(setup.ColorFormat);
    const int numMipMaps = setup.NumMipMaps;
    const GLenum glTexImageFormat = glTypes::asGLTexImageFormat(setup.ColorFormat);
    const GLenum glTexImageType = glTypes::asGLTexImageType(setup.ColorFormat);

    // test if the texture format is actually supported
    if (!glCaps::HasTextureFormat(setup.ColorFormat)) {
        o_warn("glTextureFactory: unsupported texture format for resource '%s'\n", setup.Locator.Location().AsCStr());
        return ResourceState::Failed;
    }

    // create one or two texture object
    tex.numSlots = Usage::Stream == setup.TextureUsage ? 2 : 1;
    for (int slotIndex = 0; slotIndex < tex.numSlots; slotIndex++) {

        tex.glTextures[slotIndex] = this->glGenAndBindTexture(glTextureTarget);
        this->setupTextureParams(setup, glTextureTarget, tex.glTextures[slotIndex]);

        // initialize texture storage
        #if ORYOL_OPENGLES3
        if (!glCaps::IsFlavour(glCaps::GLES2)) {
            ::glTexStorage2D(glTextureTarget, numMipMaps, glTexImageInternalFormat, width, height);
        }
        else
        #endif
        {
            for (int mipIndex = 0; mipIndex < numMipMaps; mipIndex++) {
                int mipWidth = width >> mipIndex;
                if (mipWidth == 0) {
                    mipWidth = 1;
                }
                int mipHeight = height >> mipIndex;
                if (mipHeight == 0) {
                    mipHeight = 1;
                }
                ::glTexImage2D(glTextureTarget,
                               mipIndex,
                               glTexImageInternalFormat,
                               mipWidth,
                               mipHeight,
                               0,
                               glTexImageFormat,
                               glTexImageType,
                               nullptr);
            }
        }
        ORYOL_GL_CHECK_ERROR();
    }

    // setup texture object
    this->setupTextureAttrs(tex);
    tex.glTarget = glTextureTarget;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
GLuint
glTextureFactory::glGenAndBindTexture(GLenum target) {
    o_assert_dbg(this->isValid);

    this->pointers.renderer->invalidateTextureState();
    GLuint glTex = 0;
    ::glGenTextures(1, &glTex);
    ::glActiveTexture(GL_TEXTURE0);
    ::glBindTexture(target, glTex);
    ORYOL_GL_CHECK_ERROR();
    return glTex;
}

} // namespace _priv
} // namespace Oryol
