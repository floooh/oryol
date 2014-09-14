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
    auto renderSetup = RenderSetup::Window(400, 400, false, "Oryol Triangle Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    Render::Setup(renderSetup);
    
    // create triangle mesh with 3 vertices, with position and vertex color
    MeshBuilder meshBuilder;
    meshBuilder.NumVertices = 3;
    meshBuilder.IndicesType = IndexType::None;
    meshBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Color0, VertexFormat::Float4);
    meshBuilder.AddPrimitiveGroup(PrimitiveType::Triangles, 0, 3);
    meshBuilder.Begin()
        .Vertex(0, VertexAttr::Position, 0.0f, 0.5f, 0.5f)
        .Vertex(0, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f)
        .Vertex(1, VertexAttr::Position, 0.5f, -0.5f, 0.5f)
        .Vertex(1, VertexAttr::Color0, 0.0f, 1.0f, 0.0f, 1.0f)
        .Vertex(2, VertexAttr::Position, -0.5f, -0.5f, 0.5f)
        .Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 1.0f, 1.0f)
        .End();
    Id mesh = Render::CreateResource(MeshSetup::FromStream(), meshBuilder.GetStream());
    Id prog = Render::CreateResource(Shaders::Triangle::CreateSetup());
    this->drawState = Render::CreateResource(DrawStateSetup::FromMeshAndProg(mesh, prog));

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
