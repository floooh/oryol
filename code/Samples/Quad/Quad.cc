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
    auto vbufSetup = BufferSetup::Make(sizeof(vertices), BufferType::VertexBuffer);
    this->drawState.VertexBuffers[0] = Gfx::CreateResource(vbufSetup, vertices, sizeof(vertices));

    // create index buffer
    const uint16_t indices[2 * 3] = {
        0, 1, 2,    // first triangle
        0, 2, 3,    // second triangle
    };
    auto ibufSetup = BufferSetup::Make(sizeof(indices), BufferType::IndexBuffer);
    this->drawState.IndexBuffer = Gfx::CreateResource(ibufSetup, indices, sizeof(indices));

    // create shader and pipeline-state-object
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto ps = PipelineSetup::FromShaderAndLayout(shd, {
        { "in_pos", VertexFormat::Float3 },
        { "in_color", VertexFormat::Float4 }
    });
    ps.IndexType = IndexType::UInt16;
    this->drawState.Pipeline = Gfx::CreateResource(ps);

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
