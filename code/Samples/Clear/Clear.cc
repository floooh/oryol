//------------------------------------------------------------------------------
//  Clear.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "glm/glm.hpp"

using namespace Oryol;

class ClearApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
    PassAction passAction;
};
OryolMain(ClearApp);

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(400, 300, "Oryol Clear Sample"));
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnRunning() {

    // render one frame
    Gfx::BeginPass(this->passAction);
    Gfx::EndPass();
    Gfx::CommitFrame();

    // update the clear color for next frame
    this->passAction.Color[0] += glm::vec4(0.01f, 0.005f, 0.0025f, 0.0f);
    this->passAction.Color[0] = glm::mod(this->passAction.Color[0], glm::vec4(1.0f));
    this->passAction.Color[0].w = 1.0f;

    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
