//------------------------------------------------------------------------------
//  NanoVG.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IO.h"
#include "IO/Core/IOQueue.h"
#include "HTTP/HTTPFileSystem.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "Time/Clock.h"
#include "NanoVG/NanoVG.h"
#include "NanoVG/NanoVGWrapper.h"
#include "demo.h"

using namespace Oryol;

class NanoVGApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    void loadAssets();

    NVGcontext* ctx = nullptr;
    DemoData data;
    IOQueue ioQueue;
};
OryolMain(NanoVGApp);

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnRunning() {
    
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.3f), 1.0f, 0);

    const int32 w = Gfx::DisplayAttrs().FramebufferWidth;
    const int32 h = Gfx::DisplayAttrs().FramebufferHeight;
    const int32 mouseX = (const int32) Input::Mouse().Position.x;
    const int32 mouseY = (const int32) Input::Mouse().Position.y;
    const int32 blowup = Input::Keyboard().KeyPressed(Key::Space) ? 1 : 0;
    const float64 time = Clock::Now().Since(0).AsSeconds();
    
    NanoVG::BeginFrame(this->ctx);
    renderDemo(this->ctx, mouseX, mouseY, w, h, time, blowup, &this->data);
    NanoVG::EndFrame(this->ctx);

    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnInit() {

    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("res:", ORYOL_SAMPLE_URL);
    IO::Setup(ioSetup);
    
    auto gfxSetup = GfxSetup::WindowMSAA4(1024, 600, "Oryol NanoVG Sample");
    Gfx::Setup(gfxSetup);
    Input::Setup();
    NanoVG::Setup();
    this->ctx = NanoVG::CreateContext(0); // this doubles draw calls: NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    
    // start loading assets asynchronously
    this->ioQueue.Start();
    StringBuilder str;
    for (int i = 0; i < 12; i++) {
        str.Format(128, "res:image%d.jpg", i+1);
        this->ioQueue.Add(str.GetString(), [this, i](const Ptr<Stream>& stream) {
            this->data.images[i] = NanoVG::CreateImage(this->ctx, stream, 0);
        });
    }
    this->ioQueue.Add("res:entypo.ttf", [this](const Ptr<Stream>& stream) {
        this->data.fontIcons = NanoVG::CreateFont(this->ctx, "icons", stream);
    });
    this->ioQueue.Add("res:Roboto-Regular.ttf", [this](const Ptr<Stream>& stream) {
        this->data.fontNormal = NanoVG::CreateFont(this->ctx, "sans", stream);
    });
    this->ioQueue.Add("res:Roboto-Bold.ttf", [this](const Ptr<Stream>& stream) {
        this->data.fontBold = NanoVG::CreateFont(this->ctx, "sans-bold", stream);
    });
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnCleanup() {
    // cleanup everything
    this->ioQueue.Stop();
    freeDemoData(this->ctx, &this->data);
    NanoVG::DeleteContext(this->ctx);
    this->ctx = nullptr;
    NanoVG::Discard();
    Input::Discard();
    Gfx::Discard();
    IO::Discard();
    return App::OnCleanup();
}
