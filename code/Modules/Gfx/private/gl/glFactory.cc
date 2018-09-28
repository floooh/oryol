//------------------------------------------------------------------------------
//  glFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glFactory.h"
#include "gl_impl.h"
#include "glTypes.h"
#include "glCaps.h"
#include "Gfx/private/renderer.h"
#include "Gfx/private/resource.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ResourceState::Code
glFactory::initMesh(mesh& msh, const void* data, int size) {
    o_assert_dbg(this->isValid);
    if (msh.Setup.ShouldSetupFullScreenQuad()) {
        return this->initFullscreenQuad(msh);
    }
    else {
        return this->initStdMesh(msh, data, size);
    }
}

//------------------------------------------------------------------------------
void
glFactory::destroyMesh(mesh& mesh) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateMeshState();
    for (auto& buf : mesh.buffers) {
        for (int i = 0; i < buf.numSlots; i++) {
            GLuint glBuf = buf.glBuffers[i];
            if  (0 != glBuf) {
                ::glDeleteBuffers(1, &glBuf);
            }
        }
    }
    mesh.Clear();
}

//------------------------------------------------------------------------------
GLuint
glFactory::createBuffer(GLenum type, const void* data, uint32_t bufSize, Usage::Code usage) {
    o_assert_dbg((type == GL_ARRAY_BUFFER) || (type == GL_ELEMENT_ARRAY_BUFFER));
    o_assert_dbg(bufSize > 0);
    this->pointers.renderer->invalidateMeshState();
    GLuint buf = 0;
    ::glGenBuffers(1, &buf);
    ORYOL_GL_CHECK_ERROR();
    o_assert_dbg(0 != buf);
    ::glBindBuffer(type, buf);
    ::glBufferData(type, bufSize, data, glTypes::asGLBufferUsage(usage));
    ORYOL_GL_CHECK_ERROR();
    this->pointers.renderer->invalidateMeshState();
    return buf;
}

//------------------------------------------------------------------------------
ResourceState::Code
glFactory::initStdMesh(mesh& msh, const void* data, int size) {
    o_assert_dbg(0 == msh.buffers[mesh::vb].glBuffers[0]);
    o_assert_dbg(0 == msh.buffers[mesh::ib].glBuffers[0]);
    o_assert_dbg(1 == msh.buffers[mesh::vb].numSlots);
    o_assert_dbg(1 == msh.buffers[mesh::ib].numSlots);

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = msh.Setup.NumVertices;
    vbAttrs.Layout = msh.Setup.Layout;
    vbAttrs.BufferUsage = msh.Setup.VertexUsage;
    msh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = msh.Setup.NumIndices;
    ibAttrs.Type = msh.Setup.IndicesType;
    ibAttrs.BufferUsage = msh.Setup.IndexUsage;
    msh.indexBufferAttrs = ibAttrs;

    msh.numPrimGroups = msh.Setup.NumPrimitiveGroups();
    o_assert_dbg(msh.numPrimGroups < GfxConfig::MaxNumPrimGroups);
    for (int i = 0; i < msh.numPrimGroups; i++) {
        msh.primGroups[i] = msh.Setup.PrimitiveGroup(i);
    }

    const uint8_t* ptr = (const uint8_t*)data;

    // create vertex buffer(s)
    if (msh.Setup.NumVertices > 0) {
        const int vbSize = vbAttrs.NumVertices * vbAttrs.Layout.ByteSize();
        msh.buffers[mesh::vb].numSlots = Usage::Stream == vbAttrs.BufferUsage ? 2 : 1;
        const uint8_t* vertices = nullptr;
        if (ptr) {
            o_assert_dbg(msh.Setup.VertexDataOffset >= 0);
            vertices = ptr + msh.Setup.VertexDataOffset;
            o_assert_dbg((ptr + size) >= (vertices + vbSize));
        }
        for (uint8_t slotIndex = 0; slotIndex < msh.buffers[mesh::vb].numSlots; slotIndex++) {
            msh.buffers[mesh::vb].glBuffers[slotIndex] = this->createBuffer(GL_ARRAY_BUFFER, vertices, vbSize, vbAttrs.BufferUsage);
            o_assert_dbg(0 != msh.buffers[mesh::vb].glBuffers[slotIndex]);
        }
    }

    // create optional index buffer(s)
    if (ibAttrs.Type != IndexType::None) {
        const int ibSize = ibAttrs.NumIndices * IndexType::ByteSize(ibAttrs.Type);
        msh.buffers[mesh::ib].numSlots = Usage::Stream == ibAttrs.BufferUsage ? 2 : 1;
        const uint8_t* indices = nullptr;
        if (ptr) {
            o_assert_dbg(msh.Setup.IndexDataOffset >= 0);
            indices = ptr + msh.Setup.IndexDataOffset;
            o_assert_dbg((ptr + size) >= (indices + ibSize));
        }
        for (uint8_t slotIndex = 0; slotIndex < msh.buffers[mesh::ib].numSlots; slotIndex++) {
            msh.buffers[mesh::ib].glBuffers[slotIndex] = this->createBuffer(GL_ELEMENT_ARRAY_BUFFER, indices, ibSize, ibAttrs.BufferUsage);
            o_assert_dbg(0 != msh.buffers[mesh::ib].glBuffers[slotIndex]);
        }
    }
    
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glFactory::initFullscreenQuad(mesh& mesh) {
    o_assert_dbg(mesh.Setup.Layout.NumComponents() == 2);
    o_assert_dbg(mesh.Setup.Layout.ComponentAt(0).Attr == VertexAttr::Position);
    o_assert_dbg(mesh.Setup.Layout.ComponentAt(1).Attr == VertexAttr::TexCoord0);
    o_assert_dbg(mesh.Setup.Layout.ComponentAt(0).Format == VertexFormat::Float3);
    o_assert_dbg(mesh.Setup.Layout.ComponentAt(1).Format == VertexFormat::Float2);

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = 4;
    vbAttrs.BufferUsage = Usage::Immutable;
    vbAttrs.Layout = mesh.Setup.Layout;
    mesh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = 6;
    ibAttrs.Type = IndexType::Index16;
    ibAttrs.BufferUsage = Usage::Immutable;
    mesh.indexBufferAttrs = ibAttrs;

    mesh.numPrimGroups = 1;
    mesh.primGroups[0] = PrimitiveGroup(0, 6);

    // vertices
    const float topV = mesh.Setup.FullScreenQuadFlipV ? 0.0f : 1.0f;
    const float botV = mesh.Setup.FullScreenQuadFlipV ? 1.0f : 0.0f;
    float vertices[] = {
        -1.0f, +1.0f, 0.0f, 0.0f, topV,     // top-left corner
        +1.0f, +1.0f, 0.0f, 1.0f, topV,     // top-right corner
        +1.0f, -1.0f, 0.0f, 1.0f, botV,     // bottom-right corner
        -1.0f, -1.0f, 0.0f, 0.0f, botV      // bottom-left corner
    };

    // indices
    uint16_t indices[] = {
        0, 2, 1,            // topleft -> bottomright -> topright
        0, 3, 2,            // topleft -> bottomleft -> bottomright
    };
    
    // create vertex and index buffer
    o_assert_dbg(1 == mesh.buffers[mesh::vb].numSlots);
    o_assert_dbg(1 == mesh.buffers[mesh::ib].numSlots);
    mesh.buffers[mesh::vb].glBuffers[0] = this->createBuffer(GL_ARRAY_BUFFER, vertices, sizeof(vertices), mesh.vertexBufferAttrs.BufferUsage);
    mesh.buffers[mesh::ib].glBuffers[0] = this->createBuffer(GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices), mesh.indexBufferAttrs.BufferUsage);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glFactory::initTexture(texture& tex, const void* data, int size) {
    o_assert_dbg(this->isValid);
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
    const GLenum glTexTarget = glTypes::asGLTextureTarget(setup.Type);
    if (tex.Setup.ShouldSetupFromNativeTexture()) {
        // existing native GL texture object provided
        tex.nativeHandles = true;
        tex.glTextures[0] = (GLuint) tex.Setup.NativeHandle[0];
        tex.glTextures[1] = (GLuint) tex.Setup.NativeHandle[1];
    }
    else {
        tex.nativeHandles = false;

        // create GL texture objects
        const GLenum glTexImageFormat = glTypes::asGLTexImageFormat(setup.ColorFormat);
        const bool isCompressed = PixelFormat::IsCompressedFormat(setup.ColorFormat);
        const uint8_t* srcPtr = (const uint8_t*) data;
        for (int slotIndex = 0; slotIndex < tex.numSlots; slotIndex++) {

            this->pointers.renderer->invalidateTextureState();
            ::glGenTextures(1, &tex.glTextures[slotIndex]);
            ::glActiveTexture(GL_TEXTURE0);
            ::glBindTexture(glTexTarget, tex.glTextures[slotIndex]);
            ORYOL_GL_CHECK_ERROR();

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

            const int numFaces = setup.Type == TextureType::TextureCube ? 6 : 1;
            for (int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
                GLenum glImgTarget;
                if (TextureType::TextureCube == setup.Type) {
                    glImgTarget = glTypes::asGLCubeFaceTarget(faceIndex);
                }
                else {
                    glImgTarget = glTexTarget;
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
            ::glGenRenderbuffers(1, &tex.glDepthRenderbuffer);
            o_assert_dbg(0 != tex.glDepthRenderbuffer);
            ::glBindRenderbuffer(GL_RENDERBUFFER, tex.glDepthRenderbuffer);
            GLenum glDepthFormat = glTypes::asGLDepthAttachmentFormat(setup.DepthFormat);
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
    tex.glTarget = glTexTarget;

    this->pointers.renderer->invalidateTextureState();
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glFactory::destroyTexture(texture& tex) {
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
                                                GL_COLOR_ATTACHMENT0+i,
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
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: reason = "GL_FRAMEBUFFER_IMCOMPLETE_DIMENSIONS"; break;
            #endif
            default: reason = "unknown";
        }
        o_warn("glRenderPassFactory: framebuffer completeness check failed with '%s'\n", reason);
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
glFactory::initRenderPass(renderPass& rp) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(0 == rp.glFramebuffer);

    gfxFactoryBase::initRenderPass(rp);
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
glFactory::destroyRenderPass(renderPass& rp) {
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
    gfxFactoryBase::destroyRenderPass(rp);
}

//------------------------------------------------------------------------------
static GLuint 
compileShader(ShaderStage::Code stage, const char* sourceString, int sourceLen) {
    o_assert_dbg(sourceString && (sourceLen > 0));
    
    GLuint glShader = ::glCreateShader(glTypes::asGLShaderStage(stage));
    o_assert_dbg(0 != glShader);
    ORYOL_GL_CHECK_ERROR();
    
    // attach source to shader object
    ::glShaderSource(glShader, 1, &sourceString, &sourceLen);
    ORYOL_GL_CHECK_ERROR();
    
    // compile the shader
    ::glCompileShader(glShader);
    ORYOL_GL_CHECK_ERROR();
    
    // compilation failed?
    GLint compileStatus = 0;
    ::glGetShaderiv(glShader, GL_COMPILE_STATUS, &compileStatus);
    ORYOL_GL_CHECK_ERROR();
    
    #if ORYOL_DEBUG
    GLint logLength = 0;
    ::glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &logLength);
    ORYOL_GL_CHECK_ERROR();
    if (logLength > 0) {
        
        // first print the shader source
        Log::Info("SHADER SOURCE:\n%s\n\n", sourceString);
        
        // now print the info log
        GLchar* shdLogBuf = (GLchar*) Memory::Alloc(logLength);
        ::glGetShaderInfoLog(glShader, logLength, &logLength, shdLogBuf);
        ORYOL_GL_CHECK_ERROR();
        Log::Info("SHADER LOG: %s\n\n", shdLogBuf);
        Memory::Free(shdLogBuf);
    }
    #endif
    
    if (!compileStatus) {
        // compiling failed
        ::glDeleteShader(glShader);
        ORYOL_GL_CHECK_ERROR();
        glShader = 0;
    }
    return glShader;
}

//------------------------------------------------------------------------------
ResourceState::Code
glFactory::initShader(shader& shd) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateShaderState();

    #if ORYOL_OPENGLES2
    const ShaderLang::Code slang = ShaderLang::GLSL100;
    #elif ORYOL_OPENGLES3
    const ShaderLang::Code slang = glCaps::IsFlavour(glCaps::GLES3) ? ShaderLang::GLSLES3 : ShaderLang::GLSL100;
    #elif ORYOL_OPENGL_CORE_PROFILE
    const ShaderLang::Code slang = ShaderLang::GLSL330;
    #else
    #error "Unsupported GL profile"
    #endif
    const ShaderSetup& setup = shd.Setup;

    o_assert_dbg(setup.VertexShaderSource(slang).IsValid());
    o_assert_dbg(setup.FragmentShaderSource(slang).IsValid());

    // compile vertex shader
    const String& vsSource = setup.VertexShaderSource(slang);
    GLuint glVertexShader = compileShader(ShaderStage::VS, vsSource.AsCStr(), vsSource.Length());
    o_assert_dbg(0 != glVertexShader);
        
    // compile fragment shader
    const String& fsSource = setup.FragmentShaderSource(slang);
    GLuint glFragmentShader = compileShader(ShaderStage::FS, fsSource.AsCStr(), fsSource.Length());
    o_assert_dbg(0 != glFragmentShader);
        
    // create GL program object and attach vertex/fragment shader
    GLuint glProg = ::glCreateProgram();
    ::glAttachShader(glProg, glVertexShader);
    ORYOL_GL_CHECK_ERROR();
    ::glAttachShader(glProg, glFragmentShader);
    ORYOL_GL_CHECK_ERROR();
        
    // bind vertex attribute locations
    #if !ORYOL_GL_USE_GETATTRIBLOCATION
    o_assert_dbg(VertexAttr::NumVertexAttrs <= glCaps::IntLimit(glCaps::MaxVertexAttribs));
    const VertexLayout& vsInputLayout = setup.InputLayout();
    for (int i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        VertexAttr::Code attr = (VertexAttr::Code)i;
        if (vsInputLayout.Contains(attr)) {
            ::glBindAttribLocation(glProg, i, VertexAttr::ToString(attr));
        }
    }
    ORYOL_GL_CHECK_ERROR();
    #endif

    // link the program
    ::glLinkProgram(glProg);
    ORYOL_GL_CHECK_ERROR();
        
    // can discard shaders now if we compiled them ourselves
    ::glDeleteShader(glVertexShader);
    ::glDeleteShader(glFragmentShader);

    // linking successful?
    GLint linkStatus;
    ::glGetProgramiv(glProg, GL_LINK_STATUS, &linkStatus);
    #if ORYOL_DEBUG
    GLint logLength;
    ::glGetProgramiv(glProg, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar* logBuffer = (GLchar*) Memory::Alloc(logLength);
        ::glGetProgramInfoLog(glProg, logLength, &logLength, logBuffer);
        Log::Info("%s\n", logBuffer);
        Memory::Free(logBuffer);
    }
    #endif
    ORYOL_GL_CHECK_ERROR();
    if (!linkStatus) {
        o_warn("Failed to link program '%s'\n", setup.Locator.Location().AsCStr());
        return ResourceState::Failed;
    }
        
    // linking succeeded, store GL program
    shd.glProgram = glProg;
    this->pointers.renderer->useProgram(glProg);

    // lookup uniform locations (one vec4[] per uniform block)
    const int numUniformBlocks = setup.NumUniformBlocks();
    for (int ubIndex = 0; ubIndex < numUniformBlocks; ubIndex++) {
        ShaderStage::Code ubBindStage = setup.UniformBlockBindStage(ubIndex);
        int ubBindSlot = setup.UniformBlockBindSlot(ubIndex);
        const StringAtom& ubName = setup.UniformBlockType(ubIndex);
        GLint glUniformLocation = ::glGetUniformLocation(glProg, ubName.AsCStr());
        if (-1 != glUniformLocation) {
            shd.bindUniformBlock(ubBindStage, ubBindSlot, glUniformLocation);
        }
        else {
            Log::Warn("Uniform '%s' not found on shader, will be ignored!\n", ubName.AsCStr());
        }
    }

    // resolve texture locations
    int glTextureLocation = 0;
    const int numTextures = setup.NumTextures();
    for (int texIndex = 0; texIndex < numTextures; texIndex++) {
        ShaderStage::Code bindStage = setup.TexBindStage(texIndex);
        int bindSlot = setup.TexBindSlot(texIndex);
        const StringAtom& name = setup.TexName(texIndex);
        const GLint glUniformLocation = ::glGetUniformLocation(glProg, name.AsCStr()); 
        if (-1 != glUniformLocation) {
            shd.bindSampler(bindStage, bindSlot, glTextureLocation);
            // set the sampler index in the shader program, this will never change
            ::glUniform1i(glUniformLocation, glTextureLocation);
            glTextureLocation++;
        }
        else {
            Log::Warn("Sampler uniform '%s' not found on shader, will be ignored!\n", name.AsCStr());
        }
    }
    ORYOL_GL_CHECK_ERROR();

    #if ORYOL_GL_USE_GETATTRIBLOCATION
    // resolve attrib locations
    for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        GLint loc = ::glGetAttribLocation(glProg, VertexAttr::ToString((VertexAttr::Code)i));
        shd.bindAttribLocation((VertexAttr::Code)i, loc);
    }
    #endif

    this->pointers.renderer->invalidateShaderState();
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glFactory::destroyShader(shader& shd) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateShaderState();
    if (0 != shd.glProgram) {
        ::glDeleteProgram(shd.glProgram);
        ORYOL_GL_CHECK_ERROR();
    }
    shd.Clear();
}

//------------------------------------------------------------------------------
// map Oryol VertexFormats to GL vertex attributes
struct {
    GLint size;
    GLenum type;
    GLboolean normalized;
} const vertexFormatTable[VertexFormat::NumVertexFormats] = {
    { 1, GL_FLOAT, GL_FALSE },                  // VertexFormat::Float
    { 2, GL_FLOAT, GL_FALSE },                  // VertexFormat::Float2
    { 3, GL_FLOAT, GL_FALSE },                  // VertexFormat::Float3
    { 4, GL_FLOAT, GL_FALSE },                  // VertexFormat::Float4
    { 4, GL_BYTE, GL_FALSE },                   // VertexFormat::Byte4
    { 4, GL_BYTE, GL_TRUE },                    // VertexFormat::Byte4N
    { 4, GL_UNSIGNED_BYTE, GL_FALSE },          // VertexFormat::UByte4
    { 4, GL_UNSIGNED_BYTE, GL_TRUE },           // VertexFormat::UByte4N
    { 2, GL_SHORT, GL_FALSE },                  // VertexFormat::Short2
    { 2, GL_SHORT, GL_TRUE },                   // VertexFormat::Short2N
    { 4, GL_SHORT, GL_FALSE },                  // VertexFormat::Short4
    { 4, GL_SHORT, GL_TRUE },                   // VertexFormat::Short4N
    { 4, GL_UNSIGNED_INT_2_10_10_10_REV, GL_TRUE },     // VertexFormat::UInt10_2N
};

//------------------------------------------------------------------------------
static void 
initVertexAttrs(pipeline& pip) {
    // first disable all vertex attrs
    for (int attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
        pip.glAttrs[attrIndex] = pipeline::vertexAttr();
        pip.glAttrs[attrIndex].index = attrIndex;
    }

    // convert input mesh vertex layout components to GL vertex attr
    const VertexLayout& vsInputLayout = pip.shd->Setup.InputLayout();
    for (int layoutIndex = 0; layoutIndex < GfxConfig::MaxNumInputMeshes; layoutIndex++) {
        const VertexLayout& layout = pip.Setup.Layouts[layoutIndex];
        const int numComps = layout.NumComponents();
        if (numComps > 0) {
            for (int compIndex = 0; compIndex < numComps; compIndex++) {
                const VertexLayout::Component& comp = layout.ComponentAt(compIndex);
                if (vsInputLayout.Contains(comp.Attr)) {
                    o_assert_dbg(comp.Format < VertexFormat::NumVertexFormats);
                    auto& glAttr = pip.glAttrs[comp.Attr];
                    o_assert_dbg(!glAttr.enabled);
                    glAttr.enabled = GL_TRUE;
                    glAttr.vbIndex = layoutIndex;
                    if (VertexStepFunction::PerVertex == layout.StepFunction) {
                        glAttr.divisor = 0;
                    }
                    else {
                        glAttr.divisor = layout.StepRate;
                    }
                    glAttr.stride = layout.ByteSize();
                    glAttr.offset = layout.ComponentByteOffset(compIndex);
                    glAttr.size   = vertexFormatTable[comp.Format].size;
                    glAttr.type   = vertexFormatTable[comp.Format].type;
                    glAttr.normalized = vertexFormatTable[comp.Format].normalized;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
glFactory::initPipeline(pipeline& pip) {
    o_assert_dbg(this->isValid);

    gfxFactoryBase::initPipeline(pip);
    o_assert_dbg(pip.shd);
    initVertexAttrs(pip);
    pip.glPrimType = glTypes::asGLPrimitiveType(pip.Setup.PrimType);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glFactory::destroyPipeline(pipeline& pip) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateMeshState();
    pip.Clear();
}

} // namespace _priv
} // namespace Oryol
