//------------------------------------------------------------------------------
//  Clear.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;

class ClearApp : public Core::App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    float red, green, blue;
};
OryolMain(ClearApp);

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(400, 300, "Oryol Clear Sample"));
    this->red = this->green = this->blue = 0.0f;
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        this->render->ApplyDefaultRenderTarget();
        this->render->ApplyState(Render::State::ClearColor, this->red, this->green, this->blue, 1.0f);
        this->render->Clear(true, false, false);
        this->render->EndFrame();
        if ((this->red += 0.01f) > 1.0f) this->red = 0.0f;
        if ((this->green += 0.005f) > 1.0f) this->green = 0.0f;
        if ((this->blue += 0.0025f) > 1.0f) this->blue = 0.0f;
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnCleanup() {
    // cleanup everything
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
