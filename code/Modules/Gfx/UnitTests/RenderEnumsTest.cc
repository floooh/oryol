//------------------------------------------------------------------------------
//  RenderEnumsTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/String.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/gl/gl_impl.h"
#include <array>

using namespace Oryol;

//------------------------------------------------------------------------------
TEST(PixelFormatTest) {
    CHECK(PixelFormat::NumPixelFormats == 20);
}

//------------------------------------------------------------------------------
TEST(PixelFormatChannelBitsTest) {

    CHECK(PixelFormat::NumBits(PixelFormat::RGBA8, PixelChannel::Red) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA8, PixelChannel::Green) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA8, PixelChannel::Blue) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA8, PixelChannel::Alpha) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA8, PixelChannel::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA8, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::RGB8, PixelChannel::Red) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::RGB8, PixelChannel::Green) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::RGB8, PixelChannel::Blue) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::RGB8, PixelChannel::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::RGB8, PixelChannel::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::RGB8, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelChannel::Red) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelChannel::Green) == 6);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelChannel::Blue) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelChannel::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelChannel::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelChannel::Red) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelChannel::Green) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelChannel::Blue) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelChannel::Alpha) == 1);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelChannel::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::RGBA4, PixelChannel::Red) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA4, PixelChannel::Green) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA4, PixelChannel::Blue) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA4, PixelChannel::Alpha) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA4, PixelChannel::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA4, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelChannel::Red) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelChannel::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelChannel::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelChannel::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelChannel::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelChannel::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelChannel::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelChannel::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelChannel::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelChannel::Depth) == 16);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelChannel::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelChannel::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelChannel::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelChannel::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelChannel::Depth) == 32);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelChannel::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelChannel::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelChannel::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelChannel::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelChannel::Depth) == 24);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelChannel::Stencil) == 8);

    CHECK(PixelFormat::NumBits(PixelFormat::RGBA32F, PixelChannel::Red) == 32);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA32F, PixelChannel::Green) == 32);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA32F, PixelChannel::Blue) == 32);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA32F, PixelChannel::Alpha) == 32);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA32F, PixelChannel::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA32F, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::RGBA16F, PixelChannel::Red) == 16);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA16F, PixelChannel::Green) == 16);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA16F, PixelChannel::Blue) == 16);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA16F, PixelChannel::Alpha) == 16);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA16F, PixelChannel::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::RGBA16F, PixelChannel::Stencil) == 0);

    // all other pixel formats must return 0 for all channels
    for (int pf = 0; pf < PixelFormat::NumPixelFormats; pf++) {
        if ((pf != PixelFormat::RGBA8) &&
            (pf != PixelFormat::RGB8) &&
            (pf != PixelFormat::R5G6B5) &&
            (pf != PixelFormat::R5G5B5A1) &&
            (pf != PixelFormat::RGBA4) &&
            (pf != PixelFormat::L8) &&
            (pf != PixelFormat::D16) &&
            (pf != PixelFormat::D32) &&
            (pf != PixelFormat::D24S8) &&
            (pf != PixelFormat::RGBA32F) &&
            (pf != PixelFormat::RGBA16F)) {
            std::array<PixelChannel::Bits, 6> channels = { {PixelChannel::Red, PixelChannel::Green, PixelChannel::Blue, PixelChannel::Alpha, PixelChannel::Depth, PixelChannel::Stencil} };
            for (PixelChannel::Bits chn : channels) {
                CHECK(PixelFormat::NumBits((PixelFormat::Code)pf, chn) == 0);
            }
        }
    }
}

//------------------------------------------------------------------------------
TEST(PixelFormatByteSizeTest) {
    CHECK(PixelFormat::ByteSize(PixelFormat::RGBA8) == 4);
    CHECK(PixelFormat::ByteSize(PixelFormat::RGB8) == 3);
    CHECK(PixelFormat::ByteSize(PixelFormat::R5G6B5) == 2);
    CHECK(PixelFormat::ByteSize(PixelFormat::R5G5B5A1) == 2);
    CHECK(PixelFormat::ByteSize(PixelFormat::RGBA4) == 2);
    CHECK(PixelFormat::ByteSize(PixelFormat::L8) == 1);
    CHECK(PixelFormat::ByteSize(PixelFormat::D16) == 2);
    CHECK(PixelFormat::ByteSize(PixelFormat::D32) == 4);
    CHECK(PixelFormat::ByteSize(PixelFormat::D24S8) == 4);
    CHECK(PixelFormat::ByteSize(PixelFormat::RGBA32F) == 16);
    CHECK(PixelFormat::ByteSize(PixelFormat::RGBA16F) == 8);
}

//------------------------------------------------------------------------------
TEST(TextureTypeTest) {
    CHECK(TextureType::NumTextureTypes == 3);
    CHECK(TextureType::Texture2D == GL_TEXTURE_2D);
    #if !ORYOL_OPENGLES2
    CHECK(TextureType::Texture3D == GL_TEXTURE_3D);
    #endif
    CHECK(TextureType::TextureCube == GL_TEXTURE_CUBE_MAP);
}

//------------------------------------------------------------------------------
TEST(PrimitiveTypeTest) {
    CHECK(PrimitiveType::NumPrimitiveTypes == 7);
    CHECK(PrimitiveType::Points == GL_POINTS);
    CHECK(PrimitiveType::Lines == GL_LINES);
    CHECK(PrimitiveType::LineLoop == GL_LINE_LOOP);
    CHECK(PrimitiveType::LineStrip == GL_LINE_STRIP);
    CHECK(PrimitiveType::Triangles == GL_TRIANGLES);
    CHECK(PrimitiveType::TriangleStrip == GL_TRIANGLE_STRIP);
    CHECK(PrimitiveType::TriangleFan == GL_TRIANGLE_FAN);
}

//------------------------------------------------------------------------------
TEST(UsageTest) {
    CHECK(Usage::NumUsages == 4);
    CHECK(Usage::Immutable == GL_STATIC_DRAW);
    CHECK(Usage::Static == GL_STATIC_DRAW);
    CHECK(Usage::Dynamic == GL_DYNAMIC_DRAW);
    CHECK(Usage::Stream == GL_STREAM_DRAW);
}

//------------------------------------------------------------------------------
TEST(TextureWrapMode) {
    CHECK(TextureWrapMode::NumTextureWrapModes == 3);
    CHECK(TextureWrapMode::ClampToEdge == GL_CLAMP_TO_EDGE);
    CHECK(TextureWrapMode::Repeat == GL_REPEAT);
    CHECK(TextureWrapMode::MirroredRepeat == GL_MIRRORED_REPEAT);
}

//------------------------------------------------------------------------------
TEST(IndexTypeTest) {
    CHECK(IndexType::NumIndexTypes == 3);
    CHECK(IndexType::Index16 == GL_UNSIGNED_SHORT);
    CHECK(IndexType::Index32 == GL_UNSIGNED_INT);
    CHECK(IndexType::ByteSize(IndexType::Index16) == 2);
    CHECK(IndexType::ByteSize(IndexType::Index32) == 4);
}

//------------------------------------------------------------------------------
TEST(TextureFilterModeTest) {
    CHECK(TextureFilterMode::NumTextureFilterModes == 6);
    CHECK(TextureFilterMode::Nearest == GL_NEAREST);
    CHECK(TextureFilterMode::Linear == GL_LINEAR);
    CHECK(TextureFilterMode::NearestMipmapNearest == GL_NEAREST_MIPMAP_NEAREST);
    CHECK(TextureFilterMode::NearestMipmapLinear == GL_NEAREST_MIPMAP_LINEAR);
    CHECK(TextureFilterMode::LinearMipmapNearest == GL_LINEAR_MIPMAP_NEAREST);
    CHECK(TextureFilterMode::LinearMipmapLinear == GL_LINEAR_MIPMAP_LINEAR);
}

//------------------------------------------------------------------------------
TEST(VertexFormatTest) {
    CHECK(VertexFormat::NumVertexFormats == 12);
    
    CHECK(VertexFormat::ByteSize(VertexFormat::Float) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Float2) == 8);
    CHECK(VertexFormat::ByteSize(VertexFormat::Float3) == 12);
    CHECK(VertexFormat::ByteSize(VertexFormat::Float4) == 16);
    CHECK(VertexFormat::ByteSize(VertexFormat::Byte4) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Byte4N) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::UByte4) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::UByte4N) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short2) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short2N) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short4) == 8);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short4N) == 8);
}

//------------------------------------------------------------------------------
TEST(VertexAttrTest) {
    CHECK(VertexAttr::NumVertexAttrs == 16);
    
    CHECK(String(VertexAttr::ToString(VertexAttr::Position)) == "position");
    CHECK(String(VertexAttr::ToString(VertexAttr::Normal)) == "normal");
    CHECK(String(VertexAttr::ToString(VertexAttr::TexCoord0)) == "texcoord0");
    CHECK(String(VertexAttr::ToString(VertexAttr::TexCoord1)) == "texcoord1");
    CHECK(String(VertexAttr::ToString(VertexAttr::TexCoord2)) == "texcoord2");
    CHECK(String(VertexAttr::ToString(VertexAttr::TexCoord3)) == "texcoord3");
    CHECK(String(VertexAttr::ToString(VertexAttr::Tangent)) == "tangent");
    CHECK(String(VertexAttr::ToString(VertexAttr::Binormal)) == "binormal");
    CHECK(String(VertexAttr::ToString(VertexAttr::Weights)) == "weights");
    CHECK(String(VertexAttr::ToString(VertexAttr::Indices)) == "indices");
    CHECK(String(VertexAttr::ToString(VertexAttr::Color0)) == "color0");
    CHECK(String(VertexAttr::ToString(VertexAttr::Color1)) == "color1");
    CHECK(String(VertexAttr::ToString(VertexAttr::Instance0)) == "instance0");
    CHECK(String(VertexAttr::ToString(VertexAttr::Instance1)) == "instance1");
    CHECK(String(VertexAttr::ToString(VertexAttr::Instance2)) == "instance2");
    CHECK(String(VertexAttr::ToString(VertexAttr::Instance3)) == "instance3");
}

//------------------------------------------------------------------------------
TEST(ShaderTypeTest) {
    CHECK(ShaderType::NumShaderTypes == 2);
    CHECK(ShaderType::VertexShader == GL_VERTEX_SHADER);
    CHECK(ShaderType::FragmentShader == GL_FRAGMENT_SHADER);
}

