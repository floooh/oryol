#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11DisplayMgr
    @ingroup _priv
    @brief display manager implementation for D3D11

    NOTE: the guts of window management are taken from GLFW3!
*/
#include "Gfx/Core/displayMgrBase.h"
#include "Core/Assertion.h"
#include "Gfx/d3d11/d3d11_decl.h"
#include "Gfx/d3d11/d3d11InputDefs.h"

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

    /// pointer to d3d11 device
    ID3D11Device* d3d11Device;
    /// pointer to immediate mode device context
    ID3D11DeviceContext* d3d11DeviceContext;
    /// pointer to swap chain
    IDXGISwapChain* dxgiSwapChain;
    /// pointer to default render target back buffer texture
    ID3D11Texture2D* backBuffer;
    /// pointer to default render target view
    ID3D11RenderTargetView* renderTargetView;
    /// pointer to default depth/stencil buffer texture
    ID3D11Texture2D* depthStencilBuffer;
    /// pointer to default default depth/stencil view
    ID3D11DepthStencilView* depthStencilView;

    /// register the window class
    void registerWindowClass();
    /// unregister the window class
    void unregisterWindowClass();
    /// create the application window
    void createWindow(const GfxSetup& gfxSetup);
    /// destroy the application window
    void destroyWindow();
    /// create swap chain and d3d device
    void createDeviceAndSwapChain(const GfxSetup& gfxSetup, DXGI_SWAP_CHAIN_DESC& outSwapChainDesc);
    /// destroy the d3d device and swap chain
    void destroyDeviceAndSwapChain();
    /// create the default render target
    void createDefaultRenderTarget(const GfxSetup& gfxSetup, const DXGI_SWAP_CHAIN_DESC& swapChainDesc);
    /// destroy the default render target
    void destroyDefaultRenderTarget();
    /// compute actual window size from client rect size plus window chrome
    void computeWindowSize(int clientWidth, int clientHeight, int& outWidth, int& outHeight);

    /// setup the key translation table
    void setupKeyTranslationTable();
    /// set input mode (called from d3d11InputMgr)
    void setInputMode(int mode, int value);
    /// set cursor mode (called from setInputMode)
    void setCursorMode(int newMode);
    /// apply the current cursor mode
    void applyCursorMode();
    /// restore the mouse cursor
    void restoreCursor();
    /// hide the mouse cursor
    void hideCursor();
    /// disable the mouse cursor
    void disableCursor();
    /// update cursor clip rect
    void updateClipRect();
    /// get modifier keys
    int inputGetKeyMods();
    /// translate key code from WM wParam, lParam
    int inputTranslateKey(WPARAM wParam, LPARAM lParam);
    /// called from WinProc when key has been pressed
    void inputKey(int keyCode, int scanCode, int action, int mods);
    /// called from WinProc on WM_CHAR
    void inputChar(unsigned int codePoint, int mods, int plain);
    // called from WinProc on WM mouse button messages
    void inputMouseClick(int button, int action, int mods);
    /// called from WinProc window focus messages
    void inputWindowFocus(bool focused);
    /// called on mouse moved
    void inputCursorMotion(double x, double y);
    /// cursor enters/leaves windows, called from winproc
    void inputCursorEnter(bool entered);
    /// called from winproc on mouse wheel
    void inputScroll(double x, double y);
    /// called on WM_SIZE
    void inputFramebufferSize(int width, int height);
    /// called on WM_SIZE
    void inputWindowSize(int width, int height);
    /// called on WM_MOVE
    void inputWindowPos(int xpos, int ypos);
    /// window has been iconified/restored
    void inputWindowIconify(bool iconified);
    
    static d3d11DisplayMgr* self;
    bool quitRequested;
    HWND hwnd;
    DWORD dwStyle;
    DWORD dwExStyle;

    int cursorMode;
    double cursorPosX;
    double cursorPosY;
    bool cursorInside;
    bool iconified;
    char mouseButtons[ORYOL_D3D11_MOUSE_BUTTON_LAST + 1];
    char keys[ORYOL_D3D11_KEY_LAST + 1];
    short int publicKeys[512];  // key-code translation table

    // callback signatures (see glfw3.h)
    typedef void(*windowposfun)(int, int);
    typedef void(*windowsizefun)(int, int);
    typedef void(*windowclosefun)();
    typedef void(*windowrefreshfun)();
    typedef void(*windowfocusfun)(int);
    typedef void(*windowiconifyfun)(int);
    typedef void(*framebuffersizefun)(int, int);
    typedef void(*mousebuttonfun)(int, int, int);
    typedef void(*cursorposfun)(double, double);
    typedef void(*cursorenterfun)(int);
    typedef void(*scrollfun)(double, double);
    typedef void(*keyfun)(int, int, int, int);
    typedef void(*charfun)(unsigned int);
    typedef void(*charmodsfun)(unsigned int, int);

    // callback pointers, these are usually populated by the Oryol Input module
    struct callbackTable {
        windowposfun        pos;
        windowsizefun       size;
        windowclosefun      close;
        windowrefreshfun    refresh;
        windowfocusfun      focus;
        windowiconifyfun    iconify;
        framebuffersizefun  fbsize;
        mousebuttonfun      mouseButton;
        cursorposfun        cursorPos;
        cursorenterfun      cursorEnter;
        scrollfun           scroll;
        keyfun              key;
        charfun             character;
        charmodsfun         charmods;
    } callbacks;
};

} // namespace _priv
} // namespace Oryol

