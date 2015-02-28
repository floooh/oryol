//------------------------------------------------------------------------------
//  TextureLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureLoader.h"
#include "Gfx/Gfx.h"
#include "IO/Stream/Stream.h"
#define GLIML_ASSERT(x) o_assert(x)
#include "gliml.h"

namespace Oryol {

OryolClassImpl(TextureLoader);

//------------------------------------------------------------------------------
void
TextureLoader::Loaded(const URL& url, int32 ioLane, const void* data, int32 numBytes) {
    // NOTE: this is running on a thread!
    o_assert(this->id.IsValid());
    
    // parse the data with gliml, just accept all texture formats,
    // GL will complain later if this is not supported
    gliml::context ctx;
    ctx.enable_dxt(true);
    ctx.enable_pvrtc(true);
    ctx.enable_etc2(true);
    if (ctx.load(data, numBytes)) {
        TextureSetup texSetup = this->buildSetup(this->setup, &ctx);
        Ptr<Stream> stream = this->buildStream(data, numBytes);
        SetupAndStream<TextureSetup> setupAndStream(texSetup, stream);
        Gfx::Resource().initResourceFromThread(ioLane, this->id, setupAndStream);
    }
}

//------------------------------------------------------------------------------
void
TextureLoader::Failed(const URL& url, int32 ioLane, IOStatus::Code ioStatus) {
    Gfx::Resource().failResourceFromThread(ioLane, this->id);
}

//------------------------------------------------------------------------------
TextureSetup
TextureLoader::buildSetup(const TextureSetup& blueprint, const gliml::context* ctx) {
    const int32 w = ctx->image_width(0, 0);
    const int32 h = ctx->image_height(0, 0);
    bool hasMipmaps = ctx->num_mipmaps(0) > 0;
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
        default:
            o_error("Unknown compressed pixel format!\n");
            break;
    }
    TextureSetup setup = TextureSetup::FromPixelData(w, h, hasMipmaps, pixelFormat);
    
    // setup mipmap offsets
    o_assert_dbg(TextureSetup::MaxNumMipMaps >= ctx->num_mipmaps(0));
    const uint8* start = (const uint8*) ctx->image_data(0, 0);
    for (int32 i = 0; i < ctx->num_mipmaps(0); i++) {
        const uint8* cur = (const uint8*) ctx->image_data(0, i);
        setup.MipMapOffsets[i] = int32(cur - start);
    }
    return setup;
}

//------------------------------------------------------------------------------
Ptr<Stream>
TextureLoader::buildStream(const void* data, int32 numBytes) {
    o_assert_dbg(nullptr != data);
    o_assert_dbg(0 < numBytes);
    Ptr<MemoryStream> stream = MemoryStream::Create();
    stream->Reserve(numBytes);
    stream->Open(OpenMode::WriteOnly);
    stream->Write(data, numBytes);
    stream->Close();
    return stream;
}

} // namespace Oryol