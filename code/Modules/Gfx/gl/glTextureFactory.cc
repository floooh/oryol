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
#include <math.h>

namespace Oryol {
namespace _priv {

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
glTextureFactory::SetupResource(texture& tex, const void* data, int size) {
    o_assert_dbg(this->isValid);
    return this->createTexture(tex, data, size);
}

//------------------------------------------------------------------------------
void
glTextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);
    
    this->pointers.renderer->invalidateTextureState();
    if (!tex.nativeHandles) {
        for (auto& glTex : tex.glTextures) {
            if (0 != glTex) {
                ::glDeleteTextures(1, &glTex);
                ORYOL_GL_CHECK_ERROR();
            }
        }
    }
    if (0 != tex.glMSAARenderbuffer) {
        ::glDeleteRenderbuffers(1, &tex.glMSAARenderbuffer);
        ORYOL_GL_CHECK_ERROR();
    }
    if (0 != tex.glDepthRenderbuffer) {
        ::glDeleteRenderbuffers(1, &tex.glDepthRenderbuffer);
        ORYOL_GL_CHECK_ERROR();
    }
   tex.Clear();
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
        #if !ORYOL_OPENGLES2
        if (!glCaps::IsFlavour(glCaps::GLES2)) {
            if (setup.Type == TextureType::Texture3D) {
                ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_R, glTypes::asGLTexWrapMode(setup.Sampler.WrapW));
            }
        }
        #endif
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
    attrs.DepthFormat = tex.Setup.DepthFormat;
    attrs.SampleCount = tex.Setup.SampleCount;
    attrs.TextureUsage = tex.Setup.TextureUsage;
    attrs.Width = tex.Setup.Width;
    attrs.Height = tex.Setup.Height;
    attrs.Depth = tex.Setup.Depth;
    attrs.NumMipMaps = tex.Setup.NumMipMaps;
    attrs.IsRenderTarget = tex.Setup.IsRenderTarget;
    attrs.HasDepthBuffer = tex.Setup.HasDepth();
    tex.textureAttrs = attrs;
}

//------------------------------------------------------------------------------
ResourceState::Code
glTextureFactory::createTexture(texture& tex, const void* data, int size) {
    o_assert_dbg(0 == tex.glTextures[0]);
    o_assert_dbg(0 == tex.glTextures[1]);
    ORYOL_GL_CHECK_ERROR();

    const TextureSetup& setup = tex.Setup;

    // test if the texture format is actually supported
    if (!glCaps::HasTextureFormat(setup.ColorFormat)) {
        o_warn("glTextureFactory: unsupported texture format for resource '%s'\n", setup.Locator.Location().AsCStr());
        return ResourceState::Failed;
    }

    // check required features
    if ((setup.Type == TextureType::Texture3D) && (!glCaps::HasFeature(glCaps::Texture3D))) {
        o_warn("glTextureFactory: 3D textures not supported\n");
        return ResourceState::Failed;
    }
    if ((setup.Type == TextureType::TextureArray) && (!glCaps::HasFeature(glCaps::TextureArray))) {
        o_warn("glTextureFactory: array textures not supported\n");
        return ResourceState::Failed;
    }

    // create one or two texture object
    tex.numSlots = (Usage::Stream == setup.TextureUsage) ? 2 : 1;
    #if ORYOL_DEBUG
    // initialize with data is only allowed for immutable texture
    if (data) {
        o_assert_dbg(setup.TextureUsage == Usage::Immutable);
        o_assert_dbg(setup.ImageData.NumMipMaps > 0);
        o_assert_dbg(setup.ImageData.NumFaces > 0);
    }
    #endif
    const GLenum glTexImageInternalFormat = glTypes::asGLTexImageInternalFormat(setup.ColorFormat);
    const GLenum glTextureTarget = glTypes::asGLTextureTarget(setup.Type);
    if (tex.Setup.ShouldSetupFromNativeTexture()) {
        // existing native GL texture object provided
        tex.nativeHandles = true;
        tex.glTextures[0] = (GLuint) tex.Setup.NativeHandle[0];
        tex.glTextures[1] = (GLuint) tex.Setup.NativeHandle[1];
    }
    else {
        // create GL texture objects
        const GLenum glTexImageFormat = glTypes::asGLTexImageFormat(setup.ColorFormat);
        const bool isCompressed = PixelFormat::IsCompressedFormat(setup.ColorFormat);
        const uint8_t* srcPtr = (const uint8_t*) data;
        for (int slotIndex = 0; slotIndex < tex.numSlots; slotIndex++) {

            tex.glTextures[slotIndex] = this->glGenAndBindTexture(glTextureTarget);
            this->setupTextureParams(setup, glTextureTarget, tex.glTextures[slotIndex]);

            const int numFaces = setup.Type == TextureType::TextureCube ? 6 : 1;
            for (int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
                GLenum glImgTarget;
                if (TextureType::TextureCube == setup.Type) {
                    glImgTarget = glTypes::asGLCubeFaceTarget(faceIndex);
                }
                else {
                    glImgTarget = glTextureTarget;
                }
                for (int mipIndex = 0; mipIndex < setup.NumMipMaps; mipIndex++) {
                    GLvoid* mipDataPtr = nullptr;
                    int mipDataSize = 0;
                    if (srcPtr) {
                        if (mipIndex < setup.ImageData.NumMipMaps) {
                            mipDataPtr = (GLvoid*)(srcPtr + setup.ImageData.Offsets[faceIndex][mipIndex]);
                            mipDataSize = setup.ImageData.Sizes[faceIndex][mipIndex];
                            o_assert_dbg(mipDataSize > 0);
                        }
                    }
                    int mipWidth = setup.Width >> mipIndex;
                    if (mipWidth == 0) {
                        mipWidth = 1;
                    }
                    int mipHeight = setup.Height >> mipIndex;
                    if (mipHeight == 0) {
                        mipHeight = 1;
                    }
                    if ((TextureType::Texture2D == setup.Type) || (TextureType::TextureCube == setup.Type)) {
                        if (isCompressed) {
                            ::glCompressedTexImage2D(glImgTarget,
                                                    mipIndex,
                                                    glTexImageInternalFormat,
                                                    mipWidth, mipHeight,
                                                    0,
                                                    mipDataSize, mipDataPtr);
                            ORYOL_GL_CHECK_ERROR();
                        }
                        else {
                            const GLenum glTexImageType = glTypes::asGLTexImageType(setup.ColorFormat);
                            ::glTexImage2D(glImgTarget,
                                        mipIndex,
                                        glTexImageInternalFormat,
                                        mipWidth, mipHeight,
                                        0,
                                        glTexImageFormat,
                                        glTexImageType,
                                        mipDataPtr);
                            ORYOL_GL_CHECK_ERROR();
                        }
                    }
                    #if !ORYOL_OPENGLES2
                    else if ((TextureType::Texture3D == setup.Type) || (TextureType::TextureArray == setup.Type)) {
                        int mipDepth = setup.Depth >> mipIndex;
                        if (mipDepth == 0) {
                            mipDepth = 1;
                        }
                        if (isCompressed) {
                            ::glCompressedTexImage3D(glImgTarget,
                                                    mipIndex,
                                                    glTexImageInternalFormat,
                                                    mipWidth, mipHeight, mipDepth,
                                                    0,
                                                    mipDataSize, mipDataPtr);
                            ORYOL_GL_CHECK_ERROR();
                        }
                        else {
                            const GLenum glTexImageType = glTypes::asGLTexImageType(setup.ColorFormat);
                            ::glTexImage3D(glImgTarget,
                                        mipIndex,
                                        glTexImageInternalFormat,
                                        mipWidth, mipHeight, mipDepth,
                                        0,
                                        glTexImageFormat,
                                        glTexImageType,
                                        mipDataPtr);
                            ORYOL_GL_CHECK_ERROR();
                        }
                    }
                    #endif
                }
            }
        }
    }

    // additional render target stuff
    if (setup.IsRenderTarget) {
        // create MSAA renderbuffer
        #if !ORYOL_OPENGLES2
        const bool msaa = (setup.SampleCount > 1) && glCaps::HasFeature(glCaps::MSAARenderTargets);
        if (msaa) {
            ::glGenRenderbuffers(1, &tex.glMSAARenderbuffer);
            o_assert_dbg(0 != tex.glMSAARenderbuffer);
            ::glBindRenderbuffer(GL_RENDERBUFFER, tex.glMSAARenderbuffer);
            ::glRenderbufferStorageMultisample(GL_RENDERBUFFER, setup.SampleCount, glTexImageInternalFormat, setup.Width, setup.Height);
        }
        #endif
        ORYOL_GL_CHECK_ERROR();

        // create depth buffer
        if (setup.HasDepth()) {
            o_assert_dbg(PixelFormat::IsValidTextureDepthFormat(setup.DepthFormat));

            // FIXME: optionally create a depth texture instead of a render buffer here...
            o_assert_dbg(PixelFormat::InvalidPixelFormat != setup.DepthFormat);
            
            ::glGenRenderbuffers(1, &tex.glDepthRenderbuffer);
            o_assert_dbg(0 != tex.glDepthRenderbuffer);
            ::glBindRenderbuffer(GL_RENDERBUFFER, tex.glDepthRenderbuffer);
            GLint glDepthFormat = glTypes::asGLDepthAttachmentFormat(setup.DepthFormat);
            #if !ORYOL_OPENGLES2
            if (msaa) {
                ::glRenderbufferStorageMultisample(GL_RENDERBUFFER, setup.SampleCount, glDepthFormat, setup.Width, setup.Height);
            }
            else
            #endif
            {
                ::glRenderbufferStorage(GL_RENDERBUFFER, glDepthFormat, setup.Width, setup.Height);
            }
            ORYOL_GL_CHECK_ERROR();
        }
    }

    // setup texture object
    this->setupTextureAttrs(tex);
    tex.glTarget = glTextureTarget;

    this->pointers.renderer->invalidateTextureState();
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
