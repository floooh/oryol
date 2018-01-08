//------------------------------------------------------------------------------
//  winDisplayMgr.cc
//  NOTE: window management and input handling is largely taken from GLFW!
//------------------------------------------------------------------------------
#include "Pre.h"
#include "winDisplayMgr.h"
#include "Core/String/StringBuilder.h"
#include "Core/String/StringConverter.h"

#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <windowsx.h>

#ifndef DPI_ENUMS_DECLARED
typedef enum PROCESS_DPI_AWARENESS
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
#endif /*DPI_ENUMS_DECLARED*/

namespace Oryol {
namespace _priv {

static LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
winDisplayMgr* winDisplayMgr::self = nullptr;

//------------------------------------------------------------------------------
winDisplayMgr::winDisplayMgr() :
quitRequested(false),
hwnd(0),
dwStyle(0),
dwExStyle(0),
inCreateWindow(false),
dpiAware(false),
windowScale(1),
contentScale(1),
cursorMode(ORYOL_WIN_CURSOR_NORMAL),
cursorPosX(0.0),
cursorPosY(0.0),
windowCursorPosX(0.0),
windowCursorPosY(0.0),
win32CursorPosX(0),
win32CursorPosY(0),
cursorTracked(false),
iconified(false) {
    self = this;
    Memory::Clear(this->mouseButtons, sizeof(mouseButtons));
    Memory::Clear(this->keys, sizeof(keys));
    Memory::Clear(&this->callbacks, sizeof(this->callbacks));
    this->setupKeyTranslationTable();
}

//------------------------------------------------------------------------------
winDisplayMgr::~winDisplayMgr() {
    o_assert_dbg(!this->IsDisplayValid());
    self = nullptr;
}

//------------------------------------------------------------------------------
void
winDisplayMgr::SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs, const char* windowTitlePostfix) {
    o_assert(!this->IsDisplayValid());

    displayMgrBase::SetupDisplay(setup, ptrs);

    this->initDPI(setup.HighDPI);
    this->registerWindowClass();
    this->createWindow(windowTitlePostfix);

    // get actual size and update display attrs
    int actWidth, actHeight;
    this->winGetWindowSize(&actWidth, &actHeight);
    this->displayAttrs.WindowWidth = int(actWidth / this->windowScale);
    this->displayAttrs.WindowHeight = int(actHeight / this->windowScale);
    this->displayAttrs.FramebufferWidth = int(this->displayAttrs.WindowWidth * this->contentScale);
    this->displayAttrs.FramebufferHeight = int(this->displayAttrs.WindowHeight *  this->contentScale);
    this->curFramebufferWidth = this->displayAttrs.FramebufferWidth;
    this->curFramebufferHeight = this->displayAttrs.FramebufferHeight;
}

//------------------------------------------------------------------------------
void
winDisplayMgr::DiscardDisplay() {
    o_assert_dbg(this->IsDisplayValid());

    this->destroyWindow();
    this->unregisterWindowClass();

    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
bool
winDisplayMgr::QuitRequested() const {
    return this->quitRequested;
}

//------------------------------------------------------------------------------
void
winDisplayMgr::ProcessSystemEvents() {

    MSG msg;
    while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (WM_QUIT == msg.message) {
            this->quitRequested = true;
        }
        else {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
    }

    if (this->cursorMode == ORYOL_WIN_CURSOR_DISABLED) {
        int width, height;
        this->winGetWindowSize(&width, &height);
        if ((this->win32CursorPosX != width/2) || (this->win32CursorPosY != height/2)) {
            this->winSetCursorPos(width/2, height/2);
        }
    }

    // FIXME: there's some more LSHIFT/RSHIFT related stuff in GLFW, 
    // see _glfwPlatformPollEvents in win32_window.c

    // track window size changes
    this->checkWindowResize();

    displayMgrBase::ProcessSystemEvents();
}

//------------------------------------------------------------------------------
void
winDisplayMgr::registerWindowClass() {

    WNDCLASSW wc;
    Memory::Clear(&wc, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)winProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = ::GetModuleHandleW(NULL);
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"ORYOL";

    // load user-provided icon if available (use the same name like GLFW)
    wc.hIcon = ::LoadIconW(GetModuleHandleW(NULL), L"GLFW_ICON");
    if (!wc.hIcon) {
        // no user-provided icon, use default icon
        wc.hIcon = ::LoadIconW(NULL, IDI_WINLOGO);
    }
    if (!::RegisterClassW(&wc)) {
        o_error("d3d11DisplayMgr: failed to register window class!\n");
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::unregisterWindowClass() {
    ::UnregisterClassW(L"ORYOL", ::GetModuleHandleW(NULL));
}

//------------------------------------------------------------------------------
void
winDisplayMgr::createWindow(const char* titlePostFix) {
    o_assert_dbg(0 == this->hwnd);
    o_assert_dbg(titlePostFix);

    // this is a flag for our WinProc message handling that we are currently
    // within window setup, WinProc will then ignore some messages (for instance
    // the initial WM_SIZE, which may report a different window size of the
    // initial window doesn't fit the screen
    this->inCreateWindow = true;

    // setup window style flags
    this->dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    this->dwExStyle = WS_EX_APPWINDOW;
    if (this->gfxSetup.Windowed) {
        this->dwStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
        this->dwExStyle |= WS_EX_WINDOWEDGE;
    }
    else {
        this->dwStyle |= WS_POPUP;
    }

    int width, height;
    this->computeWindowSize(this->gfxSetup.Width, this->gfxSetup.Height, width, height);

    StringBuilder strBuilder(this->gfxSetup.Title);
    strBuilder.Append(titlePostFix);
    WideString title = StringConverter::UTF8ToWide(strBuilder.AsCStr());

    this->hwnd = ::CreateWindowExW(this->dwExStyle,   // dwExStyle 
        L"ORYOL",          // lpClassName
        title.AsCStr(),    // lpWindowName
        this->dwStyle,     // dwStyle
        CW_USEDEFAULT,     // X
        CW_USEDEFAULT,     // Y
        width,             // nWidth
        height,            // nHeight
        NULL,              // hWndParent
        NULL,              // hMenu
        GetModuleHandleW(NULL),    // hInstance
        NULL);
    if (!this->hwnd) {
        o_error("Failed to create app window");
    }
    ::ShowWindow(this->hwnd, SW_SHOW);
    ::BringWindowToTop(this->hwnd);
    ::SetForegroundWindow(this->hwnd);
    ::SetFocus(this->hwnd);

    this->inCreateWindow = false;
}

//------------------------------------------------------------------------------
void
winDisplayMgr::destroyWindow() {
    o_assert_dbg(this->hwnd);

    ::DestroyWindow((HWND)this->hwnd);
    this->hwnd = 0;
}

//------------------------------------------------------------------------------
void
winDisplayMgr::initDPI(bool highDpiRequested) {
    o_assert(!this->dpiAware);

    // there's a new, and an old API to tell Windows that we are DPI aware, 
    // try to call both (see GLFW)
    typedef BOOL(WINAPI * SETPROCESSDPIAWARE_T)(void);
    SETPROCESSDPIAWARE_T dpiAwareProc = 0;
    typedef HRESULT(WINAPI * SETPROCESSDPIAWARENESS_T)(PROCESS_DPI_AWARENESS);
    SETPROCESSDPIAWARENESS_T dpiAwarenessProc = 0;
    typedef HRESULT(WINAPI * GETDPIFORMONITOR_T)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
    GETDPIFORMONITOR_T getDpiForMonitorProc = 0;

    HINSTANCE hUser32 = ::LoadLibraryA("user32.dll");
    if (hUser32) {
        dpiAwareProc = (SETPROCESSDPIAWARE_T) ::GetProcAddress(hUser32, "SetProcessDPIAware");
    }
    HINSTANCE hShCore = ::LoadLibraryA("shcore.dll");
    if (hShCore) {
        dpiAwarenessProc = (SETPROCESSDPIAWARENESS_T) ::GetProcAddress(hShCore, "SetProcessDpiAwareness");
        getDpiForMonitorProc = (GETDPIFORMONITOR_T) ::GetProcAddress(hShCore, "GetDpiForMonitor");
    }
    if (dpiAwarenessProc) {
        dpiAwarenessProc(PROCESS_SYSTEM_DPI_AWARE);
        this->dpiAware = true;
    }
    else if (dpiAwareProc) {
        dpiAwareProc();
        this->dpiAware = true;
    }

    // get dpi scale factor for main monitor
    if (getDpiForMonitorProc) {
        POINT pt = { 1, 1 };
        HMONITOR hm = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        UINT dpix, dpiy;
        HRESULT hr = getDpiForMonitorProc(hm, MDT_EFFECTIVE_DPI, &dpix, &dpiy);
        o_assert(SUCCEEDED(hr));
        // don't allow fractional scales
        this->windowScale = int(float(dpix) / 96.0f);
        o_assert(this->windowScale >= 1);
    }
    else {
        this->windowScale = 1;
    }
    if (highDpiRequested) {
        this->contentScale = this->windowScale;
    }
    else {
        this->contentScale = 1;
    }

    if (hUser32) {
        ::FreeLibrary(hUser32);
    }
    if (hShCore) {
        ::FreeLibrary(hShCore);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::computeWindowSize(int clientWidth, int clientHeight, int& outWidth, int& outHeight) {
    RECT rect = { 0, 0, 
        clientWidth * this->windowScale, 
        clientHeight * this->windowScale };
    ::AdjustWindowRectEx(&rect, this->dwStyle, FALSE, this->dwExStyle);
    outWidth = rect.right - rect.left;
    outHeight = rect.bottom - rect.top;
}

//------------------------------------------------------------------------------
void
winDisplayMgr::checkWindowResize() {
    o_assert(this->hwnd);
    RECT curClientRect;
    ::GetClientRect(this->hwnd, &curClientRect);
    const int crWidth = (curClientRect.right - curClientRect.left);
    const int crHeight = (curClientRect.bottom - curClientRect.top);
    const int windowWidth = crWidth / this->windowScale;
    const int windowHeight = crHeight / this->windowScale;

    // NOTE: this method is not called when minimized, or restored from minimized
    if ((windowWidth != this->displayAttrs.WindowWidth) || (windowHeight != this->displayAttrs.WindowHeight)) {

        this->displayAttrs.WindowWidth = windowWidth;
        this->displayAttrs.WindowHeight = windowHeight;
        this->displayAttrs.FramebufferWidth = int(windowWidth * this->contentScale);
        this->displayAttrs.FramebufferHeight = int(windowHeight * this->contentScale);

        this->inputFramebufferSize(crWidth, crHeight);
        this->inputWindowSize(crWidth, crHeight);
        this->onWindowDidResize();
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::onWindowDidResize() {
    // this is called from windowResize when the window size has actually
    // changed, can be overriden in subclass to react to window size changes
}

//------------------------------------------------------------------------------
int
winDisplayMgr::inputGetKeyMods() {
    int mods = 0;
    if (::GetKeyState(VK_SHIFT) & (1 << 31)) {
        mods |= ORYOL_WIN_MOD_SHIFT;
    }
    if (::GetKeyState(VK_CONTROL) & (1 << 31)) {
        mods |= ORYOL_WIN_MOD_CONTROL;
    }
    if (::GetKeyState(VK_MENU) & (1 << 31)) {
        mods |= ORYOL_WIN_MOD_ALT;
    }
    if ((::GetKeyState(VK_LWIN) | ::GetKeyState(VK_RWIN)) & (1 << 31)) {
        mods |= ORYOL_WIN_MOD_SUPER;
    }
    return mods;
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputKey(int key, int scancode, int action, int mods) {
    if ((key >= 0) && (key <= ORYOL_WIN_KEY_LAST)) {
        bool repeated = false;
        if ((action == ORYOL_WIN_RELEASE) && (self->keys[key] == ORYOL_WIN_RELEASE)) {
            return;
        }
        if ((action == ORYOL_WIN_PRESS) && (self->keys[key] == ORYOL_WIN_PRESS)) {
            repeated = true;
        }
        this->keys[key] = (char)action;
        if (repeated) {
            action = ORYOL_WIN_REPEAT;
        }
    }
    if (this->callbacks.key) {
        this->callbacks.key(key, scancode, action, mods);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputChar(unsigned int codepoint, int mods, int plain) {
    if (codepoint < 32 || (codepoint > 126 && codepoint < 160)) {
        return;
    }
    if (this->callbacks.charmods) {
        this->callbacks.charmods(codepoint, mods);
    }
    if (plain) {
        if (this->callbacks.character) {
            this->callbacks.character(codepoint);
        }
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputScroll(double xoffset, double yoffset) {
    if (this->callbacks.scroll) {
        this->callbacks.scroll(xoffset, yoffset);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputMouseClick(int button, int action, int mods) {
    if (button < 0 || button > ORYOL_WIN_MOUSE_BUTTON_LAST) {
        return;
    }
    this->mouseButtons[button] = (char)action;
    if (this->callbacks.mouseButton) {
        this->callbacks.mouseButton(button, action, mods);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputCursorMotion(double x, double y) {

    x = (x * this->contentScale) / this->windowScale;
    y = (y * this->contentScale) / this->windowScale;

    if (this->cursorMode == ORYOL_WIN_CURSOR_DISABLED) {
        if ((x == 0.0) && (y == 0.0)) {
            return;
        }
        this->windowCursorPosX += x;
        this->windowCursorPosY += y;
        x = this->windowCursorPosX;
        y = this->windowCursorPosY;
    }
    if (this->callbacks.cursorPos) {
        this->callbacks.cursorPos(x, y);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputCursorEnter(bool entered) {
    if (this->callbacks.cursorEnter) {
        this->callbacks.cursorEnter(entered ? 1 : 0);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputWindowFocus(bool focused)
{
    if (this->callbacks.focus) {
        this->callbacks.focus(focused ? 1 : 0);
    }
    if (!focused) {
        // release all pressed keyboard keys
        for (int i = 0; i <= ORYOL_WIN_KEY_LAST; i++) {
            if (this->keys[i] == ORYOL_WIN_PRESS) {
                this->inputKey(i, 0, ORYOL_WIN_RELEASE, 0);
            }
        }

        // Release all pressed mouse buttons
        for (int i = 0; i <= ORYOL_WIN_MOUSE_BUTTON_LAST; i++) {
            if (this->mouseButtons[i] == ORYOL_WIN_PRESS) {
                this->inputMouseClick(i, ORYOL_WIN_RELEASE, 0);
            }
        }
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputWindowPos(int x, int y) {
    if (this->callbacks.pos) {
        this->callbacks.pos(x, y);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputWindowSize(int width, int height) {

    width = int(width / this->windowScale);
    height = int(height / this->windowScale);

    if (this->callbacks.size) {
        this->callbacks.size(width, height);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputWindowIconify(bool iconified) {
    if (this->callbacks.iconify) {
        this->callbacks.iconify(iconified ? 1 : 0);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::inputFramebufferSize(int width, int height) {

    width = (width * this->contentScale) / this->windowScale;
    height = (height * this->contentScale) / this->windowScale;

    if (this->callbacks.fbsize) {
        this->callbacks.fbsize(width, height);
    }
}

//------------------------------------------------------------------------------
int
winDisplayMgr::inputTranslateKey(WPARAM wParam, LPARAM lParam) {
    if (wParam == VK_CONTROL) {
        // The CTRL keys require special handling
        MSG next;
        DWORD time;

        // Is this an extended key (i.e. right key)?
        if (lParam & 0x01000000) {
            return ORYOL_WIN_KEY_RIGHT_CONTROL;
        }

        // Here is a trick: "Alt Gr" sends LCTRL, then RALT. We only
        // want the RALT message, so we try to see if the next message
        // is a RALT message. In that case, this is a false LCTRL!
        time = ::GetMessageTime();
        if (::PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE)) {
            if (next.message == WM_KEYDOWN ||
                next.message == WM_SYSKEYDOWN ||
                next.message == WM_KEYUP ||
                next.message == WM_SYSKEYUP) {

                if (next.wParam == VK_MENU &&
                    (next.lParam & 0x01000000) &&
                    next.time == time) {
                    // Next message is a RALT down message, which
                    // means that this is not a proper LCTRL message
                    return ORYOL_WIN_KEY_INVALID;
                }
            }
        }
        return ORYOL_WIN_KEY_LEFT_CONTROL;
    }
    return this->publicKeys[HIWORD(lParam) & 0x1FF];
}

//------------------------------------------------------------------------------
void
winDisplayMgr::setInputMode(int mode, int value)
{
    switch (mode) {
    case ORYOL_WIN_CURSOR:
        this->setCursorMode(value);
        break;
    default:
        o_error("d3d11DisplayMgr::setInputMode(): invalid mode!\n");
        break;
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::setCursorMode(int newMode) {
    const int oldMode = this->cursorMode;
    if (newMode != ORYOL_WIN_CURSOR_NORMAL &&
        newMode != ORYOL_WIN_CURSOR_HIDDEN &&
        newMode != ORYOL_WIN_CURSOR_DISABLED) {
        return;
    }
    if (oldMode == newMode) {
        return;
    }
    this->cursorMode = newMode;
    if (oldMode == ORYOL_WIN_CURSOR_DISABLED) {
        this->winSetCursorPos(this->cursorPosX, this->cursorPosY);
    }
    else if (newMode == ORYOL_WIN_CURSOR_DISABLED) {
        this->winGetCursorPos(&this->cursorPosX, &this->cursorPosY);
        this->windowCursorPosX = (this->cursorPosX * this->contentScale) / this->windowScale;
        this->windowCursorPosY = (this->cursorPosY * this->contentScale) / this->windowScale;
        int width, height;
        this->winGetWindowSize(&width, &height);
        this->winSetCursorPos(width/2, height/2);
    }
    this->winSetCursorMode(this->cursorMode);
}

//------------------------------------------------------------------------------
void
winDisplayMgr::updateClipCursor() {
    o_assert_dbg(this->hwnd);
    RECT clipRect;
    ::GetClientRect(this->hwnd, &clipRect);
    ::ClientToScreen(this->hwnd, (POINT*)&clipRect.left);
    ::ClientToScreen(this->hwnd, (POINT*)&clipRect.right);
    ::ClipCursor(&clipRect);
}

//------------------------------------------------------------------------------
void
winDisplayMgr::winSetCursorMode(int mode) {
    if (mode == ORYOL_WIN_CURSOR_DISABLED) {
        this->updateClipCursor();
    }
    else {
        ::ClipCursor(NULL);
    }
    POINT pos;
    if (!::GetCursorPos(&pos)) {
        return;
    }
    if (::WindowFromPoint(pos) != this->hwnd) {
        return;
    }
    if (mode == ORYOL_WIN_CURSOR_NORMAL) {
        ::SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
    else {
        ::SetCursor(NULL);
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::winSetCursorPos(double xpos, double ypos) {
    POINT pos = { (int)xpos, (int)ypos };
    this->win32CursorPosX = pos.x;
    this->win32CursorPosY = pos.y;
    ::ClientToScreen(this->hwnd, &pos);
    ::SetCursorPos(pos.x, pos.y);
}

//------------------------------------------------------------------------------
void
winDisplayMgr::winGetCursorPos(double* xpos, double* ypos) {
    POINT pos;
    if (::GetCursorPos(&pos)) {
        ::ScreenToClient(this->hwnd, &pos);
        if (xpos) {
            *xpos = pos.x;
        }
        if (ypos) {
            *ypos = pos.y;
        }
    }
}

//------------------------------------------------------------------------------
void
winDisplayMgr::winGetWindowSize(int* width, int* height) {
    RECT area;
    ::GetClientRect(this->hwnd, &area);
    if (width) {
        *width = area.right;
    }
    if (height) {
        *height = area.bottom;
    }
}

//------------------------------------------------------------------------------
LRESULT CALLBACK
winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    winDisplayMgr* self = winDisplayMgr::self;
    switch (uMsg) {
    case WM_SETFOCUS:
        if (self) {
            if (self->cursorMode == ORYOL_WIN_CURSOR_DISABLED) {
                self->winSetCursorMode(ORYOL_WIN_CURSOR_DISABLED);
            }
            self->inputWindowFocus(true);
            return 0;
        }
        break;

    case WM_KILLFOCUS:
        if (self) {
            if (self->cursorMode == ORYOL_WIN_CURSOR_DISABLED) {
                self->winSetCursorMode(ORYOL_WIN_CURSOR_NORMAL);
            }
            // FIXME: fullscreen handling
            self->inputWindowFocus(false);
            return 0;
        }
        break;

    case WM_SYSCOMMAND:
        switch (wParam & 0xfff0) {
            // FIXME: fullscreen handling (disable screen saver and power mode)
            // user trying to acces application menu using ALT?
        case SC_KEYMENU:
            return 0;
        }
        break;

    case WM_CLOSE:
        if (self) {
            self->quitRequested = true;
            return 0;
        }
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (self) {
            const int scancode = (lParam >> 16) & 0x1ff;
            const int key = self->inputTranslateKey(wParam, lParam);
            if (key == ORYOL_WIN_KEY_INVALID) {
                break;
            }
            self->inputKey(key, scancode, ORYOL_WIN_PRESS, self->inputGetKeyMods());
        }
        break;

    case WM_CHAR:
        if (self) {
            self->inputChar((unsigned int)wParam, self->inputGetKeyMods(), true);
            return 0;
        }
        break;

    case WM_SYSCHAR:
        if (self) {
            self->inputChar((unsigned int)wParam, self->inputGetKeyMods(), false);
            return 0;
        }
        break;

    case WM_UNICHAR:
        // This message is not sent by Windows, but is sent by some
        // third-party input method engines
        if (self) {
            if (wParam == UNICODE_NOCHAR) {
                return TRUE;
            }
            self->inputChar((unsigned int)wParam, self->inputGetKeyMods(), true);
            return FALSE;
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (self) {
            const int mods = self->inputGetKeyMods();
            const int scancode = (lParam >> 16) & 0x1ff;
            const int key = self->inputTranslateKey(wParam, lParam);
            if (key == ORYOL_WIN_KEY_INVALID) {
                break;
            }
            if (wParam == VK_SHIFT) {
                // Release both Shift keys on Shift up event, as only one event
                // is sent even if both keys are released
                self->inputKey(ORYOL_WIN_KEY_LEFT_SHIFT, scancode, ORYOL_WIN_RELEASE, mods);
                self->inputKey(ORYOL_WIN_KEY_RIGHT_SHIFT, scancode, ORYOL_WIN_RELEASE, mods);
            }
            else if (wParam == VK_SNAPSHOT) {
                // Key down is not reported for the print screen key
                self->inputKey(key, scancode, ORYOL_WIN_PRESS, mods);
                self->inputKey(key, scancode, ORYOL_WIN_RELEASE, mods);
            }
            else {
                self->inputKey(key, scancode, ORYOL_WIN_RELEASE, mods);
            }
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
        if (self) {
            const int mods = self->inputGetKeyMods();
            ::SetCapture(hWnd);
            if (uMsg == WM_LBUTTONDOWN) {
                self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_LEFT, ORYOL_WIN_PRESS, mods);
            }
            else if (uMsg == WM_RBUTTONDOWN) {
                self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_RIGHT, ORYOL_WIN_PRESS, mods);
            }
            else if (uMsg == WM_MBUTTONDOWN) {
                self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_MIDDLE, ORYOL_WIN_PRESS, mods);
            }
            else {
                if (HIWORD(wParam) == XBUTTON1) {
                    self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_4, ORYOL_WIN_PRESS, mods);
                }
                else if (HIWORD(wParam) == XBUTTON2) {
                    self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_5, ORYOL_WIN_PRESS, mods);
                }
                return TRUE;
            }
        }
        return 0;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
        if (self) {
            const int mods = self->inputGetKeyMods();
            ::ReleaseCapture();
            if (uMsg == WM_LBUTTONUP) {
                self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_LEFT, ORYOL_WIN_RELEASE, mods);
            }
            else if (uMsg == WM_RBUTTONUP) {
                self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_RIGHT, ORYOL_WIN_RELEASE, mods);
            }
            else if (uMsg == WM_MBUTTONUP) {
                self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_MIDDLE, ORYOL_WIN_RELEASE, mods);
            }
            else {
                if (HIWORD(wParam) == XBUTTON1) {
                    self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_4, ORYOL_WIN_RELEASE, mods);
                }
                else if (HIWORD(wParam) == XBUTTON2) {
                    self->inputMouseClick(ORYOL_WIN_MOUSE_BUTTON_5, ORYOL_WIN_RELEASE, mods);
                }
                return TRUE;
            }
        }
        return 0;

    case WM_MOUSEMOVE:
        if (self) {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            if (self->cursorMode == ORYOL_WIN_CURSOR_DISABLED) {
                self->inputCursorMotion(x - self->win32CursorPosX, y - self->win32CursorPosY);
            }
            else {
                self->inputCursorMotion(x, y);
            }
            self->win32CursorPosX = x;
            self->win32CursorPosY = y;
            if (!self->cursorTracked) {
                TRACKMOUSEEVENT tme;
                Memory::Clear(&tme, sizeof(tme));
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = self->hwnd;
                TrackMouseEvent(&tme);
                self->cursorTracked = true;
                self->inputCursorEnter(true);
            }
            return 0;
        }
        break;

    case WM_MOUSELEAVE:
        if (self) {
            self->cursorTracked = false;
            self->inputCursorEnter(false);
            return 0;
        }
        break;

    case WM_MOUSEWHEEL:
        if (self) {
            self->inputScroll(0.0, (SHORT)HIWORD(wParam) / (double)ORYOL_WIN_WHEEL_DELTA);
            return 0;
        }
        break;

    case WM_SIZE:
        if (self && !self->inCreateWindow) {
            if (self->cursorMode == ORYOL_WIN_CURSOR_DISABLED) {
                self->updateClipCursor();
            }
            if (!self->iconified && (wParam == SIZE_MINIMIZED)) {
                self->iconified = true;
                self->inputWindowIconify(true);
            }
            else if (self->iconified && ((wParam == SIZE_RESTORED) || (wParam == SIZE_MAXIMIZED))) {
                self->iconified = false;
                self->inputWindowIconify(false);
            }
            return 0;
        }
        break;

    case WM_MOVE:
        if (self) {
            if (self->cursorMode == ORYOL_WIN_CURSOR_DISABLED) {
                self->updateClipCursor();
            }
            self->inputWindowPos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        }
        break;

    case WM_PAINT:
        // nothing to do (in GLFW this calls the _glfwWindowDamage() function
        break;

    case WM_ERASEBKGND:
        return TRUE;

    case WM_SETCURSOR:
        if (self) {
            if (LOWORD(lParam) == HTCLIENT) {
                if ((self->cursorMode == ORYOL_WIN_CURSOR_HIDDEN) ||
                    (self->cursorMode == ORYOL_WIN_CURSOR_DISABLED)) {
                    ::SetCursor(NULL);
                    return TRUE;
                }
                // FIXME: custom cursor handling!
            }
        }
        break;

    case WM_DEVICECHANGE:
        // FIXME
        break;

    case WM_DWMCOMPOSITIONCHANGED:
        // FIXME
        break;

    default:
        break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
void
winDisplayMgr::setupKeyTranslationTable() {
    Memory::Fill(this->publicKeys, sizeof(this->publicKeys), 0xFF);
    this->publicKeys[0x00B] = ORYOL_WIN_KEY_0;
    this->publicKeys[0x002] = ORYOL_WIN_KEY_1;
    this->publicKeys[0x003] = ORYOL_WIN_KEY_2;
    this->publicKeys[0x004] = ORYOL_WIN_KEY_3;
    this->publicKeys[0x005] = ORYOL_WIN_KEY_4;
    this->publicKeys[0x006] = ORYOL_WIN_KEY_5;
    this->publicKeys[0x007] = ORYOL_WIN_KEY_6;
    this->publicKeys[0x008] = ORYOL_WIN_KEY_7;
    this->publicKeys[0x009] = ORYOL_WIN_KEY_8;
    this->publicKeys[0x00A] = ORYOL_WIN_KEY_9;
    this->publicKeys[0x01E] = ORYOL_WIN_KEY_A;
    this->publicKeys[0x030] = ORYOL_WIN_KEY_B;
    this->publicKeys[0x02E] = ORYOL_WIN_KEY_C;
    this->publicKeys[0x020] = ORYOL_WIN_KEY_D;
    this->publicKeys[0x012] = ORYOL_WIN_KEY_E;
    this->publicKeys[0x021] = ORYOL_WIN_KEY_F;
    this->publicKeys[0x022] = ORYOL_WIN_KEY_G;
    this->publicKeys[0x023] = ORYOL_WIN_KEY_H;
    this->publicKeys[0x017] = ORYOL_WIN_KEY_I;
    this->publicKeys[0x024] = ORYOL_WIN_KEY_J;
    this->publicKeys[0x025] = ORYOL_WIN_KEY_K;
    this->publicKeys[0x026] = ORYOL_WIN_KEY_L;
    this->publicKeys[0x032] = ORYOL_WIN_KEY_M;
    this->publicKeys[0x031] = ORYOL_WIN_KEY_N;
    this->publicKeys[0x018] = ORYOL_WIN_KEY_O;
    this->publicKeys[0x019] = ORYOL_WIN_KEY_P;
    this->publicKeys[0x010] = ORYOL_WIN_KEY_Q;
    this->publicKeys[0x013] = ORYOL_WIN_KEY_R;
    this->publicKeys[0x01F] = ORYOL_WIN_KEY_S;
    this->publicKeys[0x014] = ORYOL_WIN_KEY_T;
    this->publicKeys[0x016] = ORYOL_WIN_KEY_U;
    this->publicKeys[0x02F] = ORYOL_WIN_KEY_V;
    this->publicKeys[0x011] = ORYOL_WIN_KEY_W;
    this->publicKeys[0x02D] = ORYOL_WIN_KEY_X;
    this->publicKeys[0x015] = ORYOL_WIN_KEY_Y;
    this->publicKeys[0x02C] = ORYOL_WIN_KEY_Z;
    this->publicKeys[0x028] = ORYOL_WIN_KEY_APOSTROPHE;
    this->publicKeys[0x02B] = ORYOL_WIN_KEY_BACKSLASH;
    this->publicKeys[0x033] = ORYOL_WIN_KEY_COMMA;
    this->publicKeys[0x00D] = ORYOL_WIN_KEY_EQUAL;
    this->publicKeys[0x029] = ORYOL_WIN_KEY_GRAVE_ACCENT;
    this->publicKeys[0x01A] = ORYOL_WIN_KEY_LEFT_BRACKET;
    this->publicKeys[0x00C] = ORYOL_WIN_KEY_MINUS;
    this->publicKeys[0x034] = ORYOL_WIN_KEY_PERIOD;
    this->publicKeys[0x01B] = ORYOL_WIN_KEY_RIGHT_BRACKET;
    this->publicKeys[0x027] = ORYOL_WIN_KEY_SEMICOLON;
    this->publicKeys[0x035] = ORYOL_WIN_KEY_SLASH;
    this->publicKeys[0x056] = ORYOL_WIN_KEY_WORLD_2;
    this->publicKeys[0x00E] = ORYOL_WIN_KEY_BACKSPACE;
    this->publicKeys[0x153] = ORYOL_WIN_KEY_DELETE;
    this->publicKeys[0x14F] = ORYOL_WIN_KEY_END;
    this->publicKeys[0x01C] = ORYOL_WIN_KEY_ENTER;
    this->publicKeys[0x001] = ORYOL_WIN_KEY_ESCAPE;
    this->publicKeys[0x147] = ORYOL_WIN_KEY_HOME;
    this->publicKeys[0x152] = ORYOL_WIN_KEY_INSERT;
    this->publicKeys[0x15D] = ORYOL_WIN_KEY_MENU;
    this->publicKeys[0x151] = ORYOL_WIN_KEY_PAGE_DOWN;
    this->publicKeys[0x149] = ORYOL_WIN_KEY_PAGE_UP;
    this->publicKeys[0x045] = ORYOL_WIN_KEY_PAUSE;
    this->publicKeys[0x039] = ORYOL_WIN_KEY_SPACE;
    this->publicKeys[0x00F] = ORYOL_WIN_KEY_TAB;
    this->publicKeys[0x03A] = ORYOL_WIN_KEY_CAPS_LOCK;
    this->publicKeys[0x145] = ORYOL_WIN_KEY_NUM_LOCK;
    this->publicKeys[0x046] = ORYOL_WIN_KEY_SCROLL_LOCK;
    this->publicKeys[0x03B] = ORYOL_WIN_KEY_F1;
    this->publicKeys[0x03C] = ORYOL_WIN_KEY_F2;
    this->publicKeys[0x03D] = ORYOL_WIN_KEY_F3;
    this->publicKeys[0x03E] = ORYOL_WIN_KEY_F4;
    this->publicKeys[0x03F] = ORYOL_WIN_KEY_F5;
    this->publicKeys[0x040] = ORYOL_WIN_KEY_F6;
    this->publicKeys[0x041] = ORYOL_WIN_KEY_F7;
    this->publicKeys[0x042] = ORYOL_WIN_KEY_F8;
    this->publicKeys[0x043] = ORYOL_WIN_KEY_F9;
    this->publicKeys[0x044] = ORYOL_WIN_KEY_F10;
    this->publicKeys[0x057] = ORYOL_WIN_KEY_F11;
    this->publicKeys[0x058] = ORYOL_WIN_KEY_F12;
    this->publicKeys[0x064] = ORYOL_WIN_KEY_F13;
    this->publicKeys[0x065] = ORYOL_WIN_KEY_F14;
    this->publicKeys[0x066] = ORYOL_WIN_KEY_F15;
    this->publicKeys[0x067] = ORYOL_WIN_KEY_F16;
    this->publicKeys[0x068] = ORYOL_WIN_KEY_F17;
    this->publicKeys[0x069] = ORYOL_WIN_KEY_F18;
    this->publicKeys[0x06A] = ORYOL_WIN_KEY_F19;
    this->publicKeys[0x06B] = ORYOL_WIN_KEY_F20;
    this->publicKeys[0x06C] = ORYOL_WIN_KEY_F21;
    this->publicKeys[0x06D] = ORYOL_WIN_KEY_F22;
    this->publicKeys[0x06E] = ORYOL_WIN_KEY_F23;
    this->publicKeys[0x076] = ORYOL_WIN_KEY_F24;
    this->publicKeys[0x038] = ORYOL_WIN_KEY_LEFT_ALT;
    this->publicKeys[0x01D] = ORYOL_WIN_KEY_LEFT_CONTROL;
    this->publicKeys[0x02A] = ORYOL_WIN_KEY_LEFT_SHIFT;
    this->publicKeys[0x15B] = ORYOL_WIN_KEY_LEFT_SUPER;
    this->publicKeys[0x137] = ORYOL_WIN_KEY_PRINT_SCREEN;
    this->publicKeys[0x138] = ORYOL_WIN_KEY_RIGHT_ALT;
    this->publicKeys[0x11D] = ORYOL_WIN_KEY_RIGHT_CONTROL;
    this->publicKeys[0x036] = ORYOL_WIN_KEY_RIGHT_SHIFT;
    this->publicKeys[0x15C] = ORYOL_WIN_KEY_RIGHT_SUPER;
    this->publicKeys[0x150] = ORYOL_WIN_KEY_DOWN;
    this->publicKeys[0x14B] = ORYOL_WIN_KEY_LEFT;
    this->publicKeys[0x14D] = ORYOL_WIN_KEY_RIGHT;
    this->publicKeys[0x148] = ORYOL_WIN_KEY_UP;
    this->publicKeys[0x052] = ORYOL_WIN_KEY_KP_0;
    this->publicKeys[0x04F] = ORYOL_WIN_KEY_KP_1;
    this->publicKeys[0x050] = ORYOL_WIN_KEY_KP_2;
    this->publicKeys[0x051] = ORYOL_WIN_KEY_KP_3;
    this->publicKeys[0x04B] = ORYOL_WIN_KEY_KP_4;
    this->publicKeys[0x04C] = ORYOL_WIN_KEY_KP_5;
    this->publicKeys[0x04D] = ORYOL_WIN_KEY_KP_6;
    this->publicKeys[0x047] = ORYOL_WIN_KEY_KP_7;
    this->publicKeys[0x048] = ORYOL_WIN_KEY_KP_8;
    this->publicKeys[0x049] = ORYOL_WIN_KEY_KP_9;
    this->publicKeys[0x04E] = ORYOL_WIN_KEY_KP_ADD;
    this->publicKeys[0x053] = ORYOL_WIN_KEY_KP_DECIMAL;
    this->publicKeys[0x135] = ORYOL_WIN_KEY_KP_DIVIDE;
    this->publicKeys[0x11C] = ORYOL_WIN_KEY_KP_ENTER;
    this->publicKeys[0x037] = ORYOL_WIN_KEY_KP_MULTIPLY;
    this->publicKeys[0x04A] = ORYOL_WIN_KEY_KP_SUBTRACT;
}

} // namespace _priv
} // namespace Oryol
