//-----------------------------------------------------------------------------
//  TurboBadgerDemo.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "TBUI/TBUI.h"

using namespace Oryol;

class TurboBadgerDemoApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();
private:
};
OryolMain(TurboBadgerDemoApp);

//-----------------------------------------------------------------------------
AppState::Code
TurboBadgerDemoApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(1024, 512, "TurboBadger UI Demo"));
    TBUI::Setup();

    return AppState::Running;
}

//-----------------------------------------------------------------------------
AppState::Code
TurboBadgerDemoApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.5f, 0.0f, 1.0f, 1.0f));
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//-----------------------------------------------------------------------------
AppState::Code
TurboBadgerDemoApp::OnCleanup() {
    TBUI::Discard();
    Gfx::Discard();
    return AppState::Destroy;
}
