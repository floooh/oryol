//------------------------------------------------------------------------------
//  NanoVG.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IOFacade.h"
#include "IO/Core/IOQueue.h"
#include "HTTP/HTTPFileSystem.h"
#include "Render/RenderFacade.h"
#include "Input/InputFacade.h"
#include "Time/Clock.h"
#include "NanoVG/NVGFacade.h"
#include "NanoVG/NanoVG.h"
#include "demo.h"

using namespace Oryol;
using namespace Oryol::IO;
using namespace Oryol::HTTP;
using namespace Oryol::Render;
using namespace Oryol::Time;
using namespace Oryol::Input;
using namespace Oryol::NanoVG;

// derived application class
class NanoVGApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    void loadAssets();

    IOFacade* io = nullptr;
    RenderFacade* render = nullptr;
    InputFacade* input = nullptr;
    NVGFacade* nvg = nullptr;
    NVGcontext* ctx = nullptr;
    DemoData data;
    IOQueue ioQueue;
};
OryolMain(NanoVGApp);

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnRunning() {
    
    const DisplayAttrs& attrs = this->render->GetDisplayAttrs();
    const int32 w = attrs.FramebufferWidth;
    const int32 h = attrs.FramebufferHeight;
    if (this->render->BeginFrame()) {
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::All, glm::vec4(0.3f), 1.0f, 0);

        const Mouse& mouse = this->input->Mouse();
        const int32 mouseX = mouse.Position().x;
        const int32 mouseY = mouse.Position().y;
        const int32 blowup = this->input->Keyboard().KeyPressed(Key::Space) ? 1 : 0;
        const float64 time = Clock::Now().Since(0).AsSeconds();
        
        this->nvg->BeginFrame(this->ctx);
        renderDemo(this->ctx, mouseX, mouseY, w, h, time, blowup, &this->data);
        this->nvg->EndFrame(this->ctx);

        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnInit() {
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("res:", "http://localhost:8000/");
    this->io = IOFacade::CreateSingle(ioSetup);
    
    auto renderSetup = RenderSetup::AsWindow(1024, 600, true, "Oryol NanoVG Sample");
    this->render = RenderFacade::CreateSingle(renderSetup);
    this->input = InputFacade::CreateSingle();
    this->nvg = NVGFacade::CreateSingle();
    this->ctx = this->nvg->CreateContext(0); // this doubles draw calls: NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    
    // start loading assets asynchronously
    this->ioQueue.Start();
    StringBuilder str;
    for (int i = 0; i < 12; i++) {
        str.Format(128, "res:image%d.jpg", i+1);
        this->ioQueue.Add(str.GetString(), [this, i](const Ptr<Stream>& stream) {
            this->data.images[i] = this->nvg->CreateImage(this->ctx, stream, 0);
        });
    }
    this->ioQueue.Add("res:entypo.ttf", [this](const Ptr<Stream>& stream) {
        this->data.fontIcons = this->nvg->CreateFont(this->ctx, "icons", stream);
    });
    this->ioQueue.Add("res:Roboto-Regular.ttf", [this](const Ptr<Stream>& stream) {
        this->data.fontNormal = this->nvg->CreateFont(this->ctx, "sans", stream);
    });
    this->ioQueue.Add("res:Roboto-Bold.ttf", [this](const Ptr<Stream>& stream) {
        this->data.fontBold = this->nvg->CreateFont(this->ctx, "sans-bold", stream);
    });
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnCleanup() {
    // cleanup everything
    this->ioQueue.Stop();
    freeDemoData(this->ctx, &this->data);
    this->nvg->DeleteContext(this->ctx);
    this->ctx = nullptr;
    this->nvg = nullptr;
    this->input = nullptr;
    this->render = nullptr;
    NVGFacade::DestroySingle();
    InputFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
