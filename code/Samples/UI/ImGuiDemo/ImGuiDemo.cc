//------------------------------------------------------------------------------
//  ImGuiDemo.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "IMUI/IMUI.h"

using namespace Oryol;

class ImGuiDemoApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();
};
OryolMain(ImGuiDemoApp);

//------------------------------------------------------------------------------
AppState::Code
ImGuiDemoApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(1000, 650, "ImGui Demo"));

    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ImGuiDemoApp::OnRunning() {

    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.5f, 0.0f, 1.0f, 1.0f));

    Gfx::CommitFrame();

    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ImGuiDemoApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}