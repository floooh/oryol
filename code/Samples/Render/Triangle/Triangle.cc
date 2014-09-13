//------------------------------------------------------------------------------
//  Triangle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/Render.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/MeshBuilder.h"
#include "shaders.h"

using namespace Oryol;

// derived application class
class TriangleApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    Id drawState;
};
OryolMain(TriangleApp);

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    
    // render one frame
    if (Render::BeginFrame()) {
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::RGBA, glm::vec4(0.0f), 1.0f, 0);
        Render::ApplyDrawState(this->drawState);
        Render::Draw(0);
        Render::EndFrame();
    }
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnInit() {
    // setup rendering system
    auto renderSetup = RenderSetup::AsWindow(400, 400, false, "Oryol Triangle Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    Render::Setup(renderSetup);
    
    // create a triangle mesh, with position and vertex color
    MeshBuilder meshBuilder;
    meshBuilder.SetNumVertices(3);
    meshBuilder.SetIndexType(IndexType::None);
    meshBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    meshBuilder.VertexLayout().Add(VertexAttr::Color0, VertexFormat::Float4);
    meshBuilder.AddPrimitiveGroup(PrimitiveType::Triangles, 0, 3);
    meshBuilder.Begin();
    // first vertex pos and color
    meshBuilder.Vertex(0, VertexAttr::Position, 0.0f, 0.5f, 0.5f);
    meshBuilder.Vertex(0, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f);
    // second vertex pos and color
    meshBuilder.Vertex(1, VertexAttr::Position, 0.5f, -0.5f, 0.5f);
    meshBuilder.Vertex(1, VertexAttr::Color0, 0.0f, 1.0f, 0.0f, 1.0f);
    // third vertex pos and color
    meshBuilder.Vertex(2, VertexAttr::Position, -0.5f, -0.5f, 0.5f);
    meshBuilder.Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 1.0f, 1.0f);
    meshBuilder.End();
    Id mesh = Render::CreateResource(MeshSetup::FromData("msh"), meshBuilder.GetStream());
    Id prog = Render::CreateResource(Shaders::Triangle::CreateSetup());
    this->drawState = Render::CreateResource(DrawStateSetup("ds", mesh, prog, 0));

    Render::ReleaseResource(mesh);
    Render::ReleaseResource(prog);
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnCleanup() {
    // cleanup everything
    Render::ReleaseResource(this->drawState);
    Render::Discard();
    return App::OnCleanup();
}
