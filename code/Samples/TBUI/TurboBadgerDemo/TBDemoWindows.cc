//------------------------------------------------------------------------------
//  TBDemoWindows.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "tb_message_window.h"
#include "animation/tb_widget_animation.h"
#include "tb_select.h"
#include "tb_inline_select.h"
#include "tb_tab_container.h"
#include "tb_system.h"
#include "tb_widgets_reader.h"
#include "tb_menu_window.h"
#include "tb_tempbuffer.h"
#include "TBUI/TBUI.h"
#include "TBDemoWindows.h"

using namespace Oryol;
using namespace tb;

extern AdvancedItemSource advanced_source;
extern TBGenericStringItemSource name_source;
extern TBGenericStringItemSource popup_menu_source;

//==============================================================================
MainWindow::MainWindow() {
    this->LoadResourceFile(GetMainResource());
    this->SetOpacity(0.97f);
}

//------------------------------------------------------------------------------
URL
MainWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_ui.tb.txt";
}

//------------------------------------------------------------------------------
void
MainWindow::OnMessageReceived(TBMessage* msg) {
    Log::Info("MainWindow::OnMessageReceived()\n");
}

//------------------------------------------------------------------------------
bool
MainWindow::OnEvent(const TBWidgetEvent& ev) {
    if (ev.type == EVENT_TYPE_CLICK) {
        if (ev.target->GetID() == TBIDC("new")) {
            new MainWindow();
            return true;
        }
        if (ev.target->GetID() == TBIDC("msg")) {
            this->PostMessage(TBIDC("instantmsg"), nullptr);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("busymsg")) {
            if (ev.target->GetValue() == 1) {
                // Post the first "busy" message when we check the checkbox.
                o_assert_dbg(!this->GetMessageByID(TBIDC("busy")));
                if (!this->GetMessageByID(TBIDC("busy"))) {
                    this->PostMessage(TBIDC("busy"), nullptr);
                    TBMessageWindow *msg_win = new TBMessageWindow(this, TBIDC("test_dialog"));
                    msg_win->Show("Message window", "The message loop is now constantly busy with messages to process.\n\n"
                        "The main thread should be working hard, but input & animations should still be running smoothly.");
                }
            }
            else {
                // Remove any pending "busy" message when we uncheck the checkbox.
                o_assert_dbg(this->GetMessageByID(TBIDC("busy")));
                if (TBMessage *busymsg = this->GetMessageByID(TBIDC("busy"))) {
                    this->DeleteMessage(busymsg);
                }
            }
			return true;
		}
        else if (ev.target->GetID() == TBIDC("delayedmsg")) {
            this->PostMessageDelayed(TBIDC("delayedmsg"), nullptr, 2000);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("TBWindow.close")) {
            // Intercept the TBWindow.close message and stop it from bubbling
            // to TBWindow (prevent the window from closing)
            TBMessageWindow *msg_win = new TBMessageWindow(this, TBIDC("confirm_close_dialog"));
            TBMessageWindowSettings settings(TB_MSG_YES_NO, TBIDC("Icon48"));
            settings.dimmer = true;
            settings.styling = true;
            msg_win->Show("Are you sure?", "Really <color #0794f8>close</color> the window?", &settings);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("confirm_close_dialog")) {
            if (ev.ref_id == TBIDC("TBMessageWindow.yes")) {
                this->Close();
            }
            return true;
        }
        else if (ev.target->GetID() == TBIDC("reload skin bitmaps")) {
            int reload_count = 10;
            double t1 = TBSystem::GetTimeMS();
            for (int i = 0; i < reload_count; i++) {
                g_tb_skin->ReloadBitmaps();
            }
            double t2 = TBSystem::GetTimeMS();

            TBStr message;
            message.SetFormatted("Reloading the skin graphics %d times took %dms", reload_count, (int)(t2 - t1));
            TBMessageWindow *msg_win = new TBMessageWindow(ev.target, TBID());
            msg_win->Show("GFX load performance", message);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test context lost")) {
            g_renderer->InvokeContextLost();
            g_renderer->InvokeContextRestored();
            TBMessageWindow *msg_win = new TBMessageWindow(ev.target, TBID());
            msg_win->Show("Context lost & restore",
                "Called InvokeContextLost and InvokeContextRestored.\n\n"
                "Does everything look fine?");
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-layout")) {            
            TBStr resource_file("ui:demo/ui_resources/");
            resource_file.Append(ev.target->data.GetString());
            TBUI::DoAfter(resource_file.CStr(), [resource_file] () {
                new LayoutWindow(resource_file);
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-connections")) {
            TBUI::DoAfter(ConnectionWindow::GetMainResource(), [] () {
                new ConnectionWindow();
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-list")) {
            TBUI::DoAfter(AdvancedListWindow::GetMainResource(), [] () {
                new AdvancedListWindow(&advanced_source);
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-image")) {
            TBUI::DoAfter(Array<URL>({
                ImageWindow::GetMainResource(),
                "ui:demo/images/image_1.png",
                "ui:demo/images/image_2.png",
                "ui:demo/images/image_3.png",
                "ui:demo/images/image_4.png",
                "ui:demo/images/image_5.png",
                "ui:demo/images/image_6.png",
                "ui:demo/images/image_7.png",
                "ui:demo/images/image_8.png",
                "ui:demo/images/image_9.png"
            }), [] {
                new ImageWindow();
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-page")) {
            TBUI::DoAfter(PageWindow::GetMainResource(), [] {
                new PageWindow();
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-animations")) {
            TBUI::DoAfter(AnimationsWindow::GetMainResource(), [] {
                new AnimationsWindow();
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-scroll-container")) {
            TBUI::DoAfter(ScrollContainerWindow::GetMainResources(), [] {
                new ScrollContainerWindow();            
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-skin-conditions")) {
            TBUI::DoAfter(Array<URL>({
                "ui:demo/ui_resources/test_skin_conditions01.tb.txt",
                "ui:demo/ui_resources/test_skin_conditions02.tb.txt"}), [] {
                
                TBUIWindow* w0 = new TBUIWindow();
                TBUIWindow* w1 = new TBUIWindow();
                w0->LoadResourceFile("ui:demo/ui_resources/test_skin_conditions01.tb.txt");
                w1->LoadResourceFile("ui:demo/ui_resources/test_skin_conditions02.tb.txt");
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-resource-edit")) {
            TBUI::DoAfter(Array<URL>({
                "ui:demo/ui_resources/resource_edit_test.tb.txt",
                "ui:demo/ui_resources/resource_edit_window.tb.txt" }), [this] {
                
                ResourceEditWindow *res_edit_win = new ResourceEditWindow();
                res_edit_win->Load("ui:demo/ui_resources/resource_edit_test.tb.txt");
                //this->GetParent()->AddChild(res_edit_win);
            });
            return true;
        }
        else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("debug settings")) {
            #ifdef TB_RUNTIME_DEBUG_INFO
            this->ShowDebugInfoSettingsWindow(GetParentRoot());
            #else
            TBMessageWindow *msg_win = new TBMessageWindow(ev.target, TBID());
            msg_win->Show("Debug settings",
                "Debug settings is only available in builds "
                "compiled with TB_RUNTIME_DEBUG_INFO defined.\n\n"
                "Debug builds enable this by default.");
            #endif
            return true;
        }
    }
    return TBUIWindow::OnEvent(ev);
}

//==============================================================================
ImageWindow::ImageWindow() {
    this->LoadResourceFile(GetMainResource());
}

//------------------------------------------------------------------------------
URL
ImageWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_image_widget.tb.txt";
}

//------------------------------------------------------------------------------
bool ImageWindow::OnEvent(const TBWidgetEvent &ev) {
    if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("remove")) {
        TBWidget *image = ev.target->GetParent();
        image->GetParent()->RemoveChild(image);
        delete image;
        return true;
    }
    return TBUIWindow::OnEvent(ev);
}

//==============================================================================
PageWindow::PageWindow() {
    this->LoadResourceFile(GetMainResource());

    // Listen to the pagers scroller
    if (TBWidget *pager = GetWidgetByID(TBIDC("page-scroller")))
        pager->GetScroller()->SetSnapListener(this);
}

//------------------------------------------------------------------------------
URL
PageWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_scroller_snap.tb.txt";
}

//------------------------------------------------------------------------------
void
PageWindow::OnScrollSnap(TBWidget *target_widget, int &target_x, int &target_y) {
    int page_w = target_widget->GetPaddingRect().w;
    int target_page = (target_x + page_w / 2) / page_w;
    target_x = target_page * page_w;
}

//==============================================================================
AnimationsWindow::AnimationsWindow() {
    this->LoadResourceFile(GetMainResource());
    this->Animate();
}

//------------------------------------------------------------------------------
URL
AnimationsWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_animations.tb.txt";
}

//------------------------------------------------------------------------------
void
AnimationsWindow::Animate() {
    // Abort any still unfinished animations.
    TBWidgetsAnimationManager::AbortAnimations(this);

    ANIMATION_CURVE curve = ANIMATION_CURVE_SLOW_DOWN;
    double duration = 500;
    bool fade = true;

    if (TBSelectList *curve_select = GetWidgetByIDAndType<TBSelectList>("curve")) {
        curve = static_cast<ANIMATION_CURVE>(curve_select->GetValue());
    }
    if (TBInlineSelect *duration_select = GetWidgetByIDAndType<TBInlineSelect>("duration")) {
        duration = duration_select->GetValueDouble();
    }
    if (TBCheckBox *fade_check = GetWidgetByIDAndType<TBCheckBox>("fade")) {
        fade = fade_check->GetValue() ? true : false;
    }

    // Start move animation
    if (TBAnimationObject *anim = new TBWidgetAnimationRect(this, GetRect().Offset(-GetRect().x - GetRect().w, 0), GetRect())) {
        TBAnimationManager::StartAnimation(anim, curve, duration);
    }
    // Start fade animation
    if (fade) {
        if (TBAnimationObject *anim = new TBWidgetAnimationOpacity(this, TB_ALMOST_ZERO_OPACITY, 1, false)) {
            TBAnimationManager::StartAnimation(anim, ANIMATION_CURVE_SLOW_DOWN, duration);
        }
    }
}

//------------------------------------------------------------------------------
bool AnimationsWindow::OnEvent(const TBWidgetEvent &ev) {
    if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("Animate!")) {
        Animate();
    }
    return TBUIWindow::OnEvent(ev);
}

//==============================================================================
TabContainerWindow::TabContainerWindow()
{
    LoadResourceFile(GetMainResource());
}

//------------------------------------------------------------------------------
URL
TabContainerWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_tabcontainer01.tb.txt";
}

//------------------------------------------------------------------------------
bool
TabContainerWindow::OnEvent(const TBWidgetEvent &ev) {
    if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("set_align")) {
        if (TBTabContainer *tc = GetWidgetByIDAndType<TBTabContainer>(TBIDC("tabcontainer"))) {
            tc->SetAlignment(static_cast<TB_ALIGN>(ev.target->data.GetInt()));
        }
        ResizeToFitContent(RESIZE_FIT_CURRENT_OR_NEEDED);
    }
    else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("toggle_tab_axis")) {
        static AXIS axis = AXIS_X;
        axis = axis == AXIS_X ? AXIS_Y : AXIS_X;
        if (TBTabContainer *tc = GetWidgetByIDAndType<TBTabContainer>(TBIDC("tabcontainer"))) {
            for (TBWidget *child = tc->GetTabLayout()->GetFirstChild(); child; child = child->GetNext()) {
                if (TBButton *button = TBSafeCast<TBButton>(child)) {
                    button->SetAxis(axis);
                }
            }
        }
        ResizeToFitContent(RESIZE_FIT_CURRENT_OR_NEEDED);
    }
    else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("start_spinner")) {
        if (TBProgressSpinner *spinner = GetWidgetByIDAndType<TBProgressSpinner>(TBIDC("spinner"))) {
            spinner->SetValue(1);
        }
    }
    else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("stop_spinner")) {
        if (TBProgressSpinner *spinner = GetWidgetByIDAndType<TBProgressSpinner>(TBIDC("spinner"))) {
            spinner->SetValue(0);
        }
    }
    return TBUIWindow::OnEvent(ev);
}

//==============================================================================
LayoutWindow::LayoutWindow(const char* filename) {
    LoadResourceFile(filename);
}

//------------------------------------------------------------------------------
bool
LayoutWindow::OnEvent(const TBWidgetEvent& ev) {
    if (ev.type == EVENT_TYPE_CHANGED && ev.target->GetID() == TBIDC("select position")) {
        LAYOUT_POSITION pos = LAYOUT_POSITION_CENTER;
        if (TBSelectDropdown *select = GetWidgetByIDAndType<TBSelectDropdown>(TBIDC("select position"))) {
            pos = static_cast<LAYOUT_POSITION>(select->GetValue());
        }
        for (int i = 0; i < 3; i++) {
            if (TBLayout *layout = GetWidgetByIDAndType<TBLayout>(i + 1)) {
                layout->SetLayoutPosition(pos);
            }
        }
        return true;
    }
    else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("toggle axis")) {
        static AXIS axis = AXIS_Y;
        for (int i = 0; i < 3; i++) {
            if (TBLayout *layout = GetWidgetByIDAndType<TBLayout>(i + 1)) {
                layout->SetAxis(axis);
            }
        }
        axis = axis == AXIS_X ? AXIS_Y : AXIS_X;
        if (TBLayout *layout = GetWidgetByIDAndType<TBLayout>(TBIDC("switch_layout"))) {
            layout->SetAxis(axis);
        }
        ResizeToFitContent(RESIZE_FIT_CURRENT_OR_NEEDED);
        return true;
    }
    return TBUIWindow::OnEvent(ev);
}

//==============================================================================
ConnectionWindow::ConnectionWindow()
{
    LoadResourceFile(GetMainResource());
}

//------------------------------------------------------------------------------
URL
ConnectionWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_connections.tb.txt";
}

//------------------------------------------------------------------------------
bool
ConnectionWindow::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("reset-master-volume")) {
        if (TBWidgetValue *val = g_value_group.GetValue(TBIDC("master-volume"))) {
            val->SetInt(50);
        }
    }
    else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("reset-user-name")) {
        if (TBWidgetValue *val = g_value_group.GetValue(TBIDC("user-name"))) {
            val->SetText("");
        }
    }
	return TBUIWindow::OnEvent(ev);
}

//==============================================================================
ScrollContainerWindow::ScrollContainerWindow() {
    LoadResourceFile(GetMainResources()[0]);
    if (TBSelectDropdown *select = GetWidgetByIDAndType<TBSelectDropdown>(TBIDC("name dropdown"))) {
        select->SetSource(&name_source);
    }
    if (TBSelectDropdown *select = GetWidgetByIDAndType<TBSelectDropdown>(TBIDC("advanced dropdown"))) {
        select->SetSource(&advanced_source);
    }
}

//------------------------------------------------------------------------------
Array<URL>
ScrollContainerWindow::GetMainResources() {
    return Array<URL>({
        "ui:demo/ui_resources/test_scrollcontainer.tb.txt",
        "ui:demo/fonts/orange_30.png",
        "ui:default_font/segoe_white_with_shadow_28.png"});
}

//------------------------------------------------------------------------------
bool
ScrollContainerWindow::OnEvent(const TBWidgetEvent &ev) {
    if (ev.type == EVENT_TYPE_CLICK) {
        if (ev.target->GetID() == TBIDC("add img")) {
            TBButton *button = TBSafeCast<TBButton>(ev.target);
            TBSkinImage *skin_image = new TBSkinImage;
            skin_image->SetSkinBg(TBIDC("Icon16"));
            button->GetContentRoot()->AddChild(skin_image, WIDGET_Z_BOTTOM);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("new buttons")) {
            for(int i = 0; i < ev.target->data.GetInt(); i++) {
                TBStr str;
                str.SetFormatted("Remove %d", i);
                TBButton *button = new TBButton;
                button->SetID(TBIDC("remove button"));
                button->SetText(str);
                ev.target->GetParent()->AddChild(button);
            }
            return true;
		}
        else if (ev.target->GetID() == TBIDC("new buttons delayed")) {
            for(int i = 0; i < ev.target->data.GetInt(); i++) {
                TBMessageData *data = new TBMessageData();
                data->id1 = ev.target->GetParent()->GetID();
                data->v1.SetInt(i);
                PostMessageDelayed(TBIDC("new button"), data, 100 + i * 500);
            }
            return true;
        }
        else if (ev.target->GetID() == TBIDC("remove button")) {
            ev.target->GetParent()->RemoveChild(ev.target);
            delete ev.target;
            return true;
        }
        else if (ev.target->GetID() == TBIDC("showpopupmenu1")) {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("popupmenu1"))) {
                menu->Show(&popup_menu_source, TBPopupAlignment());
            }
            return true;
        }
        else if (ev.target->GetID() == TBIDC("popupmenu1")) {
            TBStr str;
            str.SetFormatted("Menu event received!\nref_id: %d", (int)ev.ref_id);
            TBMessageWindow *msg_win = new TBMessageWindow(this, TBIDC("popup_dialog"));
            msg_win->Show("Info", str);
            return true;
        }
    }
    return TBUIWindow::OnEvent(ev);
}

//------------------------------------------------------------------------------
void
ScrollContainerWindow::OnMessageReceived(TBMessage *msg) {
    if (msg->message == TBIDC("new button") && msg->data) {
        if (TBWidget *target = GetWidgetByID(msg->data->id1)) {
            TBStr str;
            str.SetFormatted("Remove %d", msg->data->v1.GetInt());
            TBButton *button = new TBButton;
            button->SetID(TBIDC("remove button"));
            button->SetText(str);
            target->AddChild(button);
        }
    }
}

//==============================================================================
AdvancedItemWidget::AdvancedItemWidget(AdvancedItem *item, AdvancedItemSource *source,
    TBSelectItemViewer *source_viewer, int index)
    : m_source(source)
    , m_source_viewer(source_viewer)
    , m_index(index)
{
    this->SetSkinBg(TBIDC("TBSelectItem"));
    this->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
    this->SetLayoutDistributionPosition(LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP);
    this->SetPaintOverflowFadeout(false);

    g_widgets_reader->LoadFile(this->GetContentRoot(), "ui:demo/ui_resources/test_list_item.tb.txt");
    TBCheckBox *checkbox = this->GetWidgetByIDAndType<TBCheckBox>(TBIDC("check"));
    TBTextField *name = this->GetWidgetByIDAndType<TBTextField>(TBIDC("name"));
    TBTextField *info = this->GetWidgetByIDAndType<TBTextField>(TBIDC("info"));
    checkbox->SetValue(item->GetChecked() ? true : false);
    name->SetText(item->str);
    info->SetText(item->GetMale() ? "Male" : "Female");
}

//------------------------------------------------------------------------------
bool
AdvancedItemWidget::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("check")) {
        AdvancedItem *item = m_source->GetItem(m_index);
        item->SetChecked(ev.target->GetValue() ? true : false);
        m_source->InvokeItemChanged(m_index, m_source_viewer);
        return true;
    }
    else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("delete")) {
        m_source->DeleteItem(m_index);
        return true;
    }
    return TBLayout::OnEvent(ev);
}

//==============================================================================
bool
AdvancedItemSource::Filter(int index, const char *filter) {
    // Override this method so we can return hits for our extra data too.
    if (TBSelectItemSource::Filter(index, filter)) {
        return true;
    }

    AdvancedItem *item = this->GetItem(index);
    return stristr(item->GetMale() ? "Male" : "Female", filter) ? true : false;
}

//------------------------------------------------------------------------------
TBWidget*
AdvancedItemSource::CreateItemWidget(int index, TBSelectItemViewer *viewer) {
    if (TBLayout *layout = new AdvancedItemWidget(this->GetItem(index), this, viewer, index)) {
        return layout;
    }
    return nullptr;
}

//==============================================================================
ListWindow::ListWindow(TBSelectItemSource *source) {
    this->LoadResourceFile(GetMainResource());
    if (TBSelectList *select = this->GetWidgetByIDAndType<TBSelectList>("list")) {
        select->SetSource(source);
        select->GetScrollContainer()->SetScrollMode(SCROLL_MODE_Y_AUTO);
    }
}

//------------------------------------------------------------------------------
URL
ListWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_select.tb.txt";
}

//------------------------------------------------------------------------------
bool
ListWindow::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CHANGED && ev.target->GetID() == TBIDC("filter")) {
        if (TBSelectList *select = GetWidgetByIDAndType<TBSelectList>("list")) {
            select->SetFilter(ev.target->GetText());
        }
        return true;
    }
    return TBUIWindow::OnEvent(ev);
}

//==============================================================================
AdvancedListWindow::AdvancedListWindow(AdvancedItemSource *source)
: m_source(source) {
    this->LoadResourceFile(GetMainResource());
    if (TBSelectList *select = GetWidgetByIDAndType<TBSelectList>("list")) {
        select->SetSource(source);
        select->GetScrollContainer()->SetScrollMode(SCROLL_MODE_X_AUTO_Y_AUTO);
    }
}

//------------------------------------------------------------------------------
URL
AdvancedListWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_select_advanced.tb.txt";
}

//------------------------------------------------------------------------------
Array<URL>
AdvancedListWindow::GetResources() {
    return Array<URL>({
        "ui:demo/ui_resources/test_select_advanced.tb.txt",
        "ui:demo/ui_resources/test_list_item.tb.txt",
    });
}

//------------------------------------------------------------------------------
bool
AdvancedListWindow::OnEvent(const TBWidgetEvent &ev)
{
    TBSelectList *select = this->GetWidgetByIDAndType<TBSelectList>("list");
    if (select && ev.type == EVENT_TYPE_CHANGED && ev.target->GetID() == TBIDC("filter")) {
        select->SetFilter(ev.target->GetText());
        return true;
    }
    else if (select && ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("add")) {
        TBStr name = GetTextByID(TBIDC("add_name"));
        if (!name.IsEmpty()) {
            m_source->AddItem(new AdvancedItem(name, TBIDC("boy_item"), true));
        }
        return true;
    }
    else if (select && ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("delete all")) {
        m_source->DeleteAllItems();
        return true;
    }
    return TBUIWindow::OnEvent(ev);
}

//==============================================================================
ResourceItem::ResourceItem(TBWidget *widget, const char *str)
    : TBGenericStringItem(str)
    , m_widget(widget) {
}

//==============================================================================
ResourceEditWindow::ResourceEditWindow()
    : m_widget_list(nullptr)
    , m_scroll_container(nullptr)
    , m_build_container(nullptr)
    , m_source_edit(nullptr) {
    
    // Register as global listener to intercept events in the build container
    TBWidgetListener::AddGlobalListener(this);

    g_widgets_reader->LoadFile(this, "ui:demo/ui_resources/resource_edit_window.tb.txt");

    m_scroll_container = GetWidgetByIDAndType<TBScrollContainer>(TBIDC("scroll_container"));
    m_build_container = m_scroll_container->GetContentRoot();
    m_source_edit = GetWidgetByIDAndType<TBEditField>(TBIDC("source_edit"));

    m_widget_list = GetWidgetByIDAndType<TBSelectList>(TBIDC("widget_list"));
    m_widget_list->SetSource(&m_widget_list_source);

    SetRect(TBRect(100, 50, 900, 600));
}

//------------------------------------------------------------------------------
ResourceEditWindow::~ResourceEditWindow() {
    TBWidgetListener::RemoveGlobalListener(this);

    // avoid assert
    m_widget_list->SetSource(nullptr);
}

//------------------------------------------------------------------------------
void
ResourceEditWindow::Load(const char *resource_file) {
    m_resource_filename.Set(resource_file);
    SetText(resource_file);

    // Set the text of the source view
    m_source_edit->SetText("");

    if (TBFile *file = TBFile::Open(m_resource_filename, TBFile::MODE_READ)) {
        TBTempBuffer buffer;
        if (buffer.Reserve(file->Size())) {
            uint32 size_read = file->Read(buffer.GetData(), 1, buffer.GetCapacity());
            m_source_edit->SetText(buffer.GetData(), size_read);
        }
        delete file;
    }
    else {
        // Error, show message
        TBStr text;
        text.SetFormatted("Could not load file %s", resource_file);
        if (TBMessageWindow *msg_win = new TBMessageWindow(GetParentRoot(), TBIDC(""))) {
            msg_win->Show("Error loading resource", text);
        }
    }

    RefreshFromSource();
}

//------------------------------------------------------------------------------
void
ResourceEditWindow::RefreshFromSource() {
    // Clear old widgets
    while (TBWidget *child = m_build_container->GetFirstChild()) {
        m_build_container->RemoveChild(child);
        delete child;
    }

    // Create new widgets from source
    g_widgets_reader->LoadData(m_build_container, m_source_edit->GetText());

    // Force focus back in case the edited resource has autofocus.
    // FIX: It would be better to prevent the focus change instead!
    m_source_edit->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
}

//------------------------------------------------------------------------------
void
ResourceEditWindow::UpdateWidgetList(bool immediately) {
    if (!immediately) {
        TBID id = TBIDC("update_widget_list");
        if (!GetMessageByID(id)) {
            PostMessage(id, nullptr);
        }
    }
    else {
        m_widget_list_source.DeleteAllItems();
        AddWidgetListItemsRecursive(m_build_container, 0);
        m_widget_list->InvalidateList();
    }
}

//------------------------------------------------------------------------------
void
ResourceEditWindow::AddWidgetListItemsRecursive(TBWidget *widget, int depth) {
    if (depth > 0) {
        // Ignore the root
        // Add a new ResourceItem for this widget
        TBStr str;
        const char *classname = widget->GetClassName();
        if (!*classname) {
            classname = "<Unknown widget type>";
        }
        str.SetFormatted("% *s%s", depth - 1, "", classname);

        if (ResourceItem *item = new ResourceItem(widget, str)) {
            m_widget_list_source.AddItem(item);
        }
    }
    for (TBWidget *child = widget->GetFirstChild(); child; child = child->GetNext()) {
        AddWidgetListItemsRecursive(child, depth + 1);
    }
}

//------------------------------------------------------------------------------
ResourceEditWindow::ITEM_INFO
ResourceEditWindow::GetItemFromWidget(TBWidget *widget) {
    ITEM_INFO item_info = { nullptr, -1 };
    for (int i = 0; i < m_widget_list_source.GetNumItems(); i++) {
        if (m_widget_list_source.GetItem(i)->GetWidget() == widget) {
            item_info.index = i;
            item_info.item = m_widget_list_source.GetItem(i);
            break;
        }
    }
    return item_info;
}

//------------------------------------------------------------------------------
void
ResourceEditWindow::SetSelectedWidget(TBWidget *widget) {
    m_selected_widget.Set(widget);
    ITEM_INFO item_info = GetItemFromWidget(widget);
    if (item_info.item) {
        m_widget_list->SetValue(item_info.index);
    }
}

//------------------------------------------------------------------------------
bool
ResourceEditWindow::OnEvent(const TBWidgetEvent &ev) {
    if (ev.type == EVENT_TYPE_CHANGED && ev.target->GetID() == TBIDC("widget_list_search")) {
        m_widget_list->SetFilter(ev.target->GetText());
        return true;
    }
    else if (ev.type == EVENT_TYPE_CHANGED && ev.target == m_widget_list) {
        if (m_widget_list->GetValue() >= 0 && m_widget_list->GetValue() < m_widget_list_source.GetNumItems())
            if (ResourceItem *item = m_widget_list_source.GetItem(m_widget_list->GetValue()))
                SetSelectedWidget(item->GetWidget());
    }
    else if (ev.type == EVENT_TYPE_CHANGED && ev.target == m_source_edit) {
        RefreshFromSource();
        return true;
    }
    else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("test")) {
        // Create a window containing the current layout, resize and center it.
        if (TBWindow *win = new TBWindow()) {
            win->SetText("Test window");
            g_widgets_reader->LoadData(win->GetContentRoot(), m_source_edit->GetText());
            TBRect bounds(0, 0, GetParent()->GetRect().w, GetParent()->GetRect().h);
            win->SetRect(win->GetResizeToFitContentRect().CenterIn(bounds).MoveIn(bounds).Clip(bounds));
            GetParent()->AddChild(win);
        }
        return true;
    }
    else if (ev.target->GetID() == TBIDC("constrained")) {
        m_scroll_container->SetAdaptContentSize(ev.target->GetValue() ? true : false);
        return true;
    }
    else if (ev.type == EVENT_TYPE_FILE_DROP) {
        return OnDropFileEvent(ev);
    }
    return TBUIWindow::OnEvent(ev);
}

//------------------------------------------------------------------------------
void
ResourceEditWindow::OnPaintChildren(const PaintProps &paint_props) {
    TBWindow::OnPaintChildren(paint_props);

    // Paint the selection of the selected widget
    if (TBWidget *selected_widget = GetSelectedWidget()) {
        TBRect widget_rect(0, 0, selected_widget->GetRect().w, selected_widget->GetRect().h);
        selected_widget->ConvertToRoot(widget_rect.x, widget_rect.y);
        ConvertFromRoot(widget_rect.x, widget_rect.y);
        g_renderer->DrawRect(widget_rect, TBColor(255, 205, 0));
    }
}

//------------------------------------------------------------------------------
void
ResourceEditWindow::OnMessageReceived(TBMessage *msg) {
    if (msg->message == TBIDC("update_widget_list")) {
        UpdateWidgetList(true);
    }
}

//------------------------------------------------------------------------------
bool
ResourceEditWindow::OnWidgetInvokeEvent(TBWidget *widget, const TBWidgetEvent &ev) {
    // Intercept all events to widgets in the build container
    if (m_build_container->IsAncestorOf(ev.target)) {
        // Let events through if alt is pressed so we can test some
        // functionality right in the editor (like toggle hidden UI).
        if (ev.modifierkeys & TB_ALT) {
            return false;
        }

        // Select widget when clicking
        if (ev.type == EVENT_TYPE_POINTER_DOWN) {
            SetSelectedWidget(ev.target);
        }

        if (ev.type == EVENT_TYPE_FILE_DROP) {
            OnDropFileEvent(ev);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
void
ResourceEditWindow::OnWidgetAdded(TBWidget *parent, TBWidget *child) {
    if (m_build_container && m_build_container->IsAncestorOf(child)) {
        UpdateWidgetList(false);
    }
}

//------------------------------------------------------------------------------
void
ResourceEditWindow::OnWidgetRemove(TBWidget *parent, TBWidget *child) {
    if (m_build_container && m_build_container->IsAncestorOf(child)) {
        UpdateWidgetList(false);
    }
}

//------------------------------------------------------------------------------
bool
ResourceEditWindow::OnDropFileEvent(const TBWidgetEvent &ev) {
    const TBWidgetEventFileDrop *fd_event = TBSafeCast<TBWidgetEventFileDrop>(&ev);
    if (fd_event->files.GetNumItems() > 0) {
        Load(*fd_event->files.Get(0));
    }
    return true;
}

