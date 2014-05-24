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

// derived application class
class FullscreenQuadApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    Resource::Id progId;
    float time = 0.0f;
};
OryolMain(FullscreenQuadApp);

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(600, 600, "Oryol Fullscreen Quad Sample"));
    this->progId = this->render->CreateResource(Shaders::Main::CreateSetup());
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnRunning() {
    // render one frame
    this->time += 1.0f / 60.0f;
    if (this->render->BeginFrame()) {
        this->render->ApplyProgram(this->progId, 0);
        this->render->ApplyVariable(Shaders::Main::Time, this->time);
        this->render->DrawFullscreenQuad();
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
FullscreenQuadApp::OnCleanup() {
    this->render->DiscardResource(this->progId);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
