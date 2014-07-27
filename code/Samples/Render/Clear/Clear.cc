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
    glm::vec4 clearColor;
};
OryolMain(ClearApp);

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(400, 300, "Oryol Clear Sample"));
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::RGBA, this->clearColor, 1.0f, 0);
        this->render->EndFrame();
        
        this->clearColor += glm::vec4(0.01, 0.005, 0.0025f, 0.0);
        if (this->clearColor.x > 1.0f) {
            this->clearColor.x = 0.0f;
        }
        if (this->clearColor.y > 1.0f) {
            this->clearColor.y = 0.0f;
        }
        if (this->clearColor.z > 1.0f) {
            this->clearColor.z = 0.0f;
        }
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
