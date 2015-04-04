#pragma once
//------------------------------------------------------------------------------
/**
    @class MainWindow
    @brief main window class of TurboBadgerDemo
*/
#include "TBUI/TBUIWindow.h"

class MainWindow : public Oryol::TBUIWindow {
    OryolClassDecl(MainWindow);
public:
    
protected:
    /// get the URL of the main resource file
    virtual Oryol::URL MainResource() const;
};

