//------------------------------------------------------------------------------
//  Triangle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Gfx/Util/RawMeshLoader.h"
#include "Gfx/Util/MeshBuilder.h"
#include "shaders.h"

using namespace Oryol;

class TriangleApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
private:
    GfxId drawState;
};
OryolMain(TriangleApp);

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    
    // render one frame
    if (Gfx::BeginFrame()) {
        Gfx::ApplyDefaultRenderTarget();
        Gfx::Clear(PixelChannel::RGBA, glm::vec4(0.0f), 1.0f, 0);
        Gfx::ApplyDrawState(this->drawState);
        Gfx::Draw(0);
        Gfx::EndFrame();
    }
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnInit() {
    // setup rendering system
    auto gfxSetup = GfxSetup::Window(400, 400, false, "Oryol Triangle Sample");
    gfxSetup.Loaders.Add(RawMeshLoader::Creator());
    Gfx::Setup(gfxSetup);
    
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
    GfxId mesh = Gfx::CreateResource(MeshSetup::FromStream(), meshBuilder.GetStream());
    GfxId prog = Gfx::CreateResource(Shaders::Triangle::CreateSetup());
    this->drawState = Gfx::CreateResource(DrawStateSetup::FromMeshAndProg(mesh, prog));

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnCleanup() {
    this->drawState.Release();
    Gfx::Discard();
    return App::OnCleanup();
}
