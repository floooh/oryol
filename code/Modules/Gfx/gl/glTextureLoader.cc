//------------------------------------------------------------------------------
//  glTextureLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTextureLoader.h"
#include "Core/String/StringBuilder.h"
#include "Gfx/Core/texture.h"
#include "Gfx/gl/gl_impl.h"
#define GLIML_ASSERT(x) o_assert(x)
#include "gliml/gliml.h"
#include "IO/IO.h"
#include "Gfx/gl/glTypes.h"
#include "Gfx/gl/glExt.h"
#include "Gfx/Core/textureFactory.h"

namespace Oryol {
namespace _priv {
    
OryolClassImpl(glTextureLoader);

//------------------------------------------------------------------------------
bool
glTextureLoader::Accepts(const texture& tex) const {
    o_assert(tex.GetState() == ResourceState::Setup);

    // test the file extension, we will accept and load anything, whether
    // the compressed texture can actually be loaded is another question
    const char* loc = tex.GetSetup().Locator.Location().AsCStr();
    if ((InvalidIndex != StringBuilder::FindSubString(loc, 0, EndOfString, ".dds")) ||
        (InvalidIndex != StringBuilder::FindSubString(loc, 0, EndOfString, ".pvr")) ||
        (InvalidIndex != StringBuilder::FindSubString(loc, 0, EndOfString, ".ktx"))) {
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
glTextureLoader::Accepts(const texture& tex, const Ptr<Stream>& data) const {
    o_assert(tex.GetState() == ResourceState::Setup);
    bool accepted = false;
    
    // need to look at magic number
    if (data->Open(OpenMode::ReadOnly)) {
        const void* dataPtr = data->MapRead(nullptr);
        const int32 dataSize = data->Size();
        if (gliml::is_dds(dataPtr, dataSize)) {
            accepted = true;
        }
        else if (gliml::is_pvr(dataPtr, dataSize)) {
            accepted = true;
        }
        else if (gliml::is_ktx(dataPtr, dataSize)) {
            accepted = true;
        }
        data->UnmapRead();
        data->Close();
    }
    return accepted;
}

//------------------------------------------------------------------------------
void
glTextureLoader::Load(texture& tex) const {
    const TextureSetup& setup = tex.GetSetup();
    const ResourceState::Code state = tex.GetState();
    o_assert((state == ResourceState::Setup) || (state == ResourceState::Pending));
    
    if (state == ResourceState::Setup) {
        // start loading the resource
        tex.setIORequest(IO::LoadFile(setup.Locator.Location(), setup.IOLane));
        tex.setState(ResourceState::Pending);
        return;
    }
    else if (state == ResourceState::Pending) {
        // check if async IO has finished
        if (tex.GetIORequest()->Handled()) {
            if (tex.GetIORequest()->GetStatus() == IOStatus::OK) {
                // forward to load-with-data method
                this->Load(tex, tex.GetIORequest()->GetStream());
                o_assert((tex.GetState() == ResourceState::Valid) || (tex.GetState() == ResourceState::Failed));
            }
            else {
                // hmm loading the image data failed
                tex.setState(ResourceState::Failed);
            }
            // clear the IO request object
            tex.setIORequest(nullptr);
        }
    }
}

//------------------------------------------------------------------------------
void
glTextureLoader::Load(texture& tex, const Ptr<Stream>& data) const {
    o_assert((tex.GetState() == ResourceState::Setup) || (tex.GetState() == ResourceState::Pending));
    
    // open stream and get pointer to contained data
    if (data->Open(OpenMode::ReadOnly)) {
        const void* dataPtr = data->MapRead(nullptr);
        const int32 dataSize = data->Size();
        gliml::context ctx;
        ctx.enable_dxt(glExt::HasExtension(glExt::TextureCompressionDXT));
        ctx.enable_pvrtc(glExt::HasExtension(glExt::TextureCompressionPVRTC));
        #if ORYOL_OPENGLES3
        ctx.enable_etc2(true);
        #endif
        if (ctx.load(dataPtr, dataSize)) {
            if (this->glCreateTexture(tex, ctx)) {
                tex.setState(ResourceState::Valid);
            }
            else {
                tex.setState(ResourceState::Failed);
            }
        }
        else {
            Log::Warn("glTextureLoader: failed to load texture '%s'!\n", tex.GetSetup().Locator.Location().AsCStr());
            tex.setState(ResourceState::Failed);
        }
        data->UnmapRead();
        data->Close();
    }
    else {
        o_assert2(false, "can't happen!\n");
    }
}

//------------------------------------------------------------------------------
bool
glTextureLoader::glCreateTexture(texture& tex, const gliml::context& ctx) const {
    o_assert(this->texFactory);
    const TextureSetup& setup = tex.GetSetup();
    const GLenum glTexTarget = ctx.texture_target();
    ORYOL_GL_CHECK_ERROR();
    
    // create a texture object
    GLuint glTex = this->texFactory->glGenAndBindTexture(glTexTarget);
    
    // setup texture params
    GLenum glMinFilter = setup.MinFilter;
    GLenum glMagFilter = setup.MagFilter;
    if (1 == ctx.num_mipmaps(0)) {
        if ((glMinFilter == GL_NEAREST_MIPMAP_NEAREST) || (glMinFilter == GL_NEAREST_MIPMAP_LINEAR)) {
            glMinFilter = GL_NEAREST;
        }
        else if ((glMinFilter == GL_LINEAR_MIPMAP_NEAREST) || (glMinFilter == GL_LINEAR_MIPMAP_LINEAR)) {
            glMinFilter = GL_LINEAR;
        }
    }
    ::glTexParameteri(glTexTarget, GL_TEXTURE_MIN_FILTER, glMinFilter);
    ::glTexParameteri(glTexTarget, GL_TEXTURE_MAG_FILTER, glMagFilter);
    ORYOL_GL_CHECK_ERROR();
    if (ctx.num_faces() > 1) {
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else {
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_S, setup.WrapU);
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_T, setup.WrapV);
    }
    if (ctx.is_3d()) {
        #if ORYOL_OPENGLES2
        o_error("glTextureLoader: 3D texture not supported on OpenGLES2!\n");
        #else
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_R, setup.WrapW);
        #endif
    }
    ORYOL_GL_CHECK_ERROR();

    // setup the image data in the texture
    for (int32 faceIndex = 0; faceIndex < ctx.num_faces(); faceIndex++) {
        for (int32 mipIndex = 0; mipIndex < ctx.num_mipmaps(faceIndex); mipIndex++) {
            if (ctx.is_compressed()) {
                // a compressed texture
                if (ctx.is_2d()) {
                    // a compressed 2D or CubeMap textue
                    ::glCompressedTexImage2D(ctx.image_target(faceIndex),
                                             mipIndex,
                                             ctx.image_internal_format(),
                                             ctx.image_width(faceIndex, mipIndex),
                                             ctx.image_height(faceIndex, mipIndex),
                                             0,
                                             ctx.image_size(faceIndex, mipIndex),
                                             ctx.image_data(faceIndex, mipIndex));
                    ORYOL_GL_CHECK_ERROR();
                }
                else {
                    // a compressed 3D texture
                    #if ORYOL_OPENGLES2
                    o_error("glTextureLoader: 3D compressed textures not supported in OpenGLES2!\n");
                    #else
                    ::glCompressedTexImage3D(ctx.image_target(faceIndex),
                                             mipIndex,
                                             ctx.image_internal_format(),
                                             ctx.image_width(faceIndex, mipIndex),
                                             ctx.image_height(faceIndex, mipIndex),
                                             ctx.image_depth(faceIndex, mipIndex),
                                             0,
                                             ctx.image_size(faceIndex, mipIndex),
                                             ctx.image_data(faceIndex, mipIndex));
                    #endif
                }
            }
            else {
                // an uncompressed texture
                if (ctx.is_2d()) {
                    // an uncompressed 2D or CubeMap texture
                    ::glTexImage2D(ctx.image_target(faceIndex),
                                   mipIndex,
                                   ctx.image_internal_format(),
                                   ctx.image_width(faceIndex, mipIndex),
                                   ctx.image_height(faceIndex, mipIndex),
                                   0,
                                   ctx.image_format(),
                                   ctx.image_type(),
                                   ctx.image_data(faceIndex, mipIndex));
                    ORYOL_GL_CHECK_ERROR();
                }
                else {
                    // an uncompressed 3D texture
                    #if ORYOL_OPENGLES2
                    o_error("glTextureLoader: 3D uncompressed textures not supported in OpenGLES2!\n");
                    #else
                    ::glTexImage3D(ctx.image_target(faceIndex),
                                   mipIndex,
                                   ctx.image_internal_format(),
                                   ctx.image_width(faceIndex, mipIndex),
                                   ctx.image_height(faceIndex, mipIndex),
                                   ctx.image_depth(faceIndex, mipIndex),
                                   0,
                                   ctx.image_format(),
                                   ctx.image_type(),
                                   ctx.image_data(faceIndex, mipIndex));
                    ORYOL_GL_CHECK_ERROR();
                    #endif
                }
            }
        }
    }
    
    // setup texture attributes
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    if (ctx.is_2d()) {
        if (ctx.num_faces() > 1) {
            attrs.Type = TextureType::TextureCube;
        }
        else {
            attrs.Type = TextureType::Texture2D;
        }
    }
    else if (ctx.is_3d()) {
        attrs.Type = TextureType::Texture3D;
    }
    attrs.ColorFormat = setup.ColorFormat;
    attrs.DepthFormat = setup.DepthFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = ctx.image_width(0, 0);
    attrs.Height = ctx.image_height(0, 0);
    attrs.Depth = ctx.image_depth(0, 0);
    attrs.HasMipmaps = ctx.num_mipmaps(0) > 1;
    
    // setup texture
    tex.setTextureAttrs(attrs);
    tex.glSetTexture(glTex);
    tex.glSetTarget(ctx.texture_target());
    tex.setState(ResourceState::Valid);
    
    return true;
}

} // namespace _priv
} // namespace Texture
