#pragma once
//------------------------------------------------------------------------------
/**
*/
#include "TBUI/TBUIWindow.h"
#include "tb_msg.h"
#include "tb_scroller.h"
#include "tb_select.h"
#include "tb_editfield.h"
#include "tb_menu_window.h"
#include "utf8/utf8.h"
#include "tb_font_renderer.h"

namespace tb {

//------------------------------------------------------------------------------
class MainWindow : public Oryol::TBUIWindow, public TBMessageHandler {
public:
    /// constructor
    MainWindow();
    /// get the URL of the main resource file
    static Oryol::URL GetMainResource();
    
    /// widget event callback
    virtual bool OnEvent(const TBWidgetEvent &ev);
    /// message handler callback
    virtual void OnMessageReceived(TBMessage *msg);
};

//------------------------------------------------------------------------------
class ImageWindow : public Oryol::TBUIWindow {
public:
    /// constructor
    ImageWindow();
    /// get the URL of the main resource file
    static Oryol::URL GetMainResource();

    /// event handler callback
    virtual bool OnEvent(const TBWidgetEvent &ev);
};

//------------------------------------------------------------------------------
class PageWindow : public Oryol::TBUIWindow, public TBScrollerSnapListener {
public:
    /// constructor
    PageWindow();
    /// get the URL of the main resource file
    static Oryol::URL GetMainResource();
    
    /// scroll snap callback
    virtual void OnScrollSnap(TBWidget *target_widget, int &target_x, int &target_y);
};

//------------------------------------------------------------------------------
class AnimationsWindow : public Oryol::TBUIWindow {
public:
    /// constructor
    AnimationsWindow();
    /// get the main resource file URL
    static Oryol::URL GetMainResource();
    
    /// (re-)start animation
    void Animate();
    /// event callback
    virtual bool OnEvent(const TBWidgetEvent &ev);
};

//------------------------------------------------------------------------------
class TabContainerWindow : public Oryol::TBUIWindow {
public:
    /// constructor
    TabContainerWindow();
    /// get the main resource file URL
    static Oryol::URL GetMainResource();
    /// event callback
    virtual bool OnEvent(const TBWidgetEvent &ev);
};

//------------------------------------------------------------------------------
class LayoutWindow : public Oryol::TBUIWindow {
public:
    /// constructor
    LayoutWindow(const char *filename);
    /// event callback
    virtual bool OnEvent(const TBWidgetEvent &ev);
};

//------------------------------------------------------------------------------
class ConnectionWindow : public Oryol::TBUIWindow {
public:
    /// constructor
    ConnectionWindow();
    /// get the main resource file URL
    static Oryol::URL GetMainResource();
    /// event callback
    virtual bool OnEvent(const TBWidgetEvent &ev);
};

//------------------------------------------------------------------------------
class ScrollContainerWindow : public Oryol::TBUIWindow, public TBMessageHandler {
public:
    /// constructor
    ScrollContainerWindow();
    /// get the main resource file URL
    static Oryol::Array<Oryol::URL> GetMainResources();
    /// event callback
    virtual bool OnEvent(const TBWidgetEvent &ev);
    // Implement TBMessageHandler
    virtual void OnMessageReceived(TBMessage *msg);
};

//------------------------------------------------------------------------------
class AdvancedItemSource;

/** Shows a list of items from *any* type of source. */
class ListWindow : public Oryol::TBUIWindow {
public:
    ListWindow(TBSelectItemSource *source);
    static Oryol::URL GetMainResource();
    virtual bool OnEvent(const TBWidgetEvent &ev);
};

/** Shows a list of items from a source of type AdvancedItemSource. */
class AdvancedListWindow : public Oryol::TBUIWindow {
public:
    AdvancedListWindow(AdvancedItemSource *source);
    static Oryol::URL GetMainResource();
    static Oryol::Array<Oryol::URL> GetResources();    
    virtual bool OnEvent(const TBWidgetEvent &ev);
private:
    AdvancedItemSource *m_source;
};

/** AdvancedItem adds extra info to a string item. */
class AdvancedItem : public TBGenericStringItem
{
public:
    AdvancedItem(const char *str, const TBID &id, bool male)
        : TBGenericStringItem(str, id)
        , m_checked(false)
        , m_male(male) {}
    void SetChecked(bool checked) { m_checked = checked; }
    bool GetChecked() const { return m_checked; }
    bool GetMale() const { return m_male; }
private:
    TBStr m_info;
    bool m_checked;
    bool m_male;
};

/** AdvancedItemSource provides items of type AdvancedItem and makes sure
    the viewer is populated with the customized widget for each item. */
class AdvancedItemSource : public TBSelectItemSourceList<AdvancedItem>
{
public:
    virtual bool Filter(int index, const char *filter);
    virtual TBWidget *CreateItemWidget(int index, TBSelectItemViewer *viewer);
};

/** AdvancedItemWidget is the widget representing a AdvancedItem.
    On changes to the item, it calls InvokeItemChanged on the source, so that all
    viewers of the source are updated to reflect the change. */
class AdvancedItemWidget : public TBLayout
{
public:
    AdvancedItemWidget(AdvancedItem *item, AdvancedItemSource *source, TBSelectItemViewer *source_viewer, int index);
    virtual bool OnEvent(const TBWidgetEvent &ev);
private:
    AdvancedItemSource *m_source;
    TBSelectItemViewer *m_source_viewer;
    int m_index;
};

//==============================================================================
class ResourceItem : public TBGenericStringItem {
public:
    ResourceItem(TBWidget *widget, const char *str);
    TBWidget *GetWidget() { return m_widget; }
private:
    TBWidget *m_widget;
};

class ResourceEditWindow : public Oryol::TBUIWindow, public TBMessageHandler, public TBWidgetListener {
public:
    // For safe typecasting
    TBOBJECT_SUBCLASS(ResourceEditWindow, TBWindow);

    ResourceEditWindow();
    ~ResourceEditWindow();

    void UpdateWidgetList(bool immediately);

    struct ITEM_INFO {
        ResourceItem *item;
        int index;
    };
    ITEM_INFO GetItemFromWidget(TBWidget *widget);
    TBWidget *GetSelectedWidget() { return m_selected_widget.Get(); }
    void SetSelectedWidget(TBWidget *widget);

    void Load(const char *resource_file);
    void RefreshFromSource();

    // == TBWindow ======================================================================
    virtual bool OnEvent(const TBWidgetEvent &ev);
    virtual void OnPaintChildren(const PaintProps &paint_props);

    // == TBMessageHandler ==============================================================
    virtual void OnMessageReceived(TBMessage *msg);

    // == TBWidgetListener ========================================================
    virtual bool OnWidgetInvokeEvent(TBWidget *widget, const TBWidgetEvent &ev);
    virtual void OnWidgetAdded(TBWidget *parent, TBWidget *child);
    virtual void OnWidgetRemove(TBWidget *parent, TBWidget *child);
private:
    TBSelectList *m_widget_list;
    TBSelectItemSourceList<ResourceItem> m_widget_list_source;
    TBScrollContainer *m_scroll_container;
    TBWidget *m_build_container;
    TBEditField *m_source_edit;
    TBStr m_resource_filename;
    TBWidgetSafePointer m_selected_widget;
    void AddWidgetListItemsRecursive(TBWidget *widget, int depth);
    bool OnDropFileEvent(const TBWidgetEvent &ev);
};

// == EditWindow ==============================================================

class EditWindow : public Oryol::TBUIWindow {
public:
    EditWindow() {
        LoadResourceFile(GetMainResource());
    }
    static Oryol::URL GetMainResource() {
        return "ui:demo/ui_resources/test_textwindow.tb.txt";
    }
    static Oryol::Array<Oryol::URL> GetResources() {
        return Oryol::Array<Oryol::URL>({
            GetMainResource(),
            "ui:default_font/segoe_white_with_shadow_28.png",
            "ui:demo/fonts/neon_42.png",
            "ui:demo/fonts/orangutang_48.png",
            "ui:demo/fonts/orange_30.png"
        });
    }
    virtual void OnProcessStates() {
        // Update the disabled state of undo/redo buttons, and caret info.
        if (TBEditField *edit = GetWidgetByIDAndType<TBEditField>(TBIDC("editfield"))) {
            if (TBWidget *undo = GetWidgetByID("undo")) {
                undo->SetState(WIDGET_STATE_DISABLED, !edit->GetStyleEdit()->CanUndo());
            }
            if (TBWidget *redo = GetWidgetByID("redo")) {
                redo->SetState(WIDGET_STATE_DISABLED, !edit->GetStyleEdit()->CanRedo());
            }
            if (TBTextField *info = GetWidgetByIDAndType<TBTextField>(TBIDC("info"))) {
                TBStr text;
                text.SetFormatted("Caret ofs: %d", edit->GetStyleEdit()->caret.GetGlobalOfs());
                info->SetText(text);
            }
        }
    }
	virtual bool OnEvent(const TBWidgetEvent &ev) {
        if (ev.type == EVENT_TYPE_CLICK) {
            TBEditField *edit = GetWidgetByIDAndType<TBEditField>(TBIDC("editfield"));
            if (!edit) {
                return false;
            }
            if (ev.target->GetID() == TBIDC("clear")) {
                edit->SetText("");
                return true;
            }
            else if (ev.target->GetID() == TBIDC("undo")) {
                edit->GetStyleEdit()->Undo();
                return true;
            }
            else if (ev.target->GetID() == TBIDC("redo")) {
                edit->GetStyleEdit()->Redo();
                return true;
            }
            else if (ev.target->GetID() == TBIDC("menu")) {
                static TBGenericStringItemSource source;
                if (!source.GetNumItems()) {
                    source.AddItem(new TBGenericStringItem("Default font", TBIDC("default font")));
                    source.AddItem(new TBGenericStringItem("Default font (larger)", TBIDC("large font")));
                    source.AddItem(new TBGenericStringItem("RGB font (Neon)", TBIDC("rgb font Neon")));
                    source.AddItem(new TBGenericStringItem("RGB font (Orangutang)", TBIDC("rgb font Orangutang")));
                    source.AddItem(new TBGenericStringItem("RGB font (Orange)", TBIDC("rgb font Orange")));
                    source.AddItem(new TBGenericStringItem("-"));
                    source.AddItem(new TBGenericStringItem("Glyph cache stresstest (CJK)", TBIDC("CJK")));
                    source.AddItem(new TBGenericStringItem("-"));
                    source.AddItem(new TBGenericStringItem("Toggle wrapping", TBIDC("toggle wrapping")));
                    source.AddItem(new TBGenericStringItem("-"));
                    source.AddItem(new TBGenericStringItem("Align left", TBIDC("align left")));
                    source.AddItem(new TBGenericStringItem("Align center", TBIDC("align center")));
                    source.AddItem(new TBGenericStringItem("Align right", TBIDC("align right")));
                }

                if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("popup_menu"))) {
                    menu->Show(&source, TBPopupAlignment());
                }
                return true;
            }
            else if (ev.target->GetID() == TBIDC("popup_menu")) {
                if (ev.ref_id == TBIDC("default font")) {
                    edit->SetFontDescription(TBFontDescription());
                }
                else if (ev.ref_id == TBIDC("large font")) {
                    TBFontDescription fd = g_font_manager->GetDefaultFontDescription();
                    fd.SetSize(28);
                    edit->SetFontDescription(fd);
                }
                else if (ev.ref_id == TBIDC("rgb font Neon")) {
                    TBFontDescription fd = edit->GetCalculatedFontDescription();
                    fd.SetID(TBIDC("Neon"));
                    edit->SetFontDescription(fd);
                }
                else if (ev.ref_id == TBIDC("rgb font Orangutang")) {
                    TBFontDescription fd = edit->GetCalculatedFontDescription();
                    fd.SetID(TBIDC("Orangutang"));
                    edit->SetFontDescription(fd);
                }
                else if (ev.ref_id == TBIDC("rgb font Orange")) {
                    TBFontDescription fd = edit->GetCalculatedFontDescription();
                    fd.SetID(TBIDC("Orange"));
                    edit->SetFontDescription(fd);
                }
                else if (ev.ref_id == TBIDC("CJK")) {
                    TBTempBuffer buf;
                    for (int i = 0, cp = 0x4E00; cp <= 0x9FCC; cp++, i++) {
                        char utf8[8];
                        int len = utf8::encode(cp, utf8);
                        buf.Append(utf8, len);
                        if (i % 64 == 63)
                            buf.Append("\n", 1);
                    }
                    edit->GetStyleEdit()->SetText(buf.GetData(), buf.GetAppendPos());
                }
                else if (ev.ref_id == TBIDC("toggle wrapping")) {
                    edit->SetWrapping(!edit->GetWrapping());
                }
                else if (ev.ref_id == TBIDC("align left")) {
                    edit->SetTextAlign(TB_TEXT_ALIGN_LEFT);
                }
                else if (ev.ref_id == TBIDC("align center")) {
                    edit->SetTextAlign(TB_TEXT_ALIGN_CENTER);
                }
                else if (ev.ref_id == TBIDC("align right")) {
                    edit->SetTextAlign(TB_TEXT_ALIGN_RIGHT);
                }
                return true;
            }
        }
        return TBUIWindow::OnEvent(ev);
    }
};

} // namespace tb