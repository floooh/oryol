#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11DisplayMgr
    @ingroup _priv
    @brief display manager implementation for D3D11

    NOTE: the guts of window management are taken from GLFW3!
*/
#include "Gfx/Core/displayMgrBase.h"

namespace Oryol {
namespace _priv {

class d3d11DisplayMgr : public displayMgrBase {
public:
    /// constructor
    d3d11DisplayMgr();
    /// destructor
    ~d3d11DisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& gfxSetup);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// process window system events (call near start of frame)
    void ProcessSystemEvents();
    /// present the current rendered frame
    void Present();
    /// check whether the window system requests to quit the application
    bool QuitRequested() const;

private:
    /// register the window class
    void registerWindowClass();
    /// unregister the window class
    void unregisterWindowClass();
    /// create the application window
    void createWindow(const GfxSetup& gfxSetup);
    /// destroy the application window
    void destroyWindow();
    /// compute actual window size from client rect size plus window chrome
    void computeWindowSize(int clientWidth, int clientHeight, int& outWidth, int& outHeight);
    /// custom winproc
    static LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool quitRequested;
    HWND hwnd;
    DWORD dwStyle;
    DWORD dwExStyle;
};

} // namespace _priv
} // namespace Oryol

