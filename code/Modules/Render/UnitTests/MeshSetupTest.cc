//------------------------------------------------------------------------------
//  MeshSetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Setup/MeshSetup.h"

using namespace Oryol;
using namespace Oryol::Render;
    
TEST(MeshSetupTest) {
    
    MeshSetup s0 = MeshSetup::FromFile("s0");
    CHECK(s0.ShouldSetupFromFile());
    CHECK(!s0.ShouldSetupFromData());
    CHECK(!s0.ShouldSetupEmpty());
    CHECK(s0.GetLocator() == "s0");
    CHECK(s0.GetVertexUsage() == Usage::Immutable);
    CHECK(s0.GetIndexUsage() == Usage::Immutable);
    CHECK(s0.GetIOLane() == 0);
    CHECK(s0.GetNumVertices() == 0);
    CHECK(s0.GetNumIndices() == 0);
    CHECK(s0.VertexLayout().Empty());
    CHECK(s0.GetIndexType() == IndexType::None);
    
    MeshSetup s1 = MeshSetup::FromFile("s1", 1, Usage::DynamicStream, Usage::DynamicWrite);
    CHECK(s1.ShouldSetupFromFile());
    CHECK(!s1.ShouldSetupFromData());
    CHECK(!s1.ShouldSetupEmpty());
    CHECK(s1.GetLocator() == "s1");
    CHECK(s1.GetVertexUsage() == Usage::DynamicStream);
    CHECK(s1.GetIndexUsage() == Usage::DynamicWrite);
    CHECK(s1.GetIOLane() == 1);
    
    MeshSetup s2 = MeshSetup::FromFile("s2", s1);
    CHECK(s2.ShouldSetupFromFile());
    CHECK(!s2.ShouldSetupFromData());
    CHECK(!s2.ShouldSetupEmpty());
    CHECK(s2.GetLocator() == "s2");
    CHECK(s2.GetVertexUsage() == Usage::DynamicStream);
    CHECK(s2.GetIndexUsage() == Usage::DynamicWrite);
    CHECK(s2.GetIOLane() == 1);
    
    MeshSetup s3 = MeshSetup::FromData("s3");
    CHECK(!s3.ShouldSetupFromFile());
    CHECK(!s3.ShouldSetupEmpty());
    CHECK(s3.ShouldSetupFromData());
    CHECK(s3.GetLocator() == "s3");
    CHECK(s3.GetVertexUsage() == Usage::Immutable);
    CHECK(s3.GetIndexUsage() == Usage::Immutable);
    CHECK(s0.GetIOLane() == 0);
    
    MeshSetup s4 = MeshSetup::FromData("s4", Usage::DynamicStream, Usage::DynamicWrite);
    CHECK(!s4.ShouldSetupFromFile());
    CHECK(!s4.ShouldSetupEmpty());
    CHECK(s4.ShouldSetupFromData());
    CHECK(s4.GetLocator() == "s4");
    CHECK(s4.GetVertexUsage() == Usage::DynamicStream);
    CHECK(s4.GetIndexUsage() == Usage::DynamicWrite);
    CHECK(s4.GetIOLane() == 0);
    
    MeshSetup s5 = MeshSetup::FromData("s5", s4);
    CHECK(!s5.ShouldSetupFromFile());
    CHECK(!s5.ShouldSetupEmpty());
    CHECK(s5.ShouldSetupFromData());
    CHECK(s5.GetLocator() == "s5");
    CHECK(s5.GetVertexUsage() == Usage::DynamicStream);
    CHECK(s5.GetIndexUsage() == Usage::DynamicWrite);
    CHECK(s5.GetIOLane() == 0);
    
    MeshSetup s6 = MeshSetup::CreateEmpty("s6", 128, Usage::DynamicStream);
    s6.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    s6.VertexLayout().Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    CHECK(!s6.ShouldSetupFromFile());
    CHECK(!s6.ShouldSetupFromData());
    CHECK(s6.ShouldSetupEmpty());
    CHECK(s6.GetLocator() == "s6");
    CHECK(s6.GetVertexUsage() == Usage::DynamicStream);
    CHECK(s6.GetIndexUsage() == Usage::InvalidUsage);
    CHECK(s6.GetIOLane() == 0);
    CHECK(s6.GetNumVertices() == 128);
    CHECK(s6.GetNumIndices() == 0);
    CHECK(s6.GetIndexType() == IndexType::None);
    CHECK(s6.VertexLayout().NumComponents() == 2);
    CHECK(s6.VertexLayout().Component(0).Attr() == VertexAttr::Position);
    CHECK(s6.VertexLayout().Component(0).Format() == VertexFormat::Float3);
    CHECK(s6.VertexLayout().Component(1).Attr() == VertexAttr::TexCoord0);
    CHECK(s6.VertexLayout().Component(1).Format() == VertexFormat::Float2);
    
    MeshSetup s7 = MeshSetup::CreateEmpty("s7", 256, Usage::DynamicWrite, IndexType::Index16, 512, Usage::DynamicStream);
    s7.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    s7.VertexLayout().Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    s7.AddPrimitiveGroup(PrimitiveGroup(PrimitiveType::Triangles, 0, 64));
    CHECK(!s7.ShouldSetupFromFile());
    CHECK(!s7.ShouldSetupFromData());
    CHECK(s7.ShouldSetupEmpty());
    CHECK(s7.GetLocator() == "s7");
    CHECK(s7.GetVertexUsage() == Usage::DynamicWrite);
    CHECK(s7.GetIndexUsage() == Usage::DynamicStream);
    CHECK(s7.GetIOLane() == 0);
    CHECK(s7.GetNumVertices() == 256);
    CHECK(s7.GetNumIndices() == 512);
    CHECK(s7.GetIndexType() == IndexType::Index16);
    CHECK(s7.VertexLayout().NumComponents() == 2);
    CHECK(s7.GetNumPrimitiveGroups() == 1);
    CHECK(s7.GetPrimitiveGroup(0).GetPrimitiveType() == PrimitiveType::Triangles);
    CHECK(s7.GetPrimitiveGroup(0).GetBaseElement() == 0);
    CHECK(s7.GetPrimitiveGroup(0).GetNumElements() == 64);
}

