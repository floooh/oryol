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
    
    Id pip;
    Bindings bind;
    Shader::params params;
};
OryolMain(FullscreenQuadApp);

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnInit() {
    Gfx::Setup(GfxDesc()
        .SetWidth(600)
        .SetHeight(600)
        .SetTitle("Oryol Fullscreen Quad Sample")
        .SetHtmlTrackElementSize(true));
    auto fsq = FullscreenQuadBuilder().Build();
    this->bind.VertexBuffers[0] = Gfx::CreateBuffer(fsq.VertexBufferDesc);
    this->pip = Gfx::CreatePipeline(PipelineDesc(fsq.PipelineDesc)
        .SetShader(Gfx::CreateShader(Shader::Desc())));
    this->params.time = 0.0f;
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnRunning() {
    // render one frame
    this->params.time += 1.0f / 60.0f;
    Gfx::BeginPass();
    Gfx::ApplyPipeline(this->pip);
    Gfx::ApplyBindings(this->bind);
    Gfx::ApplyUniforms(this->params);
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
