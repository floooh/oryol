//------------------------------------------------------------------------------
//  MultipleRenderTarget.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

class MultipleRenderTargetApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
private:
    const int DisplayWidth = 640;
    const int DisplayHeight = 480;

    Id renderPass;
};
OryolMain(MultipleRenderTargetApp);

//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(DisplayWidth, DisplayHeight, "Oryol MRT Sample");
    Gfx::Setup(gfxSetup);

    auto rtSetup = TextureSetup::RenderTarget(DisplayWidth, DisplayHeight);
    rtSetup.DepthFormat = PixelFormat::DEPTHSTENCIL;
    Id rt0 = Gfx::CreateResource(rtSetup);
    rtSetup.DepthFormat = PixelFormat::None;
    Id rt1 = Gfx::CreateResource(rtSetup);
    auto passSetup = RenderPassSetup::From({ rt0, rt1 }, rt0);
    this->renderPass = Gfx::CreateResource(passSetup);
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::OnRunning() {
    Gfx::BeginPass();

    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}


//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
