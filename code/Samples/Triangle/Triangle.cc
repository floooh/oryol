//------------------------------------------------------------------------------
//  Triangle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "shaders.h"

using namespace Oryol;

class TriangleApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    DrawState drawState;
};
OryolMain(TriangleApp);

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxSetup::Window(400, 400, "Oryol Triangle Sample"));
    
    // create a mesh with vertex data from memory
    const float vertices[] = {
        // positions            // colors (RGBA)
         0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f , 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
    };
    auto meshSetup = MeshSetup::FromData();
    meshSetup.NumVertices = 3;
    meshSetup.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Color0, VertexFormat::Float4 }
    };
    meshSetup.AddPrimitiveGroup({0, 3});
    this->drawState.Mesh[0] = Gfx::CreateResource(meshSetup, vertices, sizeof(vertices));

    // create shader and pipeline-state-object
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(meshSetup.Layout, shd);
    this->drawState.Pipeline = Gfx::CreateResource(ps);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::Draw();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
