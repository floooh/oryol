//------------------------------------------------------------------------------
//  MeshBuilderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Util/MeshBuilder.h"

using namespace Oryol;
using namespace Oryol::IO;
using namespace Oryol::Core;
using namespace Oryol::Render;

//------------------------------------------------------------------------------
TEST(MeshBuilderTest) {
    
    // build a quad with 4 vertices and 2 triangles
    MeshBuilder mb;
    mb.SetNumVertices(4);
    mb.SetNumIndices(6);
    mb.SetIndexType(IndexType::Index16);
    mb.AddComponent("position", VertexFormat::Float3);
    mb.AddComponent("abcdefghijklmno", VertexFormat::Float2);     // 16 bytes long, last character should be clipped
    mb.AddPrimitiveGroup(PrimitiveType::Triangles, 0, 6);
    mb.Begin();
    
    // positions
    mb.Vertex(0, 0, 0.0f, 0.0f, 0.0f);  // top-left
    mb.Vertex(1, 0, 1.0f, 0.0f, 0.0f);  // top-right
    mb.Vertex(2, 0, 1.0f, 1.0f, 0.0f);  // bottom-right
    mb.Vertex(3, 0, 0.0f, 1.0f, 0.0f);  // bottom-left
    
    // uvs
    mb.Vertex(0, 1, 0.0f, 0.0f);
    mb.Vertex(1, 1, 1.0f, 0.0f);
    mb.Vertex(2, 1, 1.0f, 1.0f);
    mb.Vertex(3, 1, 0.0f, 1.0f);
    
    // indices
    mb.Triangle(0, 0, 1, 2);
    mb.Triangle(1, 0, 2, 3);

    mb.End();

    // get the resulting stream object
    const Ptr<Stream>& stream = mb.GetStream();
    CHECK(!stream->IsOpen());
    
    // see MeshBuilder header for those sizes
    const int32 hdrSize = 8 * sizeof(int32);
    const int32 hdrCompSize = 2 * (sizeof(int32) + 16);
    const int32 hdrPrimGroupSize = 3 * sizeof(int32);
    const int32 vbufSize = 4 * 5 * sizeof(float32);
    const int32 ibufSize = 6 * sizeof(uint16);
    const int32 allDataSize = hdrSize + hdrCompSize + hdrPrimGroupSize + vbufSize + ibufSize;
    CHECK(stream->Size() == allDataSize);
    
    // check the generated data
    stream->Open(OpenMode::ReadOnly);
    const uint8* maxPtr = nullptr;
    const uint8* ptr = stream->MapRead(&maxPtr);
    CHECK(ptr != nullptr);
    CHECK(maxPtr == ptr + allDataSize);
    
    const uint32* i32ptr = (const uint32*) ptr;
    
    // check header
    CHECK(i32ptr[0] == 'ORAW'); // magic number
    CHECK(i32ptr[1] == 4);      // num vertices
    CHECK(i32ptr[2] == 6);      // num indices
    CHECK(i32ptr[3] == IndexType::Index16); // index type
    CHECK(i32ptr[4] == 2);      // num vertex components
    CHECK(i32ptr[5] == 1);      // num primitive groups
    CHECK(i32ptr[6] == vbufSize);   // vertices byte size
    CHECK(i32ptr[7] == ibufSize);   // indices byte size
    
    // check header vertex components
    CHECK(i32ptr[8] == VertexFormat::Float3);                               // comp0: format
    CHECK(std::strcmp((const char*)&(i32ptr[9]), "position") == 0);         // comp0: attr name
    CHECK(i32ptr[13] == VertexFormat::Float2);                              // comp1: format
    CHECK(std::strcmp((const char*)&(i32ptr[14]), "abcdefghijklmno") == 0); // comp1: attr name
    
    // check primitive group
    CHECK(i32ptr[18] == PrimitiveType::Triangles);  // prim-group 0: primitive type
    CHECK(i32ptr[19] == 0);                         // prim-group 0: baseElement
    CHECK(i32ptr[20] == 6);                         // prim-group 0: numElements
    
    // check vertices
    const float32* vPtr = (const float32*) &(i32ptr[21]);
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