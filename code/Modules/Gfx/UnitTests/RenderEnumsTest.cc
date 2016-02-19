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

    CHECK(PixelFormat::NumBits(PixelFormat::DEPTH, PixelChannel::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTH, PixelChannel::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTH, PixelChannel::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTH, PixelChannel::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTH, PixelChannel::Depth) == 16);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTH, PixelChannel::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::DEPTHSTENCIL, PixelChannel::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTHSTENCIL, PixelChannel::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTHSTENCIL, PixelChannel::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTHSTENCIL, PixelChannel::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTHSTENCIL, PixelChannel::Depth) == 24);
    CHECK(PixelFormat::NumBits(PixelFormat::DEPTHSTENCIL, PixelChannel::Stencil) == 8);

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
    for (uint32 pf = 0; pf < PixelFormat::NumPixelFormats; pf++) {
        if ((pf != PixelFormat::RGBA8) &&
            (pf != PixelFormat::RGB8) &&
            (pf != PixelFormat::R5G6B5) &&
            (pf != PixelFormat::R5G5B5A1) &&
            (pf != PixelFormat::RGBA4) &&
            (pf != PixelFormat::L8) &&
            (pf != PixelFormat::DEPTH) &&
            (pf != PixelFormat::DEPTHSTENCIL) &&
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
    CHECK(PixelFormat::ByteSize(PixelFormat::DEPTH) == 2);
    CHECK(PixelFormat::ByteSize(PixelFormat::DEPTHSTENCIL) == 4);
    CHECK(PixelFormat::ByteSize(PixelFormat::RGBA32F) == 16);
    CHECK(PixelFormat::ByteSize(PixelFormat::RGBA16F) == 8);
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

