//------------------------------------------------------------------------------
//  RenderEnumsTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/String.h"
#include "Render/Core/Enums.h"
#include "Render/gl/gl_impl.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

//------------------------------------------------------------------------------
TEST(PixelFormatTest) {
    CHECK(PixelFormat::NumPixelFormats == 15);
    
}

//------------------------------------------------------------------------------
TEST(PixelFormatChannelBitsTest) {

    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Red) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Green) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Blue) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Alpha) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Red) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Green) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Blue) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Red) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Green) == 6);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Blue) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Red) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Green) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Blue) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Alpha) == 1);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Red) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Green) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Blue) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Alpha) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Red) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Depth) == 16);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Depth) == 32);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Depth) == 24);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Stencil) == 8);

    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Red) == 32);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Stencil) == 0);

    // all other pixel formats must return 0 for all channels
    for (int pf = 0; pf < PixelFormat::NumPixelFormats; pf++) {
        if ((pf != PixelFormat::R8G8B8A8) &&
            (pf != PixelFormat::R8G8B8) &&
            (pf != PixelFormat::R5G6B5) &&
            (pf != PixelFormat::R5G5B5A1) &&
            (pf != PixelFormat::R4G4B4A4) &&
            (pf != PixelFormat::L8) &&
            (pf != PixelFormat::D16) &&
            (pf != PixelFormat::D32) &&
            (pf != PixelFormat::D24S8) &&
            (pf != PixelFormat::R32F)) {
            for (int chn = 0; chn < PixelFormat::NumChannels; chn++) {
                CHECK(PixelFormat::NumBits((PixelFormat::Code)pf, (PixelFormat::Channel)chn) == 0);
            }
        }
    }
}

//------------------------------------------------------------------------------
TEST(PixelFormatByteSizeTest) {
    CHECK(PixelFormat::ByteSize(PixelFormat::R8G8B8A8) == 4);
    CHECK(PixelFormat::ByteSize(PixelFormat::R8G8B8) == 3);
    CHECK(PixelFormat::ByteSize(PixelFormat::R5G6B5) == 2);
    CHECK(PixelFormat::ByteSize(PixelFormat::R5G5B5A1) == 2);
    CHECK(PixelFormat::ByteSize(PixelFormat::R4G4B4A4) == 2);
    CHECK(PixelFormat::ByteSize(PixelFormat::L8) == 1);
    CHECK(PixelFormat::ByteSize(PixelFormat::D16) == 2);
    CHECK(PixelFormat::ByteSize(PixelFormat::D32) == 4);
    CHECK(PixelFormat::ByteSize(PixelFormat::D24S8) == 4);
    CHECK(PixelFormat::ByteSize(PixelFormat::R32F) == 4);
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
    CHECK(Usage::NumUsages == 3);
    CHECK(Usage::Immutable == GL_STATIC_DRAW);
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

