//------------------------------------------------------------------------------
//  Clear.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

using namespace Oryol;

class ClearApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    glm::vec4 clearColor;
};
OryolMain(ClearApp);

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnRunning() {
    // render one frame
    if (Gfx::BeginFrame()) {
        
        Gfx::ApplyDefaultRenderTarget();
        Gfx::Clear(PixelChannel::RGBA, this->clearColor, 1.0f, 0);
        Gfx::EndFrame();
        
        this->clearColor += glm::vec4(0.01, 0.005, 0.0025f, 0.0);
        this->clearColor = glm::mod(this->clearColor, glm::vec4(1.0f));
    }
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(400, 300, false, "Oryol Clear Sample"));
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
