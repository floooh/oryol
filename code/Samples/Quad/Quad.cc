//------------------------------------------------------------------------------
//  Quad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "shaders.h"

using namespace Oryol;

class QuadApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    Id pip;
    Bindings bind;
};
OryolMain(QuadApp);

//------------------------------------------------------------------------------
AppState::Code
QuadApp::OnInit() {
    Gfx::Setup(GfxDesc()
        .SetWidth(400)
        .SetHeight(400)
        .SetTitle("Oryol Quad Sample")
        .SetHtmlTrackElementSize(true));
    
    // create vertex and index buffers
    const float vertices[4 * 7] = {
        // positions            colors
        -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 0.0f, 1.0f,
    };
    const uint16_t indices[2 * 3] = {
        0, 1, 2,    // first triangle
        0, 2, 3,    // second triangle
    };
    this->bind.VertexBuffers[0] = Gfx::CreateBuffer(BufferDesc()
        .SetType(BufferType::VertexBuffer)
        .SetSize(sizeof(vertices))
        .SetContent(vertices));
    this->bind.IndexBuffer = Gfx::CreateBuffer(BufferDesc()
        .SetType(BufferType::IndexBuffer)
        .SetSize(sizeof(indices))
        .SetContent(indices));

    // create shader and pipeline-state-object
    this->pip = Gfx::CreatePipeline(PipelineDesc()
        .SetShader(Gfx::CreateShader(Shader::Desc()))
        .SetLayout(0, {
            { "in_pos", VertexFormat::Float3 },
            { "in_color", VertexFormat::Float4 }
        })
        .SetIndexType(IndexType::UInt16));

    return App::OnInit();
}


//------------------------------------------------------------------------------
AppState::Code
QuadApp::OnRunning() {
    
    Gfx::BeginPass();
    Gfx::ApplyPipeline(this->pip);
    Gfx::ApplyBindings(this->bind);
    Gfx::Draw(0, 6);
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
QuadApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
