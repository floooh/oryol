//------------------------------------------------------------------------------
//  MeshFactoryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/meshFactory.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/displayMgr.h"
#include "Assets/Gfx/MeshBuilder.h"

#if ORYOL_OPENGL
#include "Gfx/gl/gl_impl.h"
#endif

using namespace Oryol;
using namespace _priv;

// NOTE: this is should not be treated as sample code on how
// to initialize a mesh!
TEST(MeshFactoryTest) {
    
    #if !ORYOL_UNITTESTS_HEADLESS
    // setup a GL context
    auto gfxSetup = GfxSetup::Window(400, 300, "Oryol Test");
    displayMgr displayManager;
    displayManager.SetupDisplay(gfxSetup);
    
    // setup a meshFactory object
    meshPool meshPool;
    texturePool texPool;
    class renderer renderer;
    renderer.setup(&displayManager, &meshPool, &texPool);
    meshFactory factory;
    factory.Setup(&renderer, &meshPool);
    
    // setup a MeshBuilder and create mesh geometry
    MeshBuilder mb;
    mb.NumVertices = 4;
    mb.NumIndices = 6;
    mb.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    mb.PrimitiveGroups.Add(PrimitiveType::Triangles, 0, 6);
    mb.Begin()
        .Vertex(0, VertexAttr::Position, 0.0f, 0.0f, 0.0f)  // top-left
        .Vertex(1, VertexAttr::Position, 1.0f, 0.0f, 0.0f)  // top-right
        .Vertex(2, VertexAttr::Position, 1.0f, 1.0f, 0.0f)  // bottom-right
        .Vertex(3, VertexAttr::Position, 0.0f, 1.0f, 0.0f)  // bottom-left
        .Vertex(0, VertexAttr::TexCoord0, 0.0f, 0.0f)
        .Vertex(1, VertexAttr::TexCoord0, 1.0f, 0.0f)
        .Vertex(2, VertexAttr::TexCoord0, 1.0f, 1.0f)
        .Vertex(3, VertexAttr::TexCoord0, 0.0f, 1.0f)
        .Triangle(0, 0, 1, 2)
        .Triangle(1, 0, 2, 3)
        .End();
    
    // setup the mesh
    mesh mesh;
    mesh.Setup = mb.Result().Setup;
    auto stream = mb.Result().Stream;
    stream->Open(OpenMode::ReadOnly);
    const void* data = stream->MapRead(nullptr);
    const int32 size = stream->Size();

    factory.SetupResource(mesh, data, size);
    CHECK(!mesh.Id.IsValid());
    CHECK(mesh.Setup.Locator == Locator::NonShared());
    CHECK(mesh.vertexBufferAttrs.NumVertices == 4);
    CHECK(mesh.vertexBufferAttrs.BufferUsage == Usage::Immutable);
    CHECK(mesh.vertexBufferAttrs.Layout.NumComponents() == 2);
    CHECK(mesh.vertexBufferAttrs.Layout.ByteSize() == 20);
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentByteOffset(0) == 0);
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentByteOffset(1) == 12);
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentAt(0).IsValid());
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentAt(0).Attr == VertexAttr::Position);
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentAt(0).Format == VertexFormat::Float3);
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentAt(0).ByteSize() == 12);
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentAt(1).IsValid());
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentAt(1).Attr == VertexAttr::TexCoord0);
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentAt(1).Format == VertexFormat::Float2);
    CHECK(mesh.vertexBufferAttrs.Layout.ComponentAt(1).ByteSize() == 8);
    CHECK(mesh.indexBufferAttrs.NumIndices == 6);
    CHECK(mesh.indexBufferAttrs.Type == IndexType::Index16);
    CHECK(mesh.indexBufferAttrs.BufferUsage == Usage::Immutable);
    CHECK(mesh.indexBufferAttrs.ByteSize() == 12);
    CHECK(mesh.numPrimGroups == 1);
    CHECK(mesh.primGroups[0].PrimType == PrimitiveType::Triangles);
    CHECK(mesh.primGroups[0].BaseElement == 0);
    CHECK(mesh.primGroups[0].NumElements == 6);
    #if ORYOL_OPENGL
    CHECK(mesh.glVertexBuffers[0] != 0);
    CHECK(mesh.glIndexBuffer != 0);
    CHECK(mesh.glVAOs[0] != 0);
    for (uint32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        const glVertexAttr& glAttr = mesh.glAttrs[0][i];
        CHECK(glAttr.index == i);
        if (VertexAttr::Position == i) {
            CHECK(glAttr.enabled == GL_TRUE);
            CHECK(glAttr.size == 3);
            CHECK(glAttr.stride == 20);
            CHECK(glAttr.offset == 0);
            CHECK(glAttr.type == GL_FLOAT);
            CHECK(glAttr.normalized == GL_FALSE);
        }
        else if (VertexAttr::TexCoord0 == i) {
            CHECK(glAttr.enabled == GL_TRUE);
            CHECK(glAttr.size == 2);
            CHECK(glAttr.stride == 20);
            CHECK(glAttr.offset == 12);
            CHECK(glAttr.type == GL_FLOAT);
            CHECK(glAttr.normalized == GL_FALSE);
        }
        else {
            CHECK(glAttr.enabled == GL_FALSE);
            CHECK(glAttr.size == 0);
            CHECK(glAttr.stride == 0);
            CHECK(glAttr.offset == 0);
            CHECK(glAttr.type == 0);
            CHECK(glAttr.normalized == GL_FALSE);
        }
    }
    #endif

    stream->UnmapRead();
    stream->Close();
    factory.DestroyResource(mesh);
    CHECK(!mesh.Id.IsValid());
    CHECK(mesh.vertexBufferAttrs.NumVertices == 0);
    CHECK(mesh.vertexBufferAttrs.BufferUsage == Usage::InvalidUsage);
    CHECK(mesh.vertexBufferAttrs.Layout.NumComponents() == 0);
    CHECK(mesh.vertexBufferAttrs.Layout.ByteSize() == 0);
    CHECK(mesh.indexBufferAttrs.NumIndices == 0);
    CHECK(mesh.indexBufferAttrs.Type == IndexType::InvalidIndexType);
    CHECK(mesh.indexBufferAttrs.BufferUsage == Usage::InvalidUsage);
    CHECK(mesh.numPrimGroups == 0);
    factory.Discard();
    displayManager.DiscardDisplay();
    renderer.discard();
    
    #endif
}