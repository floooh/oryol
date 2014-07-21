//------------------------------------------------------------------------------
//  MeshSetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Setup/MeshSetup.h"

using namespace Oryol;
using namespace Oryol::Render;
    
TEST(MeshSetupTest) {
    
    const MeshSetup s0 = MeshSetup::FromFile("s0");
    CHECK(s0.ShouldSetupFromFile());
    CHECK(!s0.ShouldSetupFromData());
    CHECK(!s0.ShouldSetupEmpty());
    CHECK(s0.Locator == "s0");
    CHECK(s0.VertexUsage == Usage::Immutable);
    CHECK(s0.IndexUsage == Usage::Immutable);
    CHECK(s0.IOLane == 0);
    CHECK(s0.NumVertices == 0);
    CHECK(s0.NumIndices == 0);
    CHECK(s0.Layout.Empty());
    CHECK(s0.IndicesType == IndexType::None);
    
    const MeshSetup s1 = MeshSetup::FromFile("s1", 1, Usage::Stream, Usage::Dynamic);
    CHECK(s1.ShouldSetupFromFile());
    CHECK(!s1.ShouldSetupFromData());
    CHECK(!s1.ShouldSetupEmpty());
    CHECK(s1.Locator == "s1");
    CHECK(s1.VertexUsage == Usage::Stream);
    CHECK(s1.IndexUsage == Usage::Dynamic);
    CHECK(s1.IOLane == 1);
    
    const MeshSetup s2 = MeshSetup::FromFile("s2", s1);
    CHECK(s2.ShouldSetupFromFile());
    CHECK(!s2.ShouldSetupFromData());
    CHECK(!s2.ShouldSetupEmpty());
    CHECK(s2.Locator == "s2");
    CHECK(s2.VertexUsage == Usage::Stream);
    CHECK(s2.IndexUsage == Usage::Dynamic);
    CHECK(s2.IOLane == 1);
    
    const MeshSetup s3 = MeshSetup::FromData("s3");
    CHECK(!s3.ShouldSetupFromFile());
    CHECK(!s3.ShouldSetupEmpty());
    CHECK(s3.ShouldSetupFromData());
    CHECK(s3.Locator == "s3");
    CHECK(s3.VertexUsage == Usage::Immutable);
    CHECK(s3.IndexUsage == Usage::Immutable);
    CHECK(s0.IOLane == 0);
    
    const MeshSetup s4 = MeshSetup::FromData("s4", Usage::Stream, Usage::Dynamic);
    CHECK(!s4.ShouldSetupFromFile());
    CHECK(!s4.ShouldSetupEmpty());
    CHECK(s4.ShouldSetupFromData());
    CHECK(s4.Locator == "s4");
    CHECK(s4.VertexUsage == Usage::Stream);
    CHECK(s4.IndexUsage == Usage::Dynamic);
    CHECK(s4.IOLane == 0);
    
    const MeshSetup s5 = MeshSetup::FromData("s5", s4);
    CHECK(!s5.ShouldSetupFromFile());
    CHECK(!s5.ShouldSetupEmpty());
    CHECK(s5.ShouldSetupFromData());
    CHECK(s5.Locator == "s5");
    CHECK(s5.VertexUsage == Usage::Stream);
    CHECK(s5.IndexUsage == Usage::Dynamic);
    CHECK(s5.IOLane == 0);
    
    MeshSetup s6 = MeshSetup::CreateEmpty("s6", 128, Usage::Stream);
    s6.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    s6.Layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    CHECK(!s6.ShouldSetupFromFile());
    CHECK(!s6.ShouldSetupFromData());
    CHECK(s6.ShouldSetupEmpty());
    CHECK(s6.Locator == "s6");
    CHECK(s6.VertexUsage == Usage::Stream);
    CHECK(s6.IndexUsage == Usage::InvalidUsage);
    CHECK(s6.IOLane == 0);
    CHECK(s6.NumVertices == 128);
    CHECK(s6.NumIndices == 0);
    CHECK(s6.IndicesType == IndexType::None);
    CHECK(s6.Layout.NumComponents() == 2);
    CHECK(s6.Layout.Component(0).Attr() == VertexAttr::Position);
    CHECK(s6.Layout.Component(0).Format() == VertexFormat::Float3);
    CHECK(s6.Layout.Component(1).Attr() == VertexAttr::TexCoord0);
    CHECK(s6.Layout.Component(1).Format() == VertexFormat::Float2);
    
    MeshSetup s7 = MeshSetup::CreateEmpty("s7", 256, Usage::Dynamic, IndexType::Index16, 512, Usage::Stream);
    s7.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    s7.Layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    s7.AddPrimitiveGroup(PrimitiveGroup(PrimitiveType::Triangles, 0, 64));
    CHECK(!s7.ShouldSetupFromFile());
    CHECK(!s7.ShouldSetupFromData());
    CHECK(s7.ShouldSetupEmpty());
    CHECK(s7.Locator == "s7");
    CHECK(s7.VertexUsage == Usage::Dynamic);
    CHECK(s7.IndexUsage == Usage::Stream);
    CHECK(s7.IOLane == 0);
    CHECK(s7.NumVertices == 256);
    CHECK(s7.NumIndices == 512);
    CHECK(s7.IndicesType == IndexType::Index16);
    CHECK(s7.Layout.NumComponents() == 2);
    CHECK(s7.NumPrimitiveGroups() == 1);
    CHECK(s7.PrimitiveGroup(0).GetPrimitiveType() == PrimitiveType::Triangles);
    CHECK(s7.PrimitiveGroup(0).GetBaseElement() == 0);
    CHECK(s7.PrimitiveGroup(0).GetNumElements() == 64);
}

