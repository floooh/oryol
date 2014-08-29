//------------------------------------------------------------------------------
//  NanoVG.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IOFacade.h"
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
using namespace Oryol::Core;
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
    void startLoading();
    void updateLoading();

    IOFacade* io = nullptr;
    RenderFacade* render = nullptr;
    InputFacade* input = nullptr;
    NVGFacade* nvg = nullptr;
    NVGcontext* ctx = nullptr;
    DemoData data;
    Array<Ptr<IOProtocol::Get>> ioQueue;
};
OryolMain(NanoVGApp);

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnRunning() {

    this->updateLoading();
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
    this->io = IOFacade::CreateSingle();
    this->io->RegisterFileSystem("http", Creator<HTTPFileSystem, FileSystem>());
    this->io->SetAssign("res:", "http://localhost:8000/");

    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(1024, 600, "Oryol NanoVG Sample", PixelFormat::RGB8, PixelFormat::D24S8));
    this->input  = InputFacade::CreateSingle();
    this->nvg    = NVGFacade::CreateSingle();
    this->ctx    = this->nvg->CreateContext(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    this->startLoading();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnCleanup() {
    // cleanup everything
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

//------------------------------------------------------------------------------
void
NanoVGApp::startLoading() {

    // FIXME: really need a better way for async preload, lambdas?
    StringBuilder str;
    for (int i = 0; i < 12; i++) {
        str.Format(128, "res:image%d.jpg", i+1);
        this->ioQueue.AddBack(this->io->LoadFile(str.GetString()));
    }
    this->ioQueue.AddBack(this->io->LoadFile("res:entypo.ttf"));
    this->ioQueue.AddBack(this->io->LoadFile("res:Roboto-Regular.ttf"));
    this->ioQueue.AddBack(this->io->LoadFile("res:Roboto-Bold.ttf"));
}

//------------------------------------------------------------------------------
void
NanoVGApp::updateLoading() {

    // this kinda sucks, need to fix async file loading
    StringBuilder str;
    for (int i = this->ioQueue.Size() - 1; i >= 0; i--) {
        const auto& ioReq = this->ioQueue[i];
        const String urlPath = ioReq->GetURL().Path();
        if (ioReq->Handled()) {
            if (ioReq->GetStatus() == IOStatus::OK) {
                for (int i = 0; i < 12; i++) {
                    str.Format(128, "image%d.jpg", i+1);
                    if (urlPath == str.GetString()) {
                        this->data.images[i] = this->nvg->CreateImage(this->ctx, ioReq->GetStream(), 0);
                    }
                }
                if (urlPath == "entypo.ttf") {
                    this->data.fontIcons = this->nvg->CreateFont(this->ctx, "icons", ioReq->GetStream());
                }
                if (urlPath == "Roboto-Regular.ttf") {
                    this->data.fontNormal = this->nvg->CreateFont(this->ctx, "sans", ioReq->GetStream());
                }
                if (urlPath == "Roboto-Bold.ttf") {
                    this->data.fontNormal = this->nvg->CreateFont(this->ctx, "sans-bold", ioReq->GetStream());
                }
            }
            else {
                Log::Warn("Failed to load file '%s'!\n", urlPath.AsCStr());
            }
            this->ioQueue.Erase(i);
        }
    }
}
