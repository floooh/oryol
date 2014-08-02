//------------------------------------------------------------------------------
//  Clear.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

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
ClearApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::RGBA, this->clearColor, 1.0f, 0);
        this->render->EndFrame();
        
        this->clearColor += glm::vec4(0.01, 0.005, 0.0025f, 0.0);
        this->clearColor = glm::mod(this->clearColor, glm::vec4(1.0f));
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(400, 300, "Oryol Clear Sample"));
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnCleanup() {
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
