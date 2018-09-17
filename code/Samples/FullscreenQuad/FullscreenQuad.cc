//------------------------------------------------------------------------------
//  FullscreenQuad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/FullscreenQuadBuilder.h"
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
    Gfx::Setup(GfxDesc()
        .Width(600).Height(600)
        .Title("Oryol Fullscreen Quad Sample")
        .HtmlTrackElementSize(true));
    auto fsq = FullscreenQuadBuilder().Build();
    this->drawState.VertexBuffers[0] = Gfx::CreateBuffer(fsq.VertexBufferDesc);
    this->drawState.Pipeline = Gfx::CreatePipeline(PipelineDesc(fsq.PipelineDesc)
        .Shader(Gfx::CreateShader(Shader::Desc())));
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
