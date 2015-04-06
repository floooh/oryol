#pragma once
//------------------------------------------------------------------------------
/**
    @class MainWindow
    @brief main window class of TurboBadgerDemo
*/
#include "TBUI/TBUIWindow.h"
#include "tb_msg.h"
#include "tb_scroller.h"

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

