//------------------------------------------------------------------------------
//  FullscreenQuad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/Render.h"
#include "shaders.h"

using namespace Oryol;

// derived application class
class FullscreenQuadApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
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
    if (Render::BeginFrame()) {
        Render::ApplyDefaultRenderTarget();
        Render::ApplyDrawState(this->drawState);
        Render::ApplyVariable(Shaders::Main::Time, this->time);
        Render::Draw(0);
        Render::EndFrame();
    }
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnInit() {
    Render::Setup(RenderSetup::Window(600, 600, false, "Oryol Fullscreen Quad Sample"));
    Id mesh = Render::CreateResource(MeshSetup::FullScreenQuad());
    Id prog = Render::CreateResource(Shaders::Main::CreateSetup());
    this->drawState = Render::CreateResource(DrawStateSetup::FromMeshAndProg(mesh, prog));
    Render::ReleaseResource(mesh);
    Render::ReleaseResource(prog);
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnCleanup() {
    Render::ReleaseResource(this->drawState);
    Render::Discard();
    return App::OnCleanup();
}
