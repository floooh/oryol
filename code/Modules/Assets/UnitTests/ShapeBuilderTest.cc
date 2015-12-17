//------------------------------------------------------------------------------
//  ShapeBuilderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/meshFactory.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/displayMgr.h"
#include <cstring>

#if ORYOL_OPENGL
#include "Gfx/gl/gl_impl.h"
#endif

using namespace Oryol;
using namespace _priv;

//------------------------------------------------------------------------------
TEST(ShapeBuilderTest) {
    #if !ORYOL_UNITTESTS_HEADLESS
    
    // setup a GL context
    auto gfxSetup = GfxSetup::Window(400, 300, "Oryol Test");

    displayMgr displayManager;
    meshPool meshPool;
    texturePool texPool;
    class renderer renderer;

    gfxPointers ptrs;
    ptrs.displayMgr = &displayManager;
    ptrs.renderer = &renderer;
    ptrs.meshPool = &meshPool;
    ptrs.texturePool = &texPool;
    displayManager.SetupDisplay(gfxSetup, ptrs);
    
    // setup a meshFactory object
    renderer.setup(gfxSetup, ptrs);
    meshFactory factory;
    factory.Setup(ptrs);
    
    // the state builder
    ShapeBuilder shapeBuilder;
    
    // build a simple cube
    shapeBuilder.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 1);
    auto buildResult = shapeBuilder.Build();

    const void* data = buildResult.Data.Data();
    const int32 size = buildResult.Data.Size();

    // ...create a mesh from it and verify the mesh
    mesh simpleCube;
    simpleCube.Setup = buildResult.Setup;
    factory.SetupResource(simpleCube, data, size);
    CHECK(simpleCube.vertexBufferAttrs.NumVertices == 24);
    CHECK(simpleCube.vertexBufferAttrs.Layout.NumComponents() == 1);
    CHECK(simpleCube.vertexBufferAttrs.Layout.ByteSize() == 12);
    CHECK(simpleCube.vertexBufferAttrs.Layout.ComponentAt(0).IsValid());
    CHECK(simpleCube.vertexBufferAttrs.Layout.ComponentAt(0).Attr == VertexAttr::Position);
    CHECK(simpleCube.vertexBufferAttrs.Layout.ComponentAt(0).Format == VertexFormat::Float3);
    CHECK(simpleCube.vertexBufferAttrs.Layout.ComponentAt(0).ByteSize() == 12);
    CHECK(simpleCube.indexBufferAttrs.NumIndices == 36);
    CHECK(simpleCube.indexBufferAttrs.Type == IndexType::Index16);
    CHECK(simpleCube.indexBufferAttrs.BufferUsage == Usage::Immutable);
    CHECK(simpleCube.numPrimGroups == 1);
    CHECK(simpleCube.Setup.PrimType == PrimitiveType::Triangles);
    CHECK(simpleCube.primGroups[0].BaseElement == 0);
    CHECK(simpleCube.primGroups[0].NumElements == 36);
    #if ORYOL_OPENGL
    CHECK(simpleCube.buffers[mesh::vb].glBuffers[0] != 0);
    CHECK(simpleCube.buffers[mesh::ib].glBuffers[0] != 0);
    #endif

    factory.DestroyResource(simpleCube);
    factory.Discard();
    renderer.discard();
    displayManager.DiscardDisplay();
    
    #endif
}
