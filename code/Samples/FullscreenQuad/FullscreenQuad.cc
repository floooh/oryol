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
    Id drawState;
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
    Gfx::ApplyDrawState(this->drawState, this->meshBlock);
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
    auto dss = DrawStateSetup::FromLayoutAndShader(quadSetup.Layout, shd);
    this->drawState = Gfx::CreateResource(dss);
    this->params.Time = 0.0f;
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
