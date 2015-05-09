//------------------------------------------------------------------------------
//  FullscreenQuad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
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
    float time = 0.0f;
};
OryolMain(FullscreenQuadApp);

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnRunning() {
    // render one frame
    this->time += 1.0f / 60.0f;
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyVariable(Shaders::Main::Time, this->time);
    Gfx::Draw(0);
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(600, 600, "Oryol Fullscreen Quad Sample"));
    Id mesh = Gfx::CreateResource(MeshSetup::FullScreenQuad());
    Id prog = Gfx::CreateResource(Shaders::Main::CreateSetup());
    this->drawState = Gfx::CreateResource(DrawStateSetup::FromMeshAndProg(mesh, prog));
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
