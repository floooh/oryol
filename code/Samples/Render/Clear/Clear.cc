//------------------------------------------------------------------------------
//  Clear.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/Render.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

using namespace Oryol;

class ClearApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::vec4 clearColor;
};
OryolMain(ClearApp);

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnRunning() {
    // render one frame
    if (Render::BeginFrame()) {
        
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::RGBA, this->clearColor, 1.0f, 0);
        Render::EndFrame();
        
        this->clearColor += glm::vec4(0.01, 0.005, 0.0025f, 0.0);
        this->clearColor = glm::mod(this->clearColor, glm::vec4(1.0f));
    }
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnInit() {
    Render::Setup(RenderSetup::Window(400, 300, false, "Oryol Clear Sample"));
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnCleanup() {
    Render::Discard();
    return App::OnCleanup();
}
