//------------------------------------------------------------------------------
//  ShapeBuilderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Util/ShapeBuilder.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/meshFactory.h"
#include "Render/Core/meshPool.h"
#include "Render/Setup/RenderSetup.h"
#include "Render/Core/displayMgr.h"
#include "Render/Util/RawMeshLoader.h"
#include <cstring>

#if ORYOL_OPENGL
#include "Render/gl/gl_impl.h"
#endif

using namespace Oryol;
using namespace Oryol::IO;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;

//------------------------------------------------------------------------------
TEST(ShapeBuilderTest) {
    #if !ORYOL_UNITTESTS_HEADLESS
    
    // setup a GL context
    auto renderSetup = RenderSetup::Windowed(400, 300, "Oryol Test");
    displayMgr displayManager;
    displayManager.SetupDisplay(renderSetup);
    
    // setup a meshFactory object
    stateWrapper stWrapper;
    meshPool meshPool;
    meshFactory factory;
    factory.Setup(&stWrapper, &meshPool);
    factory.AttachLoader(RawMeshLoader::Create());
    
    // the state builder
    ShapeBuilder shapeBuilder;
    
    // build a simple cube
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddBox(1.0f, 1.0f, 1.0f, 1);
    shapeBuilder.Build();
    
    // ...create a mesh from it and verify the mesh
    mesh simpleCube;
    simpleCube.setSetup(MeshSetup::FromData(Locator("myCube")));
    simpleCube.setState(Resource::State::Setup);
    factory.SetupResource(simpleCube, shapeBuilder.GetStream());
    CHECK(simpleCube.GetVertexBufferAttrs().NumVertices == 24);
    CHECK(simpleCube.GetVertexBufferAttrs().Layout.NumComponents() == 1);
    CHECK(simpleCube.GetVertexBufferAttrs().Layout.ByteSize() == 12);
    CHECK(simpleCube.GetVertexBufferAttrs().Layout.Component(0).Valid());
    CHECK(simpleCube.GetVertexBufferAttrs().Layout.Component(0).Attr() == VertexAttr::Position);
    CHECK(simpleCube.GetVertexBufferAttrs().Layout.Component(0).Format() == VertexFormat::Float3);
    CHECK(simpleCube.GetVertexBufferAttrs().Layout.Component(0).ByteSize() == 12);
    CHECK(simpleCube.GetIndexBufferAttrs().NumIndices == 36);
    CHECK(simpleCube.GetIndexBufferAttrs().Type == IndexType::Index16);
    CHECK(simpleCube.GetIndexBufferAttrs().BufferUsage == Usage::Immutable);
    CHECK(simpleCube.GetNumPrimitiveGroups() == 1);
    CHECK(simpleCube.GetPrimitiveGroup(0).GetPrimitiveType() == PrimitiveType::Triangles);
    CHECK(simpleCube.GetPrimitiveGroup(0).GetBaseElement() == 0);
    CHECK(simpleCube.GetPrimitiveGroup(0).GetNumElements() == 36);
    #if ORYOL_OPENGL
    CHECK(simpleCube.glGetVertexBuffer(0) != 0);
    CHECK(simpleCube.glGetIndexBuffer() != 0);
    CHECK(simpleCube.glGetVAO(0) != 0);
    for (uint32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        const glVertexAttr& glAttr = simpleCube.glAttr(0, i);
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
    
    factory.DestroyResource(simpleCube);

    factory.Discard();
    displayManager.DiscardDisplay();
    
    #endif
}
