#pragma once
//------------------------------------------------------------------------------
/**
    @class MainWindow
    @brief main window class of TurboBadgerDemo
*/
#include "TBUI/TBUIWindow.h"
#include "tb_msg.h"

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

