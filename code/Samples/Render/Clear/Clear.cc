//------------------------------------------------------------------------------
//  Clear.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Application/App.h"
#include "Render/RenderFacade.h"

OryolApp("Clear", "1.0");

using namespace Oryol;
using namespace Oryol::Application;
using namespace Oryol::Render;
using namespace Oryol::Core;

class ClearApp : public Application::App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* renderFacade;
    float red, green, blue;
};

//------------------------------------------------------------------------------
void
OryolMain() {
    ClearApp app;
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnInit() {
    // setup rendering system
    this->renderFacade = RenderFacade::CreateSingleton();
    this->renderFacade->Setup(RenderSetup::Windowed(400, 300, "Oryol Clear Sample"));
    this->red = this->green = this->blue = 0.0f;
    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnRunning() {
    if (this->renderFacade->BeginFrame()) {
        this->renderFacade->ApplyState(Render::State::ClearColor, this->red, this->green, this->blue, 1.0f);
        this->renderFacade->Clear(true, false, false);
        this->renderFacade->EndFrame();
        if ((this->red += 0.01f) > 1.0f) this->red = 0.0f;
        if ((this->green += 0.005f) > 1.0f) this->green = 0.0f;
        if ((this->blue += 0.0025f) > 1.0f) this->blue = 0.0f;
    }
    return renderFacade->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnCleanup() {
    this->renderFacade->Discard();
    this->renderFacade = nullptr;
    RenderFacade::DestroySingleton();
    return AppState::Destroy;
}
