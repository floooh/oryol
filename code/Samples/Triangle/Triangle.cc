//------------------------------------------------------------------------------
//  Triangle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
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
    Id pipeline;
    MeshBlock meshBlock;
};
OryolMain(TriangleApp);

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->pipeline, this->meshBlock);
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
    meshBuilder.PrimitiveGroups.Add(0, 3);
    meshBuilder.Begin()
        .Vertex(0, VertexAttr::Position, 0.0f, 0.5f, 0.5f)
        .Vertex(0, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f)
        .Vertex(1, VertexAttr::Position, 0.5f, -0.5f, 0.5f)
        .Vertex(1, VertexAttr::Color0, 0.0f, 1.0f, 0.0f, 1.0f)
        .Vertex(2, VertexAttr::Position, -0.5f, -0.5f, 0.5f)
        .Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 1.0f, 1.0f);
    this->meshBlock[0] =  Gfx::CreateResource(meshBuilder.Build());
    Id shd = Gfx::CreateResource(Shaders::Triangle::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(meshBuilder.Layout, shd);
    this->pipeline = Gfx::CreateResource(ps);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
