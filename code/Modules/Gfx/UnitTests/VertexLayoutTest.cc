//------------------------------------------------------------------------------
//  VertexLayoutTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Gfx/Core/VertexLayout.h"

using namespace Oryol;

TEST(VertexLayoutTest) {
    VertexLayout layout;
    CHECK(layout.Empty());
    CHECK(0 == layout.NumComponents());
    
    layout.Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::UByte4N)
        .Add(VertexLayout::Component(VertexAttr::TexCoord0, VertexFormat::Float2));
    CHECK(!layout.Empty());
    CHECK(3 == layout.NumComponents());
    
    const VertexLayout::Component& comp0 = layout.ComponentAt(0);
    CHECK(comp0.Attr == VertexAttr::Position);
    CHECK(comp0.Format == VertexFormat::Float3);
    CHECK(comp0.ByteSize() == 12);
    const VertexLayout::Component& comp1 = layout.ComponentAt(1);
    CHECK(comp1.Attr == VertexAttr::Normal);
    CHECK(comp1.Format == VertexFormat::UByte4N);
    CHECK(comp1.ByteSize() == 4);
    const VertexLayout::Component& comp2 = layout.ComponentAt(2);
    CHECK(comp2.Attr == VertexAttr::TexCoord0);
    CHECK(comp2.Format == VertexFormat::Float2);
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

    // test appending and hashes
    VertexLayout l0, l1, l2, l3;
    l0.Add(VertexAttr::Position, VertexFormat::Float3)
      .Add(VertexAttr::Normal, VertexFormat::UByte4N)
      .Add(VertexAttr::TexCoord0, VertexFormat::Short2);
    l1.Add(VertexAttr::Instance0, VertexFormat::Float4);
    l2.Add(VertexAttr::Position, VertexFormat::Float3)
      .Add(VertexAttr::Normal, VertexFormat::UByte4N)
      .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    l3.Add(VertexAttr::Position, VertexFormat::Float3)
      .Add(VertexAttr::Normal, VertexFormat::UByte4N)
      .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    CHECK(l0.Hash() == l0.Hash());
    CHECK(l0.Hash() != l1.Hash());
    CHECK(l0.Hash() != l2.Hash());
    CHECK(l0.Hash() != l3.Hash());
    CHECK(l1.Hash() == l1.Hash());
    CHECK(l1.Hash() != l2.Hash());
    CHECK(l1.Hash() != l3.Hash());
    CHECK(l2.Hash() == l2.Hash());
    CHECK(l2.Hash() == l3.Hash());
    CHECK(l3.Hash() == l3.Hash());

    // test appending
    VertexLayout l4 = l0;
    l4.Append(l1);
    CHECK(l4.ComponentAt(0).Attr == VertexAttr::Position);
    CHECK(l4.ComponentAt(1).Attr == VertexAttr::Normal);
    CHECK(l4.ComponentAt(2).Attr == VertexAttr::TexCoord0);
    CHECK(l4.ComponentAt(3).Attr == VertexAttr::Instance0);
    CHECK(l4.ComponentAt(0).Format == VertexFormat::Float3);
    CHECK(l4.ComponentAt(1).Format == VertexFormat::UByte4N);
    CHECK(l4.ComponentAt(2).Format == VertexFormat::Short2);
    CHECK(l4.ComponentAt(3).Format == VertexFormat::Float4);

    // test combined hash (this is used to find matching mesh / vertex shader layout signatures
    uint64 hash0 = VertexLayout::CombinedHash(l4, l4);
    uint64 hash1 = VertexLayout::CombinedHash(l4, l4);
    uint64 hash2 = VertexLayout::CombinedHash(l4, l1);
    uint64 hash3 = VertexLayout::CombinedHash(l4, l2);
    CHECK(hash0 == hash1);
    CHECK(hash0 != hash2);
    CHECK(hash0 != hash3);
    CHECK(hash1 != hash2);
    CHECK(hash2 != hash3);
}
