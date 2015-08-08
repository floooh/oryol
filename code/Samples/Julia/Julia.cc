//------------------------------------------------------------------------------
//  Julia.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

class JuliaApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();
private:
    ClearState displayClearState;
};
OryolMain(JuliaApp)

//------------------------------------------------------------------------------
AppState::Code
JuliaApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(800, 600, "Julia");
    Gfx::Setup(gfxSetup);

    this->displayClearState.Color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
JuliaApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget(this->displayClearState);
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
JuliaApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

