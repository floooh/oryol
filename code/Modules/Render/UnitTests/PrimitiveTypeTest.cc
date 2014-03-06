//------------------------------------------------------------------------------
//  PrimitiveTypeTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/PrimitiveType.h"
#include "Core/String/String.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

TEST(PrimitiveTypeTest) {
    CHECK(PrimitiveType::NumPrimitiveTypes == 7);
    
    CHECK(PrimitiveType::FromString("Points") == PrimitiveType::Points);
    CHECK(PrimitiveType::FromString("LineStrip") == PrimitiveType::LineStrip);
    CHECK(PrimitiveType::FromString("LineLoop") == PrimitiveType::LineLoop);
    CHECK(PrimitiveType::FromString("Lines") == PrimitiveType::Lines);
    CHECK(PrimitiveType::FromString("TriangleStrip") == PrimitiveType::TriangleStrip);
    CHECK(PrimitiveType::FromString("TriangleFan") == PrimitiveType::TriangleFan);
    CHECK(PrimitiveType::FromString("Triangles") == PrimitiveType::Triangles);
    
    CHECK(String(PrimitiveType::ToString(PrimitiveType::Points)) == "Points");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::LineStrip)) == "LineStrip");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::LineLoop)) == "LineLoop");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::Lines)) == "Lines");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::TriangleStrip)) == "TriangleStrip");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::TriangleFan)) == "TriangleFan");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::Triangles)) == "Triangles");
}

