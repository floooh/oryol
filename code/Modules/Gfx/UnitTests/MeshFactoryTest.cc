//------------------------------------------------------------------------------
//  MeshFactoryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Gfx/private/renderer.h"
#include "Gfx/private/gfxFactory.h"
#include "Gfx/private/resourcePools.h"
#include "Gfx/GfxTypes.h"
#include "Gfx/private/displayMgr.h"
#include "Assets/Gfx/MeshBuilder.h"

#if ORYOL_OPENGL
#include "Gfx/private/gl/gl_impl.h"
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
    class renderer renderer;
    texturePool texPool;
    meshPool meshPool;

    gfxPointers ptrs;
    ptrs.displayMgr = &displayManager;
    ptrs.renderer = &renderer;
    ptrs.texturePool = &texPool;
    ptrs.meshPool = &meshPool;

    displayManager.SetupDisplay(gfxSetup, ptrs);
    
    // setup a meshFactory object
    renderer.setup(gfxSetup, ptrs);
    gfxFactory factory;
    factory.setup(ptrs);
    
    // setup a MeshBuilder and create mesh geometry
    MeshBuilder mb;
    mb.NumVertices = 4;
    mb.NumIndices = 6;
    mb.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    mb.PrimitiveGroups.Add(0, 6);
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
        .Triangle(1, 0, 2, 3);
    auto buildResult = mb.Build();

    // setup the mesh
    mesh mesh;
    mesh.Setup = buildResult.Setup;
    const void* data = buildResult.Data.Data();
    const int size = buildResult.Data.Size();

    factory.initMesh(mesh, data, size);
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
    CHECK(mesh.primGroups[0].BaseElement == 0);
    CHECK(mesh.primGroups[0].NumElements == 6);
    #if ORYOL_OPENGL
    CHECK(mesh.buffers[mesh::vb].glBuffers[0] != 0);
    CHECK(mesh.buffers[mesh::ib].glBuffers[0] != 0);
    #endif

    factory.destroyMesh(mesh);
    CHECK(!mesh.Id.IsValid());
    CHECK(mesh.vertexBufferAttrs.NumVertices == 0);
    CHECK(mesh.vertexBufferAttrs.BufferUsage == Usage::InvalidUsage);
    CHECK(mesh.vertexBufferAttrs.Layout.NumComponents() == 0);
    CHECK(mesh.vertexBufferAttrs.Layout.ByteSize() == 0);
    CHECK(mesh.indexBufferAttrs.NumIndices == 0);
    CHECK(mesh.indexBufferAttrs.Type == IndexType::InvalidIndexType);
    CHECK(mesh.indexBufferAttrs.BufferUsage == Usage::InvalidUsage);
    CHECK(mesh.numPrimGroups == 0);
    factory.discard();
    renderer.discard();
    displayManager.DiscardDisplay();

    #endif
}
