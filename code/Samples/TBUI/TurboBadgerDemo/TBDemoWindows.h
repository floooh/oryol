#pragma once
//------------------------------------------------------------------------------
/**
    @class MainWindow
    @brief main window class of TurboBadgerDemo
*/
#include "TBUI/TBUIWindow.h"
#include "tb_msg.h"
#include "tb_scroller.h"
#include "tb_select.h"

//------------------------------------------------------------------------------
class MainWindow : public Oryol::TBUIWindow, public tb::TBMessageHandler {
public:
    /// constructor
    MainWindow();
    /// get the URL of the main resource file
    static Oryol::URL GetMainResource();
    
    /// widget event callback
    virtual bool OnEvent(const tb::TBWidgetEvent &ev);
    /// message handler callback
    virtual void OnMessageReceived(tb::TBMessage *msg);
};

//------------------------------------------------------------------------------
class ImageWindow : public Oryol::TBUIWindow {
public:
    /// constructor
    ImageWindow();
    /// get the URL of the main resource file
    static Oryol::URL GetMainResource();

    /// event handler callback
    virtual bool OnEvent(const tb::TBWidgetEvent &ev);
};

//------------------------------------------------------------------------------
class PageWindow : public Oryol::TBUIWindow, public tb::TBScrollerSnapListener {
public:
    /// constructor
    PageWindow();
    /// get the URL of the main resource file
    static Oryol::URL GetMainResource();
    
    /// scroll snap callback
    virtual void OnScrollSnap(tb::TBWidget *target_widget, int &target_x, int &target_y);
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
    virtual bool OnEvent(const tb::TBWidgetEvent &ev);
};

//------------------------------------------------------------------------------
class TabContainerWindow : public Oryol::TBUIWindow {
public:
    /// constructor
    TabContainerWindow();
    /// get the main resource file URL
    static Oryol::URL GetMainResource();
    
    /// event callback
    virtual bool OnEvent(const tb::TBWidgetEvent &ev);
};

//------------------------------------------------------------------------------
class AdvancedItemSource;

/** Shows a list of items from *any* type of source. */
class ListWindow : public Oryol::TBUIWindow {
public:
    ListWindow(tb::TBSelectItemSource *source);
    static Oryol::URL GetMainResource();
    virtual bool OnEvent(const tb::TBWidgetEvent &ev);
};

/** Shows a list of items from a source of type AdvancedItemSource. */
class AdvancedListWindow : public Oryol::TBUIWindow {
public:
    AdvancedListWindow(AdvancedItemSource *source);
    static Oryol::URL GetMainResource();
    static Oryol::Array<Oryol::URL> GetResources();    
    virtual bool OnEvent(const tb::TBWidgetEvent &ev);
private:
    AdvancedItemSource *m_source;
};

/** AdvancedItem adds extra info to a string item. */
class AdvancedItem : public tb::TBGenericStringItem
{
public:
    AdvancedItem(const char *str, const tb::TBID &id, bool male)
        : TBGenericStringItem(str, id)
        , m_checked(false)
        , m_male(male) {}
    void SetChecked(bool checked) { m_checked = checked; }
    bool GetChecked() const { return m_checked; }
    bool GetMale() const { return m_male; }
private:
    tb::TBStr m_info;
    bool m_checked;
    bool m_male;
};

/** AdvancedItemSource provides items of type AdvancedItem and makes sure
    the viewer is populated with the customized widget for each item. */
class AdvancedItemSource : public tb::TBSelectItemSourceList<AdvancedItem>
{
public:
    virtual bool Filter(int index, const char *filter);
    virtual tb::TBWidget *CreateItemWidget(int index, tb::TBSelectItemViewer *viewer);
};

/** AdvancedItemWidget is the widget representing a AdvancedItem.
    On changes to the item, it calls InvokeItemChanged on the source, so that all
    viewers of the source are updated to reflect the change. */
class AdvancedItemWidget : public tb::TBLayout
{
public:
    AdvancedItemWidget(AdvancedItem *item, AdvancedItemSource *source, tb::TBSelectItemViewer *source_viewer, int index);
    virtual bool OnEvent(const tb::TBWidgetEvent &ev);
private:
    AdvancedItemSource *m_source;
    tb::TBSelectItemViewer *m_source_viewer;
    int m_index;
};
