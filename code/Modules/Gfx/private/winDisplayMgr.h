#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::winDisplayMgr
    @ingroup _priv
    @brief common display manager functionality for Windows D3D11 renders

    NOTE: most of the window and messaging code is taken from GLFW3!
*/
#include "Gfx/private/displayMgrBase.h"
#include "Gfx/private/win/win_decl.h"
#include "Gfx/private/win/winInputDefs.h"

namespace Oryol {
namespace _priv {

class winDisplayMgr : public displayMgrBase {
public:
    /// constructor
    winDisplayMgr();
    /// destructor
    ~winDisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& gfxSetup, const gfxPointers& ptrs, const char* windowTitlePostfix);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// process window system events (call near start of frame)
    void ProcessSystemEvents();
    /// check whether the window system requests to quit the application
    bool QuitRequested() const;

    /// register the window class
    void registerWindowClass();
    /// unregister the window class
    void unregisterWindowClass();
    /// create the application window
    void createWindow(const char* titlePostFix);
    /// destroy the application window
    void destroyWindow();
    /// initialize DPI-related stuff
    void initDPI(bool highDpiRequested);

    /// compute actual window size from client rect size plus window chrome
    void computeWindowSize(int clientWidth, int clientHeight, int& outWidth, int& outHeight);
    /// setup the key translation table
    void setupKeyTranslationTable();

    /// set input mode (called from d3d11InputMgr)
    void setInputMode(int mode, int value);
    /// set cursor mode (called from setInputMode)
    void setCursorMode(int newMode);
    /// lowlevel set cursor mode
    void winSetCursorMode(int mode);
    /// lowlevel set cursor pos
    void winSetCursorPos(double xpos, double ypos);
    /// lowlevel get cursor pos
    void winGetCursorPos(double* xpos, double* ypos);
    /// lowlevel get window size
    void winGetWindowSize(int* width, int* height);
    /// update cursor clip rect
    void updateClipCursor();

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

    /// check if window size was changed, and track those changes
    void checkWindowResize();
    /// called from checkWindowResize when window did actually resize
    virtual void onWindowDidResize();

    static winDisplayMgr* self;

    bool quitRequested;
    HWND hwnd;
    DWORD dwStyle;
    DWORD dwExStyle;
    bool inCreateWindow;
    bool dpiAware;
    int windowScale;
    int contentScale;

    int cursorMode;
    double cursorPosX;
    double cursorPosY;
    double windowCursorPosX;
    double windowCursorPosY;
    int win32CursorPosX;
    int win32CursorPosY;
    bool cursorTracked;
    bool iconified;
    char mouseButtons[ORYOL_WIN_MOUSE_BUTTON_LAST + 1];
    char keys[ORYOL_WIN_KEY_LAST + 1];
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
