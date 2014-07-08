//------------------------------------------------------------------------------
//  MeshFactoryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/meshFactory.h"
#include "Render/Core/meshPool.h"
#include "Render/Setup/RenderSetup.h"
#include "Render/Core/displayMgr.h"
#include "Render/Util/MeshBuilder.h"
#include "Render/Util/RawMeshLoader.h"

#if ORYOL_OPENGL
#include "Render/gl/gl_impl.h"
#endif

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;
using namespace Oryol::Render;
using namespace Oryol::Resource;

// NOTE: this is should not be treated as sample code on how
// to initialize a mesh!
TEST(MeshFactoryTest) {
    
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
    
    // setup a MeshBuilder and create mesh geometry
    MeshBuilder mb;
    mb.SetNumVertices(4);
    mb.SetNumIndices(6);
    mb.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    mb.AddComponent(VertexAttr::TexCoord0, VertexFormat::Float2);
    mb.AddPrimitiveGroup(PrimitiveType::Triangles, 0, 6);
    mb.Begin();
    mb.Vertex(0, VertexAttr::Position, 0.0f, 0.0f, 0.0f);  // top-left
    mb.Vertex(1, VertexAttr::Position, 1.0f, 0.0f, 0.0f);  // top-right
    mb.Vertex(2, VertexAttr::Position, 1.0f, 1.0f, 0.0f);  // bottom-right
    mb.Vertex(3, VertexAttr::Position, 0.0f, 1.0f, 0.0f);  // bottom-left
    mb.Vertex(0, VertexAttr::TexCoord0, 0.0f, 0.0f);
    mb.Vertex(1, VertexAttr::TexCoord0, 1.0f, 0.0f);
    mb.Vertex(2, VertexAttr::TexCoord0, 1.0f, 1.0f);
    mb.Vertex(3, VertexAttr::TexCoord0, 0.0f, 1.0f);
    mb.Triangle(0, 0, 1, 2);
    mb.Triangle(1, 0, 2, 3);
    mb.End();
    
    // setup the mesh
    const Ptr<Stream>& meshData = mb.GetStream();
    mesh mesh;
    mesh.setSetup(MeshSetup::FromData(Locator("myQuad")));
    
    factory.SetupResource(mesh, meshData);
    CHECK(mesh.GetState() == Resource::State::Valid);
    CHECK(!mesh.GetId().IsValid());
    CHECK(mesh.GetSetup().GetLocator().Location() == "myQuad");
    CHECK(mesh.GetVertexBufferAttrs().GetNumVertices() == 4);
    CHECK(mesh.GetVertexBufferAttrs().GetUsage() == Usage::Immutable);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetNumComponents() == 2);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetByteSize() == 20);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponentByteOffset(0) == 0);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponentByteOffset(1) == 12);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponent(0).IsValid());
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponent(0).GetAttr() == VertexAttr::Position);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponent(0).GetFormat() == VertexFormat::Float3);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponent(0).GetByteSize() == 12);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponent(1).IsValid());
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponent(1).GetAttr() == VertexAttr::TexCoord0);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponent(1).GetFormat() == VertexFormat::Float2);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetComponent(1).GetByteSize() == 8);
    CHECK(mesh.GetIndexBufferAttrs().GetNumIndices() == 6);
    CHECK(mesh.GetIndexBufferAttrs().GetIndexType() == IndexType::Index16);
    CHECK(mesh.GetIndexBufferAttrs().GetUsage() == Usage::Immutable);
    CHECK(mesh.GetIndexBufferAttrs().GetByteSize() == 12);
    CHECK(mesh.GetNumPrimitiveGroups() == 1);
    CHECK(mesh.GetPrimitiveGroup(0).GetPrimitiveType() == PrimitiveType::Triangles);
    CHECK(mesh.GetPrimitiveGroup(0).GetBaseElement() == 0);
    CHECK(mesh.GetPrimitiveGroup(0).GetNumElements() == 6);
    #if ORYOL_OPENGL
    CHECK(mesh.glGetVertexBuffer() != 0);
    CHECK(mesh.glGetIndexBuffer() != 0);
    CHECK(mesh.glGetVertexArrayObject() != 0);
    for (uint32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        const glVertexAttr& glAttr = mesh.glAttr(i);
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
    
    factory.DestroyResource(mesh);
    CHECK(mesh.GetState() == Resource::State::Setup);
    CHECK(!mesh.GetId().IsValid());
    CHECK(mesh.GetVertexBufferAttrs().GetNumVertices() == 0);
    CHECK(mesh.GetVertexBufferAttrs().GetUsage() == Usage::InvalidUsage);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetNumComponents() == 0);
    CHECK(mesh.GetVertexBufferAttrs().GetVertexLayout().GetByteSize() == 0);
    CHECK(mesh.GetIndexBufferAttrs().GetNumIndices() == 0);
    CHECK(mesh.GetIndexBufferAttrs().GetIndexType() == IndexType::InvalidIndexType);
    CHECK(mesh.GetIndexBufferAttrs().GetUsage() == Usage::InvalidUsage);
    CHECK(mesh.GetNumPrimitiveGroups() == 0);
    factory.Discard();
    displayManager.DiscardDisplay();
    
    #endif
}