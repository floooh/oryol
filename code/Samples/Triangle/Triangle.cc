//------------------------------------------------------------------------------
//  Triangle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/MeshBuilder.h"
#include "shaders.h"

using namespace Oryol;

class TriangleApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
private:
    Id drawState;
};
OryolMain(TriangleApp);

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::Draw(0);
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxSetup::Window(400, 400, "Oryol Triangle Sample"));
    
    // create triangle mesh with 3 vertices, with position and vertex color
    MeshBuilder meshBuilder;
    meshBuilder.NumVertices = 3;
    meshBuilder.IndicesType = IndexType::None;
    meshBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Color0, VertexFormat::Float4);
    meshBuilder.PrimitiveGroups.Add(PrimitiveType::Triangles, 0, 3);
    meshBuilder.Begin()
        .Vertex(0, VertexAttr::Position, 0.0f, 0.5f, 0.5f)
        .Vertex(0, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f)
        .Vertex(1, VertexAttr::Position, 0.5f, -0.5f, 0.5f)
        .Vertex(1, VertexAttr::Color0, 0.0f, 1.0f, 0.0f, 1.0f)
        .Vertex(2, VertexAttr::Position, -0.5f, -0.5f, 0.5f)
        .Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 1.0f, 1.0f)
        .End();
    Id mesh = Gfx::CreateResource(meshBuilder.Result());
    Id shd = Gfx::CreateResource(Shaders::Triangle::CreateSetup());
    this->drawState = Gfx::CreateResource(DrawStateSetup::FromMeshAndShader(mesh, shd));

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
