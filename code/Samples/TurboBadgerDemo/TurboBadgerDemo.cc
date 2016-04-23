//-----------------------------------------------------------------------------
//  TurboBadgerDemo.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/Time/Clock.h"
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
#include "TBUI/TBUI.h"
#include "TBDemoWindows.h"

using namespace Oryol;
using namespace tb;

class TurboBadgerDemoApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();
private:
    /// return array of initial resource URLs
    Array<URL> getInitResources(const TBUISetup& setup);

    TimePoint lastFrameTimePoint;
    ClearState clearState;
};
OryolMain(TurboBadgerDemoApp);

AdvancedItemSource advanced_source;
TBGenericStringItemSource name_source;
TBGenericStringItemSource popup_menu_source;

const char *girl_names[] = {
    "Maja", "Alice", "Julia", "Linnéa", "Wilma", "Ella", "Elsa", "Emma", "Alva", "Olivia", "Molly", "Ebba", "Klara", "Nellie", "Agnes",
    "Isabelle", "Ida", "Elin", "Ellen", "Moa", "Emilia", "Nova", "Alma", "Saga", "Amanda", "Isabella", "Lilly", "Alicia", "Astrid",
    "Matilda", "Tuva", "Tilde", "Stella", "Felicia", "Elvira", "Tyra", "Hanna", "Sara", "Vera", "Thea", "Freja", "Lova", "Selma",
    "Meja", "Signe", "Ester", "Lovisa", "Ellie", "Lea", "Tilda", "Tindra", "Sofia", "Nora", "Nathalie", "Leia", "Filippa", "Siri",
    "Emelie", "Inez", "Edith", "Stina", "Liv", "Lisa", "Linn", "Tove", "Emmy", "Livia", "Jasmine", "Evelina", "Cornelia", "Märta",
    "Svea", "Ingrid", "My", "Rebecca", "Joline", "Mira", "Ronja", "Hilda", "Melissa", "Anna", "Frida", "Maria", "Iris", "Josefine",
    "Elise", "Elina", "Greta", "Vilda", "Minna", "Lina", "Hedda", "Nicole", "Kajsa", "Majken", "Sofie", "Annie", "Juni", "Novalie", "Hedvig", 0 };
const char *boy_names[] = {
    "Oscar", "William", "Lucas", "Elias", "Alexander", "Hugo", "Oliver", "Theo", "Liam", "Leo", "Viktor", "Erik", "Emil",
    "Isak", "Axel", "Filip", "Anton", "Gustav", "Edvin", "Vincent", "Arvid", "Albin", "Ludvig", "Melvin", "Noah", "Charlie", "Max",
    "Elliot", "Viggo", "Alvin", "Alfred", "Theodor", "Adam", "Olle", "Wilmer", "Benjamin", "Simon", "Nils", "Noel", "Jacob", "Leon",
    "Rasmus", "Kevin", "Linus", "Casper", "Gabriel", "Jonathan", "Milo", "Melker", "Felix", "Love", "Ville", "Sebastian", "Sixten",
    "Carl", "Malte", "Neo", "David", "Joel", "Adrian", "Valter", "Josef", "Jack", "Hampus", "Samuel", "Mohammed", "Alex", "Tim",
    "Daniel", "Vilgot", "Wilhelm", "Harry", "Milton", "Maximilian", "Robin", "Sigge", "Måns", "Eddie", "Elton", "Vidar", "Hjalmar",
    "Loke", "Elis", "August", "John", "Hannes", "Sam", "Frank", "Svante", "Marcus", "Mio", "Otto", "Ali", "Johannes", "Fabian",
    "Ebbe", "Aron", "Julian", "Elvin", "Ivar", 0 };


//-----------------------------------------------------------------------------
AppState::Code
TurboBadgerDemoApp::OnInit() {

    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("res:", ORYOL_SAMPLE_URL);
    ioSetup.Assigns.Add("ui:", "res:tbui/");
    IO::Setup(ioSetup);

    Gfx::Setup(GfxSetup::Window(1000, 650, "TurboBadger UI Demo"));
    Dbg::Setup();
    Input::Setup();
    Input::BeginCaptureText();
    this->clearState.Color = glm::vec4(0.5f, 0.0f, 1.0f, 1.0f);
    
    // TBSelectList and TBSelectDropdown widgets have a default item source that are fed with any items
    // specified in the resource files. But it is also possible to set any source which can save memory
    // and improve performance. Then you don't have to populate each instance with its own set of items,
    // for widgets that occur many times in a UI, always with the same items.
    // Here we prepare the name source, that is used in a few places.
    for (int i = 0; boy_names[i]; i++) {
        advanced_source.AddItem(new AdvancedItem(boy_names[i++], TBIDC("boy_item"), true));
    }
    for (int i = 0; girl_names[i]; i++) {
        advanced_source.AddItem(new AdvancedItem(girl_names[i++], TBIDC("girl_item"), false));
    }
    for (int i = 0; girl_names[i]; i++) {
        name_source.AddItem(new TBGenericStringItem(girl_names[i++], TBIDC("girl_item")));
    }
    for (int i = 0; boy_names[i]; i++) {
        name_source.AddItem(new TBGenericStringItem(boy_names[i++], TBIDC("boy_item")));
    }
    advanced_source.SetSort(TB_SORT_ASCENDING);
    name_source.SetSort(TB_SORT_ASCENDING);
    
    // Prepare a source with submenus (with eternal recursion) so we can test sub menu support.
    popup_menu_source.AddItem(new TBGenericStringItem("Option 1", TBIDC("opt 1")));
    popup_menu_source.AddItem(new TBGenericStringItem("Option 2", TBIDC("opt 2")));
    popup_menu_source.AddItem(new TBGenericStringItem("-"));
    popup_menu_source.AddItem(new TBGenericStringItem("Same submenu", &popup_menu_source));
    popup_menu_source.AddItem(new TBGenericStringItem("Long submenu", &name_source));
    // Give the first item a skin image
    popup_menu_source.GetItem(0)->SetSkinImage(TBIDC("Icon16"));

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
    TBUI::DoAfter(this->getInitResources(tbuiSetup), [this] {
        TBUI::InitTurboBadger();
        TBUI::DoAfter(MainWindow::GetMainResource(), [] {
            new MainWindow;
        });
        TBUI::DoAfter(TabContainerWindow::GetMainResource(), [] {
            new TabContainerWindow;
        });
        TBUI::DoAfter(AdvancedListWindow::GetResources(), [this] {
            new AdvancedListWindow(&advanced_source);
        });
        TBUI::DoAfter(ListWindow::GetMainResource(), [this] {
            new ListWindow(&name_source);
        });
        TBUI::DoAfter(EditWindow::GetResources(), [] {
            new EditWindow;
        });
    });

    return AppState::Running;
}

//-----------------------------------------------------------------------------
AppState::Code
TurboBadgerDemoApp::OnRunning() {

    Gfx::ApplyDefaultRenderTarget(this->clearState);
    TBUI::Draw();
    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("\n frame=%.3fms\n\r", frameTime.AsMilliSeconds());

    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//-----------------------------------------------------------------------------
AppState::Code
TurboBadgerDemoApp::OnCleanup() {
    TBUI::Discard();
    Dbg::Discard();
    Input::Discard();
    Gfx::Discard();
    IO::Discard();
    return App::OnCleanup();
}

//-----------------------------------------------------------------------------
Array<URL>
TurboBadgerDemoApp::getInitResources(const TBUISetup& setup) {

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
    return urls;
}

