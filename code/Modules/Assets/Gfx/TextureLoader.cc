//------------------------------------------------------------------------------
//  TextureLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureLoader.h"
#include "IO/IO.h"
#include "Gfx/Gfx.h"
#include "IO/Stream/Stream.h"
#define GLIML_ASSERT(x) o_assert(x)
#include "gliml.h"

namespace Oryol {

OryolClassImpl(TextureLoader);

//------------------------------------------------------------------------------
TextureLoader::TextureLoader(const TextureSetup& setup_, int32 ioLane_) :
TextureLoaderBase(setup_, ioLane_) {
    // empty
}

//------------------------------------------------------------------------------
Id
TextureLoader::Start() {
    
    // prepare the Gfx resource
    this->resId = Gfx::Resource().prepareAsync(this->setup);
    
    // fire IO request to start loading the texture data
    this->ioRequest = IOProtocol::Request::Create();
    this->ioRequest->SetURL(setup.Locator.Location());
    this->ioRequest->SetLane(this->ioLane);
    IO::Put(this->ioRequest);
    
    return resId;
}

//------------------------------------------------------------------------------
ResourceState::Code
TextureLoader::Continue() {
    o_assert_dbg(this->resId.IsValid());
    o_assert_dbg(this->ioRequest.isValid());
    
    ResourceState::Code result = ResourceState::Pending;
    
    if (this->ioRequest->Handled()) {
        if (this->ioRequest->GetStatus() == IOStatus::OK) {
            // yeah, IO is done, let gliml parse the texture data
            // and create the texture resource
            const Ptr<Stream>& stream = this->ioRequest->GetStream();
            stream->Open(OpenMode::ReadOnly);
            const uint8* data = stream->MapRead(nullptr);
            const int32 numBytes = stream->Size();
            
            gliml::context ctx;
            ctx.enable_dxt(true);
            ctx.enable_pvrtc(true);
            ctx.enable_etc2(true);
            if (ctx.load(data, numBytes)) {
                TextureSetup texSetup = this->buildSetup(this->setup, &ctx, data);
                SetupAndStream<TextureSetup> setupAndStream(texSetup, stream);
                result = Gfx::Resource().initAsync(this->resId, setupAndStream);
            }
            else {
                result = Gfx::Resource().failedAsync(this->resId);
            }
        }
        else {
            // IO had failed
            result = Gfx::Resource().failedAsync(this->resId);
        }
        this->ioRequest = nullptr;
    }
    return result;
}

//------------------------------------------------------------------------------
TextureSetup
TextureLoader::buildSetup(const TextureSetup& blueprint, const gliml::context* ctx, const uint8* data) {
    const int32 w = ctx->image_width(0, 0);
    const int32 h = ctx->image_height(0, 0);
    const int32 numFaces = ctx->num_faces();
    const int32 numMips = ctx->num_mipmaps(0);
    PixelFormat::Code pixelFormat = PixelFormat::InvalidPixelFormat;
    switch(ctx->image_internal_format()) {
        case GLIML_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            pixelFormat = PixelFormat::DXT1;
            break;
        case GLIML_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            pixelFormat = PixelFormat::DXT3;
            break;
        case GLIML_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            pixelFormat = PixelFormat::DXT5;
            break;
        case GLIML_GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
            pixelFormat = PixelFormat::PVRTC4_RGB;
            break;
        case GLIML_GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
            pixelFormat = PixelFormat::PVRTC2_RGB;
            break;
        case GLIML_GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            pixelFormat = PixelFormat::PVRTC4_RGBA;
            break;
        case GLIML_GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
            pixelFormat = PixelFormat::PVRTC2_RGBA;
            break;
        case GLIML_GL_COMPRESSED_RGB8_ETC2:
            pixelFormat = PixelFormat::ETC2_RGB8;
            break;
        case GLIML_GL_COMPRESSED_SRGB8_ETC2:
            pixelFormat = PixelFormat::ETC2_SRGB8;
            break;
        case GLIML_GL_RGB:
            if (ctx->image_type() == GLIML_GL_UNSIGNED_BYTE) {
                pixelFormat = PixelFormat::RGB8;
            }
            else if (ctx->image_type() == GLIML_GL_UNSIGNED_SHORT_5_6_5) {
                pixelFormat = PixelFormat::R5G6B5;
            }
            break;
            
        case GLIML_GL_RGBA:
            switch (ctx->image_type()) {
                case GLIML_GL_UNSIGNED_BYTE:
                    pixelFormat = PixelFormat::RGBA8;
                    break;
                case GLIML_GL_UNSIGNED_SHORT_4_4_4_4:
                    pixelFormat = PixelFormat::RGBA4;
                    break;
                case GLIML_GL_UNSIGNED_SHORT_5_5_5_1:
                    pixelFormat = PixelFormat::R5G5B5A1;
                    break;
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
    o_assert(PixelFormat::InvalidPixelFormat != pixelFormat);
    TextureType::Code type = TextureType::InvalidTextureType;
    switch (ctx->texture_target()) {
        case GLIML_GL_TEXTURE_2D:
            type = TextureType::Texture2D;
            break;
        case GLIML_GL_TEXTURE_3D:
            type = TextureType::Texture3D;
            break;
        case GLIML_GL_TEXTURE_CUBE_MAP:
            type = TextureType::TextureCube;
            break;
        default:
            o_error("Unknown texture type!\n");
            break;
    }
    TextureSetup newSetup = TextureSetup::FromPixelData(w, h, numMips, type, pixelFormat, this->setup);
    
    // setup mipmap offsets
    o_assert_dbg(TextureSetup::MaxNumMipMaps >= ctx->num_mipmaps(0));
    for (int32 faceIndex = 0; faceIndex < numFaces; faceIndex++) {
        for (int32 mipIndex = 0; mipIndex < numMips; mipIndex++) {
            const uint8* cur = (const uint8*) ctx->image_data(faceIndex, mipIndex);
            newSetup.ImageOffsets[faceIndex][mipIndex] = int32(cur - data);
            newSetup.ImageSizes[faceIndex][mipIndex] = ctx->image_size(faceIndex, mipIndex);
        }
    }
    return newSetup;
}

} // namespace Oryol