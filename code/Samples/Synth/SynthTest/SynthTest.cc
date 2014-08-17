//------------------------------------------------------------------------------
//  SynthTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Synth/SynthFacade.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Synth;

// derived application class
class SynthTestApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    SynthFacade* synth;
};
OryolMain(SynthTestApp);

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(640, 400, "Oryol Synth Test Sample"));
    this->synth  = SynthFacade::CreateSingle(SynthSetup());
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnRunning() {

    this->synth->Update();
    if (this->render->BeginFrame()) {
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::RGBA, glm::vec4(0.5f), 1.0f, 0);
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnCleanup() {
    this->synth  = nullptr;
    this->render = nullptr;
    SynthFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
