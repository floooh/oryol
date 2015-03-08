//------------------------------------------------------------------------------
//  MeshBuilderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Assets/Gfx/MeshBuilder.h"
#include <cstring>

using namespace Oryol;

//------------------------------------------------------------------------------
TEST(MeshBuilderTest) {
    
    // build a quad with 4 vertices and 2 triangles
    MeshBuilder mb;
    mb.NumVertices = 4;
    mb.NumIndices = 6;
    mb.IndicesType = IndexType::Index16;
    mb.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    mb.PrimitiveGroups.Add(PrimitiveType::Triangles, 0, 6);
    mb.Begin()
        // positions
        .Vertex(0, VertexAttr::Position, 0.0f, 0.0f, 0.0f)  // top-left
        .Vertex(1, VertexAttr::Position, 1.0f, 0.0f, 0.0f)  // top-right
        .Vertex(2, VertexAttr::Position, 1.0f, 1.0f, 0.0f)  // bottom-right
        .Vertex(3, VertexAttr::Position, 0.0f, 1.0f, 0.0f)  // bottom-left
    
        // uvs
        .Vertex(0, VertexAttr::TexCoord0, 0.0f, 0.0f)
        .Vertex(1, VertexAttr::TexCoord0, 1.0f, 0.0f)
        .Vertex(2, VertexAttr::TexCoord0, 1.0f, 1.0f)
        .Vertex(3, VertexAttr::TexCoord0, 0.0f, 1.0f)
    
        // indices
        .Triangle(0, 0, 1, 2)
        .Triangle(1, 0, 2, 3)

        .End();

    // get the resulting stream object
    const Ptr<Stream>& stream = mb.Result().Stream;
    CHECK(!stream->IsOpen());
    const MeshSetup& meshSetup = mb.Result().Setup;
    CHECK(meshSetup.ShouldSetupFromStream());
    
    // check MeshSetup
    CHECK(meshSetup.NumVertices == 4);
    CHECK(meshSetup.NumIndices == 6);
    CHECK(meshSetup.IndicesType == IndexType::Index16);
    CHECK(meshSetup.VertexUsage == Usage::Immutable);
    CHECK(meshSetup.IndexUsage == Usage::Immutable);
    CHECK(meshSetup.Layout.NumComponents() == 2);
    CHECK(meshSetup.Layout.Component(0).Attr == VertexAttr::Position);
    CHECK(meshSetup.Layout.Component(0).Format == VertexFormat::Float3);
    CHECK(meshSetup.Layout.Component(1).Attr == VertexAttr::TexCoord0);
    CHECK(meshSetup.Layout.Component(1).Format == VertexFormat::Float2);
    CHECK(meshSetup.NumPrimitiveGroups() == 1);
    CHECK(meshSetup.PrimitiveGroup(0).PrimType == PrimitiveType::Triangles);
    CHECK(meshSetup.PrimitiveGroup(0).BaseElement == 0);
    CHECK(meshSetup.PrimitiveGroup(0).NumElements == 6);
    
    // see MeshBuilder header for those sizes
    const uint32 vbufSize = 4 * 5 * sizeof(float32);
    const uint32 ibufSize = 6 * sizeof(uint16);
    const uint32 allDataSize = vbufSize + ibufSize;
    CHECK(stream->Size() == allDataSize);
    
    // check the generated data
    stream->Open(OpenMode::ReadOnly);
    const uint8* maxPtr = nullptr;
    const uint8* ptr = stream->MapRead(&maxPtr);
    o_assert(nullptr != ptr);
    CHECK(ptr != nullptr);
    CHECK(maxPtr == ptr + allDataSize);
    
    // check vertices
    const float32* vPtr = (const float32*) ptr;
    CHECK(vPtr[0] == 0.0f); CHECK(vPtr[1] == 0.0f); CHECK(vPtr[2] == 0.0f);
    CHECK(vPtr[3] == 0.0f); CHECK(vPtr[4] == 0.0f);
    CHECK(vPtr[5] == 1.0f); CHECK(vPtr[6] == 0.0f); CHECK(vPtr[7] == 0.0f);
    CHECK(vPtr[8] == 1.0f); CHECK(vPtr[9] == 0.0f);
    CHECK(vPtr[10] == 1.0f); CHECK(vPtr[11] == 1.0f); CHECK(vPtr[12] == 0.0f);
    CHECK(vPtr[13] == 1.0f); CHECK(vPtr[14] == 1.0f);
    CHECK(vPtr[15] == 0.0f); CHECK(vPtr[16] == 1.0f); CHECK(vPtr[17] == 0.0f);
    CHECK(vPtr[18] == 0.0f); CHECK(vPtr[19] == 1.0f);
    
    // check indices
    const uint16* iPtr = (const uint16*) &(vPtr[20]);
    CHECK(iPtr[0] == 0); CHECK(iPtr[1] == 1); CHECK(iPtr[2] == 2);
    CHECK(iPtr[3] == 0); CHECK(iPtr[4] == 2); CHECK(iPtr[5] == 3);
    
    CHECK((uint8*)&(iPtr[6]) == maxPtr);
    
    stream->Close();
}
