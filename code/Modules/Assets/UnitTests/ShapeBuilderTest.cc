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
    displayMgr displayManager;
    displayManager.SetupDisplay(GfxSetup::Window(400, 300, "Oryol Test"));
    
    // setup a meshFactory object
    meshPool meshPool;
    class renderer renderer;
    renderer.setup(&displayManager, &meshPool);
    meshFactory factory;
    factory.Setup(&renderer, &meshPool);
    
    // the state builder
    ShapeBuilder shapeBuilder;
    
    // build a simple cube
    shapeBuilder.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 1);
    shapeBuilder.Build();

    auto stream = shapeBuilder.Result().Stream;
    stream->Open(OpenMode::ReadOnly);
    const void* data = stream->MapRead(nullptr);
    const int32 size = stream->Size();

    // ...create a mesh from it and verify the mesh
    mesh simpleCube;
    simpleCube.Setup = shapeBuilder.Result().Setup;
    factory.SetupResource(simpleCube, data, size);
    CHECK(simpleCube.vertexBufferAttrs.NumVertices == 24);
    CHECK(simpleCube.vertexBufferAttrs.Layout.NumComponents() == 1);
    CHECK(simpleCube.vertexBufferAttrs.Layout.ByteSize() == 12);
    CHECK(simpleCube.vertexBufferAttrs.Layout.Component(0).IsValid());
    CHECK(simpleCube.vertexBufferAttrs.Layout.Component(0).Attr == VertexAttr::Position);
    CHECK(simpleCube.vertexBufferAttrs.Layout.Component(0).Format == VertexFormat::Float3);
    CHECK(simpleCube.vertexBufferAttrs.Layout.Component(0).ByteSize() == 12);
    CHECK(simpleCube.indexBufferAttrs.NumIndices == 36);
    CHECK(simpleCube.indexBufferAttrs.Type == IndexType::Index16);
    CHECK(simpleCube.indexBufferAttrs.BufferUsage == Usage::Immutable);
    CHECK(simpleCube.numPrimGroups == 1);
    CHECK(simpleCube.primGroups[0].PrimType == PrimitiveType::Triangles);
    CHECK(simpleCube.primGroups[0].BaseElement == 0);
    CHECK(simpleCube.primGroups[0].NumElements == 36);
    #if ORYOL_OPENGL
    CHECK(simpleCube.glVertexBuffers[0] != 0);
    CHECK(simpleCube.glIndexBuffer != 0);
    CHECK(simpleCube.glVAOs[0] != 0);
    for (uint32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        const glVertexAttr& glAttr = simpleCube.glAttrs[0][i];
        CHECK(glAttr.index == i);
        if (VertexAttr::Position == i) {
            CHECK(glAttr.enabled == GL_TRUE);
            CHECK(glAttr.size == 3);
            CHECK(glAttr.stride == 12);
            CHECK(glAttr.offset == 0);
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
    factory.DestroyResource(simpleCube);
    factory.Discard();
    renderer.discard();
    displayManager.DiscardDisplay();
    
    #endif
}
