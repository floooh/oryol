//------------------------------------------------------------------------------
//  FullscreenQuad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;

// derived application class
class FullscreenQuadApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    Id drawState;
    float time = 0.0f;
};
OryolMain(FullscreenQuadApp);

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(600, 600, "Oryol Fullscreen Quad Sample"));
    Id mesh = this->render->CreateResource(MeshSetup::CreateFullScreenQuad("msh"));
    Id prog = this->render->CreateResource(Shaders::Main::CreateSetup());
    this->drawState = this->render->CreateResource(DrawStateSetup("ds", mesh, prog, 0));
    this->render->ReleaseResource(mesh);
    this->render->ReleaseResource(prog);
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnRunning() {
    // render one frame
    this->time += 1.0f / 60.0f;
    if (this->render->BeginFrame()) {
        this->render->ApplyDefaultRenderTarget();
        this->render->ApplyDrawState(this->drawState);
        this->render->ApplyVariable(Shaders::Main::Time, this->time);
        this->render->Draw(0);
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnCleanup() {
    this->render->ReleaseResource(this->drawState);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
