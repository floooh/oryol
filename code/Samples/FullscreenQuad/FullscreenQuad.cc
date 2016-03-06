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
    
private:
    Id pipeline;
    MeshBlock meshBlock;
    Shaders::Main::Params params;

};
OryolMain(FullscreenQuadApp);

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnRunning() {
    // render one frame
    this->params.Time += 1.0f / 60.0f;
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->pipeline, this->meshBlock);
    Gfx::ApplyUniformBlock(this->params);
    Gfx::Draw(0);
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(600, 600, "Oryol Fullscreen Quad Sample"));
    auto quadSetup = MeshSetup::FullScreenQuad();
    this->meshBlock[0] = Gfx::CreateResource(quadSetup);
    Id shd = Gfx::CreateResource(Shaders::Main::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(quadSetup.Layout, shd);
    this->pipeline = Gfx::CreateResource(ps);
    this->params.Time = 0.0f;
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
