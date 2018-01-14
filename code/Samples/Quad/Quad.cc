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

    DrawState drawState;
};
OryolMain(QuadApp);

//------------------------------------------------------------------------------
AppState::Code
QuadApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(400, 400, "Oryol Quad Sample"));
    
    // create vertex buffer
    const float vertices[4 * 7] = {
        // positions            colors
        -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 0.0f, 1.0f,
    };
    this->drawState.VertexBuffers[0] = Gfx::CreateBuffer(
        MakeBufferDesc()
            .Size(sizeof(vertices))
            .Type(BufferType::VertexBuffer),
        vertices, sizeof(vertices));

    // create index buffer
    const uint16_t indices[2 * 3] = {
        0, 1, 2,    // first triangle
        0, 2, 3,    // second triangle
    };
    this->drawState.IndexBuffer = Gfx::CreateBuffer(
        MakeBufferDesc()
            .Size(sizeof(indices))
            .Type(BufferType::IndexBuffer),
        indices, sizeof(indices));

    // create shader and pipeline-state-object
    Id shd = Gfx::CreateShader(Shader::Desc());
    this->drawState.Pipeline = Gfx::CreatePipeline(
        MakePipelineDesc()
            .Shader(shd)
            .Layout(0, {
                { "in_pos", VertexFormat::Float3 },
                { "in_color", VertexFormat::Float4 }
            })
            .IndexType(IndexType::UInt16));

    return App::OnInit();
}


//------------------------------------------------------------------------------
AppState::Code
QuadApp::OnRunning() {
    
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
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
