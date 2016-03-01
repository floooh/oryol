//------------------------------------------------------------------------------
//  Clear.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
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
    ClearState clearState;
};
OryolMain(ClearApp);

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnRunning() {

    // render one frame
    Gfx::ApplyDefaultRenderTarget(this->clearState);
    Gfx::CommitFrame();
    
    this->clearState.Color += glm::vec4(0.01f, 0.005f, 0.0025f, 0.0f);
    this->clearState.Color = glm::mod(this->clearState.Color, glm::vec4(1.0f));
    // important to set alpha to 1, some platforms (e.g. RPi) always
    // compose with alpha-blending
    this->clearState.Color.w = 1.0f;

    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(400, 300, "Oryol Clear Sample"));
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
