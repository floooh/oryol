//------------------------------------------------------------------------------
//  NanoVG.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IO.h"
#include "IO/Core/IOQueue.h"
#include "HTTP/HTTPFileSystem.h"
#include "Render/Render.h"
#include "Input/Input.h"
#include "Time/Clock.h"
#include "NanoVG/NanoVG.h"
#include "NanoVG/NanoVGWrapper.h"
#include "demo.h"

using namespace Oryol;

// derived application class
class NanoVGApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
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
    
    if (Render::BeginFrame()) {
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::All, glm::vec4(0.3f), 1.0f, 0);

        const int32 w = Render::DisplayAttrs().FramebufferWidth;
        const int32 h = Render::DisplayAttrs().FramebufferHeight;
        const int32 mouseX = Input::Mouse().Position().x;
        const int32 mouseY = Input::Mouse().Position().y;
        const int32 blowup = Input::Keyboard().KeyPressed(Key::Space) ? 1 : 0;
        const float64 time = Clock::Now().Since(0).AsSeconds();
        
        NanoVG::BeginFrame(this->ctx);
        renderDemo(this->ctx, mouseX, mouseY, w, h, time, blowup, &this->data);
        NanoVG::EndFrame(this->ctx);

        Render::EndFrame();
    }
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnInit() {
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("res:", "http://localhost:8000/");
    IO::Setup(ioSetup);
    
    auto renderSetup = RenderSetup::Window(1024, 600, true, "Oryol NanoVG Sample");
    Render::Setup(renderSetup);
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
    Render::Discard();
    IO::Discard();
    return App::OnCleanup();
}
