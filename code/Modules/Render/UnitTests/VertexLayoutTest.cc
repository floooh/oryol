//------------------------------------------------------------------------------
//  VertexLayoutTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/VertexLayout.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

TEST(VertexLayoutTest) {
    VertexLayout layout;
    CHECK(layout.Empty());
    CHECK(0 == layout.GetNumComponents());
    
    layout.Add("position", VertexFormat::Float3);
    layout.Add("normal", VertexFormat::UByte4N);
    layout.Add(VertexComponent("uv0", VertexFormat::Float2));
    CHECK(!layout.Empty());
    CHECK(3 == layout.GetNumComponents());
    
    const VertexComponent& comp0 = layout.GetComponent(0);
    CHECK(comp0.GetAttrName() == "position");
    CHECK(comp0.GetFormat() == VertexFormat::Float3);
    CHECK(comp0.GetByteSize() == 12);
    const VertexComponent& comp1 = layout.GetComponent(1);
    CHECK(comp1.GetAttrName() == "normal");
    CHECK(comp1.GetFormat() == VertexFormat::UByte4N);
    CHECK(comp1.GetByteSize() == 4);
    const VertexComponent& comp2 = layout.GetComponent(2);
    CHECK(comp2.GetAttrName() == "uv0");
    CHECK(comp2.GetFormat() == VertexFormat::Float2);
    CHECK(comp2.GetByteSize() == 8);
    
    CHECK(layout.GetByteSize() == 24);
    CHECK(layout.GetComponentByteOffset(0) == 0);
    CHECK(layout.GetComponentByteOffset(1) == 12);
    CHECK(layout.GetComponentByteOffset(2) == 16);
    
    layout.Clear();
    CHECK(layout.Empty());
    CHECK(0 == layout.GetNumComponents());
}
