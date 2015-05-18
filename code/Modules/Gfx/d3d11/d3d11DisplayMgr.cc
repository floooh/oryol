//------------------------------------------------------------------------------
//  d3d11DisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11DisplayMgr.h"
#include "Core/String/StringConverter.h"
#include <WinUser.h>

namespace Oryol {
namespace _priv {

static d3d11DisplayMgr* self = nullptr;

//------------------------------------------------------------------------------
d3d11DisplayMgr::d3d11DisplayMgr() :
quitRequested(false),
hwnd(0),
dwStyle(0),
dwExStyle(0) {
    self = this;
}

//------------------------------------------------------------------------------
d3d11DisplayMgr::~d3d11DisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
    self = nullptr;
}   

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::SetupDisplay(const GfxSetup& setup) {
    o_assert(!this->IsDisplayValid());
    
    displayMgrBase::SetupDisplay(setup);

    this->registerWindowClass();
    this->createWindow(setup);
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());

    this->destroyWindow();
    this->unregisterWindowClass();

    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::Present() {
    Log::Info("PRESENT!\n");
    Sleep(100);
}

//------------------------------------------------------------------------------
bool
d3d11DisplayMgr::QuitRequested() const {
    return this->quitRequested;
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::ProcessSystemEvents() {

    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (WM_QUIT == msg.message) {
            this->quitRequested = true;
        }
        else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    // FIXME: there's some more LSHIFT/RSHIFT related stuff in GLFW, 
    // see _glfwPlatformPollEvents in win32_window.c

    // FIXME
    displayMgrBase::ProcessSystemEvents();
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::registerWindowClass() {
    
    WNDCLASSW wc;
    Memory::Clear(&wc, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC) winProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"ORYOL";

    // load user-provided icon if available (use the same name like GLFW)
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), "GLFW_ICON");
    if (!wc.hIcon) {
        // no user-provided icon, use default icon
        wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    }
    if (!RegisterClassW(&wc)) {
        o_error("d3d11DisplayMgr: failed to register window class!\n");
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::unregisterWindowClass() {
    UnregisterClassW(L"ORYOL", GetModuleHandleW(NULL));
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::createWindow(const GfxSetup& setup) {
    o_assert(0 == this->hwnd);

    // setup window style flags
    this->dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    this->dwExStyle = WS_EX_APPWINDOW;
    if (setup.Windowed) {
        this->dwStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
        this->dwExStyle |= WS_EX_WINDOWEDGE;
    }
    else {
        this->dwStyle |= WS_POPUP;
    }
    
    int width, height;
    this->computeWindowSize(setup.Width, setup.Height, width, height);

    WideString title = StringConverter::UTF8ToWide(setup.Title);

    this->hwnd = CreateWindowExW(this->dwExStyle,   // dwExStyle 
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
    ShowWindow(this->hwnd, SW_SHOW);
    BringWindowToTop(this->hwnd);
    SetForegroundWindow(this->hwnd);
    SetFocus(this->hwnd);
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::destroyWindow() {
    o_assert(this->hwnd);

    DestroyWindow(this->hwnd);
    this->hwnd = 0;
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::computeWindowSize(int clientWidth, int clientHeight, int& outWidth, int& outHeight) {
    RECT rect = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRectEx(&rect, this->dwStyle, FALSE, this->dwExStyle);
    outWidth = rect.right - rect.left;
    outHeight = rect.bottom - rect.top;
}

//------------------------------------------------------------------------------
LRESULT CALLBACK 
d3d11DisplayMgr::winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SYSCOMMAND:
            switch (wParam & 0xfff0) {
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
            // FIXME!
            break;

        case WM_CHAR:
            // FIXME!
            break;

        case WM_SYSCHAR:
            // FIXME!
            break;

        case WM_UNICHAR:
            // This message is not sent by Windows, but is sent by some
            // third-party input method engines

            // FIXME!
            break;

        case WM_KEYUP:
        case WM_SYSKEYUP:
            // FIXME!
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            // FIXME!
            return 0;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            // FIXME!
            return 0;

        case WM_MOUSEMOVE:
            // FIXME!
            return 0;

        case WM_MOUSELEAVE:
            // FIXME!
            return 0;

        case WM_MOUSEWHEEL:
            // FIXME!
            return 0;

        case WM_SIZE:
            // FIXME!
            return 0;

        case WM_MOVE:
            // FIXME!
            return 0;

        case WM_PAINT:
            // FIXME!
            break;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_SETCURSOR:
            // FIXME!
            break;
        
        default:
            break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

} // namespace _priv
} // namespace Oryol
