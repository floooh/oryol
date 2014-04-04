//------------------------------------------------------------------------------
//  ShapeBuilderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Util/ShapeBuilder.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/meshFactory.h"
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
    
    // setup a GL context
    auto renderSetup = RenderSetup::Windowed(400, 300, "Oryol Test");
    displayMgr displayManager;
    displayManager.SetupDisplay(renderSetup);
    
    // setup a meshFactory object
    stateWrapper stWrapper;
    meshFactory factory;
    factory.Setup(&stWrapper);
    factory.AttachLoader(RawMeshLoader::Create());
    
    // the state builder
    ShapeBuilder shapeBuilder;
    
    // build a simple cube
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddBox(1.0f, 1.0f, 1.0f, 1);
    shapeBuilder.Build();
    
    // ...create a mesh from it and verify the mesh
    mesh simpleCube;
    simpleCube.setSetup(MeshSetup::FromData(Locator("myCube")));
    simpleCube.setState(Resource::State::Setup);
    factory.SetupResource(simpleCube, shapeBuilder.GetStream());
    CHECK(simpleCube.GetVertexBufferAttrs().GetNumVertices() == 24);
    CHECK(simpleCube.GetVertexBufferAttrs().GetVertexLayout().GetNumComponents() == 1);
    CHECK(simpleCube.GetVertexBufferAttrs().GetVertexLayout().GetByteSize() == 12);
    CHECK(simpleCube.GetVertexBufferAttrs().GetVertexLayout().GetComponent(0).IsValid());
    CHECK(simpleCube.GetVertexBufferAttrs().GetVertexLayout().GetComponent(0).GetAttr() == VertexAttr::Position);
    CHECK(simpleCube.GetVertexBufferAttrs().GetVertexLayout().GetComponent(0).GetFormat() == VertexFormat::Float3);
    CHECK(simpleCube.GetVertexBufferAttrs().GetVertexLayout().GetComponent(0).GetByteSize() == 12);
    CHECK(simpleCube.GetIndexBufferAttrs().GetNumIndices() == 36);
    CHECK(simpleCube.GetIndexBufferAttrs().GetIndexType() == IndexType::Index16);
    CHECK(simpleCube.GetIndexBufferAttrs().GetUsage() == Usage::Immutable);
    CHECK(simpleCube.GetNumPrimitiveGroups() == 1);
    CHECK(simpleCube.GetPrimitiveGroup(0).GetPrimitiveType() == PrimitiveType::Triangles);
    CHECK(simpleCube.GetPrimitiveGroup(0).GetBaseElement() == 0);
    CHECK(simpleCube.GetPrimitiveGroup(0).GetNumElements() == 36);
    #if ORYOL_OPENGL
    CHECK(simpleCube.glGetVertexBuffer() != 0);
    CHECK(simpleCube.glGetIndexBuffer() != 0);
    CHECK(simpleCube.glGetVertexArrayObject() != 0);
    for (uint32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        const glVertexAttr& glAttr = simpleCube.glAttr(i);
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
}
