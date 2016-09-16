//------------------------------------------------------------------------------
//  VertexCapture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

class VertexCaptureApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

};
OryolMain(VertexCaptureApp);

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnInit() {
    Gfx::Setup(GfxSetup::WindowMSAA4(800, 600, "Oryol Vertex Capture Sample"));

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnRunning() {

    Gfx::ApplyDefaultRenderTarget();

    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

