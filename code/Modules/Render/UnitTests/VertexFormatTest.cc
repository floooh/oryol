//------------------------------------------------------------------------------
//  VertexFormatTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/VertexFormat.h"
#include "Core/String/String.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

TEST(VertexFormatTest) {
    CHECK(VertexFormat::NumVertexFormats == 10);
    
    CHECK(VertexFormat::FromString("Float") == VertexFormat::Float);
    CHECK(VertexFormat::FromString("Float2") == VertexFormat::Float2);
    CHECK(VertexFormat::FromString("Float3") == VertexFormat::Float3);
    CHECK(VertexFormat::FromString("Float4") == VertexFormat::Float4);
    CHECK(VertexFormat::FromString("UByte4") == VertexFormat::UByte4);
    CHECK(VertexFormat::FromString("UByte4N") == VertexFormat::UByte4N);
    CHECK(VertexFormat::FromString("Short2") == VertexFormat::Short2);
    CHECK(VertexFormat::FromString("Short2N") == VertexFormat::Short2N);
    CHECK(VertexFormat::FromString("Short4") == VertexFormat::Short4);
    CHECK(VertexFormat::FromString("Short4N") == VertexFormat::Short4N);
    
    CHECK(String(VertexFormat::ToString(VertexFormat::Float)) == "Float");
    CHECK(String(VertexFormat::ToString(VertexFormat::Float2)) == "Float2");
    CHECK(String(VertexFormat::ToString(VertexFormat::Float3)) == "Float3");
    CHECK(String(VertexFormat::ToString(VertexFormat::Float4)) == "Float4");
    CHECK(String(VertexFormat::ToString(VertexFormat::UByte4)) == "UByte4");
    CHECK(String(VertexFormat::ToString(VertexFormat::UByte4N)) == "UByte4N");
    CHECK(String(VertexFormat::ToString(VertexFormat::Short2)) == "Short2");
    CHECK(String(VertexFormat::ToString(VertexFormat::Short2N)) == "Short2N");
    CHECK(String(VertexFormat::ToString(VertexFormat::Short4)) == "Short4");
    CHECK(String(VertexFormat::ToString(VertexFormat::Short4N)) == "Short4N");

    CHECK(VertexFormat::ByteSize(VertexFormat::Float) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Float2) == 8);
    CHECK(VertexFormat::ByteSize(VertexFormat::Float3) == 12);
    CHECK(VertexFormat::ByteSize(VertexFormat::Float4) == 16);
    CHECK(VertexFormat::ByteSize(VertexFormat::UByte4) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::UByte4N) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short2) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short2N) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short4) == 8);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short4N) == 8);
}