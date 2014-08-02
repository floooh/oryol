//------------------------------------------------------------------------------
//  TestInput.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Debug/DebugFacade.h"
#include "Input/InputFacade.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Debug;
using namespace Oryol::Input;

// derived application class
class TestInputApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    DebugFacade* debug;
    InputFacade* input;
};
OryolMain(TestInputApp);

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(800, 600, "Oryol Input Test Sample"));
    this->debug  = DebugFacade::CreateSingle();
    this->input  = InputFacade::CreateSingle();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
    
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::RGBA, glm::vec4(0.5f), 1.0f, 0);
        this->debug->DrawTextBuffer();
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnCleanup() {
    this->input  = nullptr;
    this->render = nullptr;
    this->debug  = nullptr;
    InputFacade::DestroySingle();
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
