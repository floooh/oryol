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
    auto bufSetup = BufferSetup::Make(sizeof(vertices));
    this->drawState.VertexBuffers[0] = Gfx::CreateResource(bufSetup, vertices, sizeof(vertices));

    // create shader and pipeline-state-object
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto pipSetup = PipelineSetup::FromShaderAndLayout(shd, {
        { "position", VertexFormat::Float3 },
        { "color0", VertexFormat::Float4 }
    });
    this->drawState.Pipeline = Gfx::CreateResource(pipSetup);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::Draw(0, 3);
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
