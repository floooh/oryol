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
    CHECK(s0.GetLocator() == "s0");
    CHECK(s0.GetVertexUsage() == Usage::Immutable);
    CHECK(s0.GetIndexUsage() == Usage::Immutable);
    CHECK(s0.GetIOLane() == 0);
    
    MeshSetup s1 = MeshSetup::FromFile("s1", 1, Usage::DynamicStream, Usage::DynamicWrite);
    CHECK(s1.ShouldSetupFromFile());
    CHECK(!s1.ShouldSetupFromData());
    CHECK(s1.GetLocator() == "s1");
    CHECK(s1.GetVertexUsage() == Usage::DynamicStream);
    CHECK(s1.GetIndexUsage() == Usage::DynamicWrite);
    CHECK(s1.GetIOLane() == 1);
    
    MeshSetup s2 = MeshSetup::FromFile("s2", s1);
    CHECK(s2.ShouldSetupFromFile());
    CHECK(!s2.ShouldSetupFromData());
    CHECK(s2.GetLocator() == "s2");
    CHECK(s2.GetVertexUsage() == Usage::DynamicStream);
    CHECK(s2.GetIndexUsage() == Usage::DynamicWrite);
    CHECK(s2.GetIOLane() == 1);
    
    MeshSetup s3 = MeshSetup::FromData("s3");
    CHECK(!s3.ShouldSetupFromFile());
    CHECK(s3.ShouldSetupFromData());
    CHECK(s3.GetLocator() == "s3");
    CHECK(s3.GetVertexUsage() == Usage::Immutable);
    CHECK(s3.GetIndexUsage() == Usage::Immutable);
    CHECK(s0.GetIOLane() == 0);
    
    MeshSetup s4 = MeshSetup::FromData("s4", Usage::DynamicStream, Usage::DynamicWrite);
    CHECK(!s4.ShouldSetupFromFile());
    CHECK(s4.ShouldSetupFromData());
    CHECK(s4.GetLocator() == "s4");
    CHECK(s4.GetVertexUsage() == Usage::DynamicStream);
    CHECK(s4.GetIndexUsage() == Usage::DynamicWrite);
    CHECK(s4.GetIOLane() == 0);
    
    MeshSetup s5 = MeshSetup::FromData("s5", s4);
    CHECK(!s5.ShouldSetupFromFile());
    CHECK(s5.ShouldSetupFromData());
    CHECK(s5.GetLocator() == "s5");
    CHECK(s5.GetVertexUsage() == Usage::DynamicStream);
    CHECK(s5.GetIndexUsage() == Usage::DynamicWrite);
    CHECK(s5.GetIOLane() == 0);
    
    MeshSetup s6(s0);
    s6.SetVertexUsage(Usage::DynamicStream);
    CHECK(s6.GetVertexUsage() == Usage::DynamicStream);
    s6.SetVertexUsage(Usage::DynamicWrite);
    CHECK(s6.GetVertexUsage() == Usage::DynamicWrite);
    s6.SetIOLane(2);
    CHECK(s6.GetIOLane() == 2);
}
