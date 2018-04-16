//------------------------------------------------------------------------------
//  FullscreenQuad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "shaders.h"

using namespace Oryol;

class FullscreenQuadApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
    DrawState drawState;
    Shader::params params;
};
OryolMain(FullscreenQuadApp);

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnInit() {
    Gfx::Setup(GfxDesc().Width(600).Height(600).Title("Oryol Fullscreen Quad Sample"));
    const float quadVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    this->drawState.VertexBuffers[0] = Gfx::CreateBuffer(NewBufferDesc()
        .Size(sizeof(quadVertices))
        .Content(quadVertices)
        .Done());
    this->drawState.Pipeline = Gfx::CreatePipeline(NewPipelineDesc()
        .Shader(Gfx::CreateShader(Shader::Desc()))
        .Layout(0, { { "in_pos", VertexFormat::Float2 } })
        .PrimitiveType(PrimitiveType::TriangleStrip)
        .Done());
    this->params.time = 0.0f;
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnRunning() {
    // render one frame
    this->params.time += 1.0f / 60.0f;
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->params);
    Gfx::Draw(0, 4);
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
