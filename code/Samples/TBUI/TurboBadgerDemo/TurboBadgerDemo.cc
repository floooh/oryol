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
    tbuiSetup.DefaultFontName = "Segoe";
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
    Array<URL> urls({
        "ui:demo/skin/focus_r8.png" ,
        "ui:demo/skin/special_button.png",
        "ui:demo/skin/star_gray.png",
        "ui:demo/skin/special_button_pressed.png",
        "ui:demo/skin/remove.png",
        "ui:demo/skin/bg_tile.png",
        "ui:demo/skin/icon16.png",
        "ui:demo/skin/image_frame.png",
        "ui:demo/skin/star_gold.png",
        "ui:demo/skin/icon128.png",
        "ui:demo/skin/icon48.png",
        
        "ui:default_font/segoe_white_with_shadow_14.png",
        
        "ui:default_skin/editfield.png",
        "ui:default_skin/tab_button_left_active.png",
        "ui:default_skin/arrow_up.png",
        "ui:default_skin/button_flat_outline.png",
        "ui:default_skin/window_close.png",
        "ui:default_skin/slider_bg_x.png",
        "ui:default_skin/button_pressed.png",
        "ui:default_skin/item_selected.png",
        "ui:default_skin/tab_button_top_inactive.png",
        "ui:default_skin/container.png",
        "ui:default_skin/arrow_left.png",
        "ui:default_skin/slider_handle.png",
        "ui:default_skin/button_grouped_x_middle_up.png",
        "ui:default_skin/window_mover_bg_tile.png",
        "ui:default_skin/item_hover.png",
        "ui:default_skin/fadeout_x.png",
        "ui:default_skin/window.png",
        "ui:default_skin/button_grouped_x_first_up.png",
        "ui:default_skin/item_separator_x.png",
        "ui:default_skin/button_grouped_x_last_up.png",
        "ui:default_skin/checkbox.png",
        "ui:default_skin/fadeout_y.png",
        "ui:default_skin/window_mover_overlay.png",
        "ui:default_skin/arrow_right.png",
        "ui:default_skin/tab_button_right_active.png",
        "ui:default_skin/tab_button_bottom_active.png",
        "ui:default_skin/window_active.png",
        "ui:default_skin/TBSectionContainer.png",
        "ui:default_skin/radio_mark.png",
        "ui:default_skin/focus_tabbutton_top.png",
        "ui:default_skin/tab_button_left_inactive.png",
        "ui:default_skin/arrow_down.png",
        "ui:default_skin/tab_button_bottom_inactive.png",
        "ui:default_skin/toggle_section_icon_middle.png",
        "ui:default_skin/search.png",
        "ui:default_skin/checkbox_pressed.png",
        "ui:default_skin/resizer.png",
        "ui:default_skin/button.png",
        "ui:default_skin/radio_pressed.png",
        "ui:default_skin/radio.png",
        "ui:default_skin/tab_button_top_active.png",
        "ui:default_skin/item_separator_y.png",
        "ui:default_skin/checkbox_mark.png",
        "ui:default_skin/window_close_pressed.png",
        "ui:default_skin/scroll_bg_x.png",
        "ui:default_skin/button_grouped_x_middle_down.png",
        "ui:default_skin/scroll_fg_x.png",
        "ui:default_skin/focus_tabbutton_left.png",
        "ui:default_skin/focus_r4.png",
        "ui:default_skin/tab_button_right_inactive.png",
        "ui:default_skin/progress_spinner_strip.png",
        "ui:default_skin/scroll_bg_y.png",
        "ui:default_skin/button_grouped_x_first_down.png",
        "ui:default_skin/button_flat_pressed.png",
        "ui:default_skin/toggle_section_icon_down.png",
        "ui:default_skin/scroll_fg_y.png",
        "ui:default_skin/button_grouped_x_last_down.png",
        "ui:default_skin/selection.png",
        "ui:default_skin/slider_bg_y.png",
        "ui:default_skin/toggle_section_icon_up.png",
    });
    
    urls.Add(setup.Locale);
    urls.Add(setup.DefaultSkin);
    urls.Add(setup.OverrideSkin);
    for (const auto& font : setup.Fonts) {
        urls.Add(font.Location);
    }
    this->ioQueue.AddGroup(urls, onLoaded);
    this->ioQueue.Start();
}

