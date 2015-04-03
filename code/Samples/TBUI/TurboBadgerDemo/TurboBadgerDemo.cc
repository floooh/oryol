//-----------------------------------------------------------------------------
//  TurboBadgerDemo.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IO.h"
#include "IO/Core/IOQueue.h"
#include "HTTP/HTTPFileSystem.h"
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
    /// load initial resources, then call callback
    void loadInitResources(const TBUISetup& setup, IOQueue::GroupSuccessFunc onLoaded);

    IOQueue ioQueue;
};
OryolMain(TurboBadgerDemoApp);

//-----------------------------------------------------------------------------
AppState::Code
TurboBadgerDemoApp::OnInit() {

    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("res:", "http://localhost:8000/");
    ioSetup.Assigns.Add("ui:", "res:tbui/");
    IO::Setup(ioSetup);

    Gfx::Setup(GfxSetup::Window(512, 256, "TurboBadger UI Demo"));
    
    TBUISetup tbuiSetup;
    tbuiSetup.Locale = "ui:language/lng_en.tb.txt";
    tbuiSetup.DefaultSkin = "ui:default_skin/skin.tb.txt";
    tbuiSetup.OverrideSkin = "ui:demo/skin/skin.tb.txt";
    tbuiSetup.Fonts.Add("ui:default_font/segoe_white_with_shadow.tb.txt", "Segoe");
    tbuiSetup.Fonts.Add("ui:demo/fonts/neon.tb.txt", "Neon");
    tbuiSetup.Fonts.Add("ui:demo/fonts/orangutang.tb.txt", "Orangutang");
    tbuiSetup.Fonts.Add("ui:demo/fonts/orange.tb.txt", "Orange");
    TBUI::Setup(tbuiSetup);
    
    this->loadInitResources(tbuiSetup, [this](const Array<Ptr<Stream>>& streams)  {
        for (const auto& stream : streams) {
            TBUI::Resource().Add(stream);
        }
        TBUI::InitTurboBadger();
    });
    
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
    this->ioQueue.Stop();
    TBUI::Discard();
    Gfx::Discard();
    IO::Discard();
    return App::OnCleanup();
}

//-----------------------------------------------------------------------------
void
TurboBadgerDemoApp::loadInitResources(const TBUISetup& setup, IOQueue::GroupSuccessFunc onLoaded) {

    // FIXME: some sort of resource bundling would be nice here...
    Array<URL> urls;
    urls.Add(setup.Locale);
    urls.Add(setup.DefaultSkin);
    urls.Add(setup.OverrideSkin);
    for (const auto& font : setup.Fonts) {
        urls.Add(font.Location);
    }
    this->ioQueue.AddGroup(urls, onLoaded);
    this->ioQueue.Start();
}

