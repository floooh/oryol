//------------------------------------------------------------------------------
//  glTextureLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTextureLoader.h"
#include "Core/String/StringBuilder.h"
#include "Render/Core/texture.h"
#include "Render/gl/gl_impl.h"
#define GLIML_ASSERT(x) o_assert(x)
#include "gliml/gliml.h"
#include "IO/IOFacade.h"
#include "Render/gl/glTypes.h"
#include "Render/Core/textureFactory.h"

namespace Oryol {
namespace Render {
    
OryolClassImpl(glTextureLoader);

using namespace Core;
using namespace IO;

//------------------------------------------------------------------------------
bool
glTextureLoader::Accepts(const texture& tex) const {
    o_assert(tex.GetState() == Resource::State::Setup);

    // test the file extension
    const char* loc = tex.GetSetup().GetLocator().Location().AsCStr();
    if (InvalidIndex != StringBuilder::FindSubString(loc, 0, EndOfString, ".dds")) {
        return true;
    }
    else {
        /// @todo: add more compressed texture formats depending of target platform
        return false;
    }
}

//------------------------------------------------------------------------------
bool
glTextureLoader::Accepts(const texture& tex, const Ptr<Stream>& data) const {
    o_assert(tex.GetState() == Resource::State::Setup);
    bool accepted = false;
    
    // need to look at magic number
    if (data->Open(OpenMode::ReadOnly)) {
        const void* dataPtr = data->MapRead(nullptr);
        if (gliml::is_dds(dataPtr, data->Size())) {
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
    const Resource::State::Code state = tex.GetState();
    o_assert((state == Resource::State::Setup) || (state == Resource::State::Pending));
    
    if (state == Resource::State::Setup) {
        // start loading the resource
        tex.setIORequest(IOFacade::Instance()->LoadFile(setup.GetLocator().Location(), setup.GetIOLane()));
        return;
    }
    else if (state == Resource::State::Pending) {
        // check if async IO has finished
        if (tex.GetIORequest()->Handled()) {
            if (tex.GetIORequest()->GetStatus() == IOStatus::OK) {
                // forward to load-with-data method
                this->Load(tex, tex.GetIORequest()->GetStream());
                o_assert(tex.GetState() == Resource::State::Valid);
            }
            else {
                // hmm loading the image data failed
                tex.setState(Resource::State::Failed);
            }
            // clear the IO request object
            tex.setIORequest(nullptr);
        }
    }
}

//------------------------------------------------------------------------------
void
glTextureLoader::Load(texture& tex, const Ptr<Stream>& data) const {
    o_assert((tex.GetState() == Resource::State::Setup) || (tex.GetState() == Resource::State::Pending));
    
    // open stream and get pointer to contained data
    if (data->Open(OpenMode::ReadOnly)) {
        const void* dataPtr = data->MapRead(nullptr);
        const int32 dataSize = data->Size();
        gliml::context ctx;
        if (gliml::is_dds(dataPtr, dataSize)) {
            if (ctx.load_dds(dataPtr, dataSize)) {
                this->glCreateTexture(tex, ctx);
                tex.setState(Resource::State::Valid);
            }
            else {
                Log::Warn("glTextureLoader: failed to load DDS texture '%s'!\n", tex.GetSetup().GetLocator().Location().AsCStr());
                tex.setState(Resource::State::Failed);
            }
        }
        else {
            Log::Warn("glTextureLoader: unknown texture file format for '%s'!\n", tex.GetSetup().GetLocator().Location().AsCStr());
            tex.setState(Resource::State::Failed);
        }
        data->UnmapRead();
        data->Close();
    }
    else {
        o_assert2(false, "can't happen!\n");
    }
}

//------------------------------------------------------------------------------
void
glTextureLoader::glCreateTexture(texture& tex, const gliml::context& ctx) const {
    o_assert(this->texFactory);
    const TextureSetup& setup = tex.GetSetup();
    const GLenum glTexTarget = ctx.texture_target();
    
    // create a texture object
    GLuint glTex = this->texFactory->glGenAndBindTexture(glTexTarget);
    
    // setup texture params
    GLenum glMinFilter = glTypes::AsGLTextureFilterMode(setup.GetMinFilter());
    GLenum glMagFilter = glTypes::AsGLTextureFilterMode(setup.GetMagFilter());
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
    ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_S, glTypes::AsGLTextureWrapMode(setup.GetWrapU()));
    ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_T, glTypes::AsGLTextureWrapMode(setup.GetWrapV()));
    if (ctx.is_3d()) {
        #if ORYOL_OPENGLES2
        o_error("glTextureLoader: 3D texture not supported on OpenGLES2!\n");
        #else
        ::glTexParameteri(glTexTarget, GL_TEXTURE_WRAP_R, glTypes::AsGLTextureWrapMode(setup.GetWrapW()));
        #endif
    }
    ORYOL_GL_CHECK_ERROR();
    
    // setup the image data in the texture
    // FIXME: check for compressed texture extensions
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
    attrs.setLocator(setup.GetLocator());
    if (ctx.is_2d()) {
        if (ctx.num_faces() > 1) {
            attrs.setType(TextureType::TextureCube);
        }
        else {
            attrs.setType(TextureType::Texture2D);
        }
    }
    else if (ctx.is_3d()) {
        attrs.setType(TextureType::Texture3D);
    }
    attrs.setColorFormat(setup.GetColorFormat());
    attrs.setDepthFormat(setup.GetDepthFormat());
    attrs.setUsage(Usage::Immutable);
    attrs.setWidth(ctx.image_width(0, 0));
    attrs.setHeight(ctx.image_height(0, 0));
    attrs.setDepth(ctx.image_depth(0, 0));
    attrs.setMipmapsFlag(ctx.num_mipmaps(0));
    
    // setup texture
    tex.setTextureAttrs(attrs);
    tex.glSetTexture(glTex);
    tex.glSetTarget(ctx.texture_target());
    tex.setState(Resource::State::Valid);
}

} // namespace Render
} // namespace Texture
