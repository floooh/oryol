//------------------------------------------------------------------------------
//  VertexLayoutTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Core/VertexLayout.h"

using namespace Oryol;
using namespace Oryol::Render;

TEST(VertexLayoutTest) {
    VertexLayout layout;
    CHECK(layout.Empty());
    CHECK(0 == layout.NumComponents());
    
    layout.Add(VertexAttr::Position, VertexFormat::Float3);
    layout.Add(VertexAttr::Normal, VertexFormat::UByte4N);
    layout.Add(VertexComponent(VertexAttr::TexCoord0, VertexFormat::Float2));
    CHECK(!layout.Empty());
    CHECK(3 == layout.NumComponents());
    
    const VertexComponent& comp0 = layout.Component(0);
    CHECK(comp0.Attr() == VertexAttr::Position);
    CHECK(comp0.Format() == VertexFormat::Float3);
    CHECK(comp0.ByteSize() == 12);
    const VertexComponent& comp1 = layout.Component(1);
    CHECK(comp1.Attr() == VertexAttr::Normal);
    CHECK(comp1.Format() == VertexFormat::UByte4N);
    CHECK(comp1.ByteSize() == 4);
    const VertexComponent& comp2 = layout.Component(2);
    CHECK(comp2.Attr() == VertexAttr::TexCoord0);
    CHECK(comp2.Format() == VertexFormat::Float2);
    CHECK(comp2.ByteSize() == 8);
    
    CHECK(layout.ByteSize() == 24);
    CHECK(layout.ComponentByteOffset(0) == 0);
    CHECK(layout.ComponentByteOffset(1) == 12);
    CHECK(layout.ComponentByteOffset(2) == 16);
    
    CHECK(layout.ComponentIndexByVertexAttr(VertexAttr::Position) == 0);
    CHECK(layout.ComponentIndexByVertexAttr(VertexAttr::Normal) == 1);
    CHECK(layout.ComponentIndexByVertexAttr(VertexAttr::TexCoord0) == 2);
    CHECK(layout.ComponentIndexByVertexAttr(VertexAttr::TexCoord1) == InvalidIndex);
    CHECK(layout.Contains(VertexAttr::Position));
    CHECK(layout.Contains(VertexAttr::Normal));
    CHECK(layout.Contains(VertexAttr::TexCoord0));
    CHECK(!layout.Contains(VertexAttr::TexCoord1));
    
    layout.Clear();
    CHECK(layout.Empty());
    CHECK(0 == layout.NumComponents());
}
