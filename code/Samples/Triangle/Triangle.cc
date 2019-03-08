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

    Id pip;
    Bindings bind;
};
OryolMain(TriangleApp);

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxDesc()
        .SetWidth(400)
        .SetHeight(400)
        .SetTitle("Oryol Triangle Sample")
        .SetHtmlTrackElementSize(true));
    
    // create a mesh with vertex data from memory
    const float vertices[] = {
        // positions            // colors (RGBA)
         0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f , 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
    };
    this->bind.VertexBuffers[0] = Gfx::CreateBuffer(BufferDesc()
        .SetSize(sizeof(vertices))
        .SetContent(vertices));

    // create shader and pipeline-state-object
    this->pip = Gfx::CreatePipeline(PipelineDesc()
        .SetShader(Gfx::CreateShader(Shader::Desc()))
        .SetLayout(0, {
            { "position", VertexFormat::Float3 },
            { "color0", VertexFormat::Float4 }
        }));

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    
    Gfx::BeginPass();
    Gfx::ApplyPipeline(this->pip);
    Gfx::ApplyBindings(this->bind);
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
