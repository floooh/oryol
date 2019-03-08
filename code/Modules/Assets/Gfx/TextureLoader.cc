//------------------------------------------------------------------------------
//  TextureLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureLoader.h"
#include "IO/IO.h"
#include "Gfx/Gfx.h"
#define GLIML_ASSERT(x) o_assert(x)
#include "gliml.h"

namespace Oryol {

//------------------------------------------------------------------------------
static TextureDesc buildDesc(const TextureDesc& blueprint, const gliml::context& ctx, const uint8_t* data) {
    const int w = ctx.image_width(0, 0);
    const int h = ctx.image_height(0, 0);
    const int d = ctx.image_depth(0, 0);
    const int numFaces = ctx.num_faces();
    const int numMips = ctx.num_mipmaps(0);
    PixelFormat::Code pixelFormat = PixelFormat::Invalid;
    switch(ctx.image_internal_format()) {
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
            if (ctx.image_type() == GLIML_GL_UNSIGNED_BYTE) {
                pixelFormat = PixelFormat::RGB8;
            }
            else if (ctx.image_type() == GLIML_GL_UNSIGNED_SHORT_5_6_5) {
                pixelFormat = PixelFormat::R5G6B5;
            }
            break;

        case GLIML_GL_RGBA:
            switch (ctx.image_type()) {
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
    o_assert(PixelFormat::Invalid != pixelFormat);
    auto desc = TextureDesc(blueprint)
        .SetWidth(w)
        .SetHeight(h)
        .SetNumMipMaps(numMips)
        .SetFormat(pixelFormat);
    switch (ctx.texture_target()) {
        case GLIML_GL_TEXTURE_2D:
            desc.SetType(TextureType::Texture2D);
            break;
        case GLIML_GL_TEXTURE_3D:
            desc.SetType(TextureType::Texture3D)
                .SetDepth(d);
            break;
        case GLIML_GL_TEXTURE_CUBE_MAP:
            desc.SetType(TextureType::TextureCube);
            break;
        default:
            o_error("Unknown texture type!\n");
            break;
    }

    // setup mipmap content
    o_assert_dbg(GfxConfig::MaxNumTextureMipMaps >= ctx.num_mipmaps(0));
    for (int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
        for (int mipIndex = 0; mipIndex < numMips; mipIndex++) {
            desc.SetMipContent(faceIndex, mipIndex, ctx.image_data(faceIndex, mipIndex));
            desc.SetMipSize(faceIndex, mipIndex, ctx.image_size(faceIndex, mipIndex));
        }
    }
    return desc;
}

//------------------------------------------------------------------------------
Id
TextureLoader::Load(const TextureDesc& desc) {
    Id resId = Gfx::AllocTexture(desc.Locator);
    IO::Load(URL(desc.Locator.Location()), [resId, desc](IO::LoadResult result) {
        const uint8_t* data = result.Data.Data();
        const int dataSize = result.Data.Size();
        gliml::context ctx;
        ctx.enable_dxt(true);
        ctx.enable_pvrtc(true);
        ctx.enable_etc2(true);
        if (ctx.load(data, dataSize)) {
            TextureDesc initDesc = buildDesc(desc, ctx, data);
            Gfx::InitTexture(resId, initDesc);
        }
    },
    [resId](const URL& url, IOStatus::Code ioStatus) {
        Gfx::FailTexture(resId);
    });
    return resId;
}

} // namespace Oryol
