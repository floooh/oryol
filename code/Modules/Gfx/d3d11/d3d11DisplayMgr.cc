//------------------------------------------------------------------------------
//  d3d11DisplayMgr.cc
//  NOTE: Most of the window handling code is taken from GLFW, it makes
//  sense to keep the code that's not directly related to D3D11 as close
//  to GLFW behaviour as possible!
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11DisplayMgr.h"
#include "d3d11Types.h"
#include "d3d11InputDefs.h"
#include "Gfx/Core/renderer.h"
#include "Core/String/StringConverter.h"
#include "Core/String/StringBuilder.h"

#define UNICODE
#include "d3d11_impl.h"
#include <windowsx.h>

namespace Oryol {
namespace _priv {

static LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
d3d11DisplayMgr* d3d11DisplayMgr::self = nullptr;

static DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;

//------------------------------------------------------------------------------
d3d11DisplayMgr::d3d11DisplayMgr() :
d3d11Device(nullptr),
d3d11DeviceContext(nullptr),
dxgiSwapChain(nullptr),
d3d11RenderTarget(nullptr),
d3d11RenderTargetView(nullptr),
d3d11DepthStencilBuffer(nullptr),
d3d11DepthStencilView(nullptr),
quitRequested(false),
hwnd(0),
dwStyle(0),
dwExStyle(0),
cursorMode(ORYOL_D3D11_CURSOR_NORMAL),
cursorPosX(0.0),
cursorPosY(0.0),
cursorInside(false),
iconified(false) {
    self = this;
    Memory::Clear(this->mouseButtons, sizeof(mouseButtons));
    Memory::Clear(this->keys, sizeof(keys));
    Memory::Clear(&this->callbacks, sizeof(this->callbacks));
    Memory::Clear(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
    this->setupKeyTranslationTable();
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
d3d11DisplayMgr::SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());

    displayMgrBase::SetupDisplay(setup, ptrs);

    this->registerWindowClass();
    this->createWindow();
    this->createDeviceAndSwapChain();
    this->createDefaultRenderTarget(setup.Width, setup.Height);
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());

    this->destroyDefaultRenderTarget();
    this->destroyDeviceAndSwapChain();
    this->destroyWindow();
    this->unregisterWindowClass();

    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::Present() {
    o_assert_dbg(this->dxgiSwapChain);
    this->dxgiSwapChain->Present(this->gfxSetup.SwapInterval, 0);
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
    while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (WM_QUIT == msg.message) {
            this->quitRequested = true;
        }
        else {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
    }

    // FIXME: there's some more LSHIFT/RSHIFT related stuff in GLFW, 
    // see _glfwPlatformPollEvents in win32_window.c

    // FIXME
    displayMgrBase::ProcessSystemEvents();
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::createDeviceAndSwapChain() {
    o_assert_dbg(nullptr == this->d3d11Device);
    o_assert_dbg(nullptr == this->d3d11DeviceContext);
    o_assert_dbg(nullptr == this->dxgiSwapChain);
    o_assert_dbg(0 != this->hwnd);

    int createFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
    #if ORYOL_DEBUG
        createFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    Memory::Clear(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
    dxgiSwapChainDesc.BufferDesc.Width = this->gfxSetup.Width;
    dxgiSwapChainDesc.BufferDesc.Height = this->gfxSetup.Height;
    dxgiSwapChainDesc.BufferDesc.Format = d3d11Types::asSwapChainFormat(this->gfxSetup.ColorFormat);
    dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    dxgiSwapChainDesc.SampleDesc.Count = this->gfxSetup.Samples < 2 ? 1 : this->gfxSetup.Samples;
    dxgiSwapChainDesc.SampleDesc.Quality = this->gfxSetup.Samples < 2 ? 0 : D3D11_STANDARD_MULTISAMPLE_PATTERN;
    dxgiSwapChainDesc.BufferCount = 1;
    dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dxgiSwapChainDesc.OutputWindow = this->hwnd;
    dxgiSwapChainDesc.Windowed = this->gfxSetup.Windowed;
    dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    dxgiSwapChainDesc.Flags = 0;

    D3D_FEATURE_LEVEL d3dFeatureLevel;
    HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
        NULL,                           // pAdapter (use default adapter)
        D3D_DRIVER_TYPE_HARDWARE,       // DriverType
        NULL,                           // Software
        createFlags,                    // Flags
        NULL,                           // pFeatureLevels
        0,                              // FeatureLevels
        D3D11_SDK_VERSION,              // SDKVersion
        &dxgiSwapChainDesc,             // pSwapChainDesc
        &this->dxgiSwapChain,           // ppSwapChain
        &this->d3d11Device,             // ppDevice
        &d3dFeatureLevel,               // pFeatureLevel
        &this->d3d11DeviceContext);     // ppImmediateContext
    o_assert2(SUCCEEDED(hr), "Failed to create D3D11 device and swap chain1\n");
    o_assert(this->d3d11Device);
    o_assert(this->d3d11DeviceContext);
    o_assert(this->dxgiSwapChain);
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::destroyDeviceAndSwapChain() {
    if (this->dxgiSwapChain) {
        this->dxgiSwapChain->Release();
        this->dxgiSwapChain = nullptr;
    }
    if (this->d3d11Device) {
        this->d3d11Device->Release();
        this->d3d11Device = nullptr;
    }
    if (this->d3d11DeviceContext) {
        this->d3d11DeviceContext->Release();
        this->d3d11DeviceContext = nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::createDefaultRenderTarget(int width, int height) {
    o_assert_dbg(this->d3d11Device);
    o_assert_dbg(this->dxgiSwapChain);
    o_assert_dbg(nullptr == this->d3d11RenderTarget);
    o_assert_dbg(nullptr == this->d3d11RenderTargetView);
    o_assert_dbg(nullptr == this->d3d11DepthStencilBuffer);
    o_assert_dbg(nullptr == this->d3d11DepthStencilView);
    HRESULT hr;

    // setup color buffer (obtain from swap chain and create render target view)
    hr = this->dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &this->d3d11RenderTarget);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(this->d3d11RenderTarget);
    hr = this->d3d11Device->CreateRenderTargetView(this->d3d11RenderTarget, NULL, &this->d3d11RenderTargetView);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(this->d3d11RenderTargetView);

    // setup depth/stencil buffer (if required)
    if (PixelFormat::None != this->gfxSetup.DepthFormat) {
        
        D3D11_TEXTURE2D_DESC depthStencilDesc;
        Memory::Clear(&depthStencilDesc, sizeof(depthStencilDesc));
        depthStencilDesc.Width = width;
        depthStencilDesc.Height = height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = d3d11Types::asRenderTargetFormat(this->gfxSetup.DepthFormat);
        depthStencilDesc.SampleDesc = dxgiSwapChainDesc.SampleDesc;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;
        hr = this->d3d11Device->CreateTexture2D(&depthStencilDesc, nullptr, &this->d3d11DepthStencilBuffer);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(this->d3d11DepthStencilBuffer);

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        Memory::Clear(&dsvDesc, sizeof(dsvDesc));
        dsvDesc.Format = depthStencilDesc.Format;
        if (this->gfxSetup.Samples > 1) {
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        }
        else {
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        }
        dsvDesc.Texture2D.MipSlice = 0;
        hr = this->d3d11Device->CreateDepthStencilView(this->d3d11DepthStencilBuffer, &dsvDesc, &this->d3d11DepthStencilView);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(this->d3d11DepthStencilView);
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::destroyDefaultRenderTarget() {
    if (this->d3d11RenderTarget) {
        this->d3d11RenderTarget->Release();
        this->d3d11RenderTarget = nullptr;
    }
    if (this->d3d11RenderTargetView) {
        this->d3d11RenderTargetView->Release();
        this->d3d11RenderTargetView = nullptr;
    }
    if (this->d3d11DepthStencilBuffer) {
        this->d3d11DepthStencilBuffer->Release();
        this->d3d11DepthStencilBuffer = nullptr;
    }
    if (this->d3d11DepthStencilView) {
        this->d3d11DepthStencilView->Release();
        this->d3d11DepthStencilView = nullptr;
    }
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
d3d11DisplayMgr::unregisterWindowClass() {
    ::UnregisterClassW(L"ORYOL", GetModuleHandleW(NULL));
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::createWindow() {
    o_assert(0 == this->hwnd);

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
    strBuilder.Append(" (D3D11)");
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
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::destroyWindow() {
    o_assert(this->hwnd);

    ::DestroyWindow((HWND)this->hwnd);
    this->hwnd = 0;
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::computeWindowSize(int clientWidth, int clientHeight, int& outWidth, int& outHeight) {
    RECT rect = { 0, 0, clientWidth, clientHeight };
    ::AdjustWindowRectEx(&rect, this->dwStyle, FALSE, this->dwExStyle);
    outWidth = rect.right - rect.left;
    outHeight = rect.bottom - rect.top;
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::onWindowResize(int newWidth, int newHeight) {
    o_assert((0 != newWidth) && (0 != newHeight));

    // NOTE: this method is not called when minimized, or restored from minimized
    if ((newWidth != this->displayAttrs.FramebufferWidth) ||
        (newWidth != this->displayAttrs.FramebufferHeight)) {
    
        this->displayAttrs.FramebufferWidth = newWidth;
        this->displayAttrs.FramebufferHeight = newHeight;
        this->displayAttrs.WindowWidth = newWidth;
        this->displayAttrs.WindowHeight = newHeight;
    }

    // resize the DXGI framebuffer (this requires that all state is unbound)
    if (this->dxgiSwapChain) {
        this->pointers.renderer->resetStateCache();
        this->destroyDefaultRenderTarget();
        DXGI_FORMAT d3d11Fmt = d3d11Types::asSwapChainFormat(this->gfxSetup.ColorFormat);
        HRESULT hr = this->dxgiSwapChain->ResizeBuffers(1, newWidth, newHeight, d3d11Fmt, 0);
        o_assert(SUCCEEDED(hr));
        this->createDefaultRenderTarget(newWidth, newHeight);
    }
}

//------------------------------------------------------------------------------
int
d3d11DisplayMgr::inputGetKeyMods() {
    int mods = 0;
    if (::GetKeyState(VK_SHIFT) & (1<<31)) {
        mods |= ORYOL_D3D11_MOD_SHIFT;
    }
    if (::GetKeyState(VK_CONTROL) & (1<<31)) {
        mods |= ORYOL_D3D11_MOD_CONTROL;
    }
    if (::GetKeyState(VK_MENU) & (1<<31)) {
        mods |= ORYOL_D3D11_MOD_ALT;
    }
    if ((::GetKeyState(VK_LWIN) | ::GetKeyState(VK_RWIN)) & (1<<31)) {
        mods |= ORYOL_D3D11_MOD_SUPER;
    }
    return mods;
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::updateClipRect() {
    o_assert_dbg(this->hwnd);
    RECT clipRect;
    ::GetClientRect(this->hwnd, &clipRect);
    ::ClientToScreen(this->hwnd, (POINT*)&clipRect.left);
    ::ClientToScreen(this->hwnd, (POINT*)&clipRect.right);
    ::ClipCursor(&clipRect);
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::restoreCursor() {
    o_assert_dbg(0 != this->hwnd);
    POINT pos;
    ::ClipCursor(NULL);
    if (::GetCursorPos(&pos)) {
        if (::WindowFromPoint(pos) == self->hwnd) {
            // FIXME: handle custom cursor
            ::SetCursor(::LoadCursorW(NULL, IDC_ARROW));
        }
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::hideCursor() {
    o_assert_dbg(0 != this->hwnd);
    POINT pos;
    ::ClipCursor(NULL);
    if (::GetCursorPos(&pos)) {
        if (::WindowFromPoint(pos) == self->hwnd) {
            ::SetCursor(NULL);
        }
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::disableCursor() {
    o_assert_dbg(0 != this->hwnd);
    POINT pos;
    this->updateClipRect();
    if (::GetCursorPos(&pos)) {
        if (::WindowFromPoint(pos) == self->hwnd) {
            ::SetCursor(NULL);
        }
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::applyCursorMode() {
    switch (this->cursorMode)
    {
        case ORYOL_D3D11_CURSOR_NORMAL:
            this->restoreCursor();
            break;
        case ORYOL_D3D11_CURSOR_HIDDEN:
            this->hideCursor();
            break;
        case ORYOL_D3D11_CURSOR_DISABLED:
            this->disableCursor();
            break;
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::inputKey(int key, int scancode, int action, int mods) {
    if ((key >= 0) && (key <= ORYOL_D3D11_KEY_LAST)) {
        bool repeated = false;
        if ((action == ORYOL_D3D11_RELEASE) && (self->keys[key] == ORYOL_D3D11_RELEASE)) {
            return;
        }
        if ((action == ORYOL_D3D11_PRESS) && (self->keys[key] == ORYOL_D3D11_PRESS)) {
            repeated = true;
        }
        this->keys[key] = (char) action;
        if (repeated) {
            action = ORYOL_D3D11_REPEAT;
        }
    }
    if (this->callbacks.key) {
        this->callbacks.key(key, scancode, action, mods);
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::inputChar(unsigned int codepoint, int mods, int plain) {
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
d3d11DisplayMgr::inputScroll(double xoffset, double yoffset) {
    if (this->callbacks.scroll) {
        this->callbacks.scroll(xoffset, yoffset);
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::inputMouseClick(int button, int action, int mods) {
    if (button < 0 || button > ORYOL_D3D11_MOUSE_BUTTON_LAST) {
        return;
    }
    this->mouseButtons[button] = (char)action;
    if (this->callbacks.mouseButton) {
        this->callbacks.mouseButton(button, action, mods);
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::inputCursorMotion(double x, double y) {
    if (this->cursorMode == ORYOL_D3D11_CURSOR_DISABLED) {
        if ((x == 0.0) && (y == 0.0)) {
            return;
        }
        this->cursorPosX += x;
        this->cursorPosY += y;

        x = this->cursorPosX;
        y = this->cursorPosY;
    }
    if (this->callbacks.cursorPos) {
        this->callbacks.cursorPos(x, y);
    }
}

//------------------------------------------------------------------------------
void 
d3d11DisplayMgr::inputCursorEnter(bool entered) {
    if (this->callbacks.cursorEnter) {
        this->callbacks.cursorEnter(entered ? 1 : 0);
    }
}

//------------------------------------------------------------------------------
void 
d3d11DisplayMgr::inputWindowFocus(bool focused)
{
    if (this->callbacks.focus) {
        this->callbacks.focus(focused ? 1 : 0);
    }
    if (!focused) {
        // release all pressed keyboard keys
        for (int i = 0; i <= ORYOL_D3D11_KEY_LAST; i++) {
            if (this->keys[i] == ORYOL_D3D11_PRESS) {
                this->inputKey(i, 0, ORYOL_D3D11_RELEASE, 0);
            }
        }

        // Release all pressed mouse buttons
        for (int i = 0; i <= ORYOL_D3D11_MOUSE_BUTTON_LAST; i++) {
            if (this->mouseButtons[i] == ORYOL_D3D11_PRESS) {
                this->inputMouseClick(i, ORYOL_D3D11_RELEASE, 0);
            }
        }
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::inputWindowPos(int x, int y) {
    if (this->callbacks.pos) {
        this->callbacks.pos(x, y);
    }
}

//------------------------------------------------------------------------------
void 
d3d11DisplayMgr::inputWindowSize(int width, int height) {
    if (this->callbacks.size) {
        this->callbacks.size(width, height);
    }
}

//------------------------------------------------------------------------------
void 
d3d11DisplayMgr::inputWindowIconify(bool iconified) {
    if (this->callbacks.iconify) {
        this->callbacks.iconify(iconified ? 1 : 0);
    }
}

//------------------------------------------------------------------------------
void 
d3d11DisplayMgr::inputFramebufferSize(int width, int height) {
    if (this->callbacks.fbsize) {
        this->callbacks.fbsize(width, height);
    }
}

//------------------------------------------------------------------------------
int
d3d11DisplayMgr::inputTranslateKey(WPARAM wParam, LPARAM lParam) {
    if (wParam == VK_CONTROL) {
        // The CTRL keys require special handling
        MSG next;
        DWORD time;

        // Is this an extended key (i.e. right key)?
        if (lParam & 0x01000000) {
            return ORYOL_D3D11_KEY_RIGHT_CONTROL;
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
                    return ORYOL_D3D11_KEY_INVALID;
                }
            }
        }
        return ORYOL_D3D11_KEY_LEFT_CONTROL;
    }
    return this->publicKeys[HIWORD(lParam) & 0x1FF];
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::setCursorMode(int newMode) {

    const int oldMode = this->cursorMode;
    if (newMode != ORYOL_D3D11_CURSOR_NORMAL &&
        newMode != ORYOL_D3D11_CURSOR_HIDDEN &&
        newMode != ORYOL_D3D11_CURSOR_DISABLED) {
        return;
    }
    if (oldMode == newMode) {
        return;
    }
    this->cursorMode = newMode;
    // FIXME: I have omitted some cursor positioning code from GLFW here
    this->applyCursorMode();
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::setInputMode(int mode, int value)
{
    switch (mode) {
        case ORYOL_D3D11_CURSOR:
            this->setCursorMode(value);
            break;
        default:
            o_error("d3d11DisplayMgr::setInputMode(): invalid mode!\n");
            break;
    }
}

//------------------------------------------------------------------------------
LRESULT CALLBACK 
winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    d3d11DisplayMgr* self = d3d11DisplayMgr::self;
    switch (uMsg) {
        case WM_SETFOCUS:
            if (self) {
                if (self->cursorMode != ORYOL_D3D11_CURSOR_NORMAL) {
                    self->applyCursorMode();
                }
                // FIXME: fullscreen handling
                self->inputWindowFocus(true);
                return 0;
            }
            break;

        case WM_KILLFOCUS:
            if (self) {
                if (self->cursorMode != ORYOL_D3D11_CURSOR_NORMAL) {
                    self->restoreCursor();
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
                if (key == ORYOL_D3D11_KEY_INVALID) {
                    break;
                }
                self->inputKey(key, scancode, ORYOL_D3D11_PRESS, self->inputGetKeyMods());
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
                if (key == ORYOL_D3D11_KEY_INVALID) {
                    break;
                }
                if (wParam == VK_SHIFT) {
                    // Release both Shift keys on Shift up event, as only one event
                    // is sent even if both keys are released
                    self->inputKey(ORYOL_D3D11_KEY_LEFT_SHIFT, scancode, ORYOL_D3D11_RELEASE, mods);
                    self->inputKey(ORYOL_D3D11_KEY_RIGHT_SHIFT, scancode, ORYOL_D3D11_RELEASE, mods);
                }
                else if (wParam == VK_SNAPSHOT) {
                    // Key down is not reported for the print screen key
                    self->inputKey(key, scancode, ORYOL_D3D11_PRESS, mods);
                    self->inputKey(key, scancode, ORYOL_D3D11_RELEASE, mods);
                }
                else {
                    self->inputKey(key, scancode, ORYOL_D3D11_RELEASE, mods);
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
                    self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_LEFT, ORYOL_D3D11_PRESS, mods);
                }
                else if (uMsg == WM_RBUTTONDOWN) {
                    self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_RIGHT, ORYOL_D3D11_PRESS, mods);
                }
                else if (uMsg == WM_MBUTTONDOWN) {
                    self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_MIDDLE, ORYOL_D3D11_PRESS, mods);
                }
                else {
                    if (HIWORD(wParam) == XBUTTON1) {
                        self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_4, ORYOL_D3D11_PRESS, mods);
                    }
                    else if (HIWORD(wParam) == XBUTTON2) {
                        self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_5, ORYOL_D3D11_PRESS, mods);
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
                    self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_LEFT, ORYOL_D3D11_RELEASE, mods);
                }
                else if (uMsg == WM_RBUTTONUP) {
                    self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_RIGHT, ORYOL_D3D11_RELEASE, mods);
                }
                else if (uMsg == WM_MBUTTONUP) {
                    self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_MIDDLE, ORYOL_D3D11_RELEASE, mods);
                }
                else {
                    if (HIWORD(wParam) == XBUTTON1) {
                        self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_4, ORYOL_D3D11_RELEASE, mods);
                    }
                    else if (HIWORD(wParam) == XBUTTON2) {
                        self->inputMouseClick(ORYOL_D3D11_MOUSE_BUTTON_5, ORYOL_D3D11_RELEASE, mods);
                    }
                    return TRUE;
                }
            }
            return 0;

        case WM_MOUSEMOVE:
            if (self) {
                const int x = GET_X_LPARAM(lParam);
                const int y = GET_Y_LPARAM(lParam);
                if (self->cursorMode == ORYOL_D3D11_CURSOR_DISABLED) {
                    self->inputCursorMotion(x - self->cursorPosX, y - self->cursorPosY);
                }
                else {
                    self->inputCursorMotion(x, y);
                }
                self->cursorPosX = x;
                self->cursorPosY = y;
                if (!self->cursorInside) {
                    TRACKMOUSEEVENT tme;
                    Memory::Clear(&tme, sizeof(tme));
                    tme.cbSize = sizeof(tme);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = self->hwnd;
                    TrackMouseEvent(&tme);
                    self->cursorInside = true;
                    self->inputCursorEnter(true);
                }
                return 0;
            }
            break;

        case WM_MOUSELEAVE:
            if (self) {
                self->cursorInside = false;
                self->inputCursorEnter(false);
                return 0;
            }
            break;

        case WM_MOUSEWHEEL:
            if (self) {
                self->inputScroll(0.0, (SHORT) HIWORD(wParam) / (double) ORYOL_D3D11_WHEEL_DELTA);
                return 0;
            }
            break;

        case WM_SIZE:
            if (self) {
                if (self->cursorMode == ORYOL_D3D11_CURSOR_DISABLED) {
                    self->updateClipRect();
                }
                if (!self->iconified && (wParam == SIZE_MINIMIZED)) {
                    self->iconified = true;
                    self->inputWindowIconify(true);
                }
                else if (self->iconified && ((wParam == SIZE_RESTORED) || (wParam == SIZE_MAXIMIZED))) {
                    self->iconified = false;
                    self->inputWindowIconify(false);
                }
                else {
                    self->onWindowResize(LOWORD(lParam), HIWORD(lParam));
                }
                self->inputFramebufferSize(LOWORD(lParam), HIWORD(lParam));
                self->inputWindowSize(LOWORD(lParam), HIWORD(lParam));
                return 0;
            }
            break;

        case WM_MOVE:
            if (self) {
                if (self->cursorMode == ORYOL_D3D11_CURSOR_DISABLED) {
                    self->updateClipRect();
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
                    if ((self->cursorMode == ORYOL_D3D11_CURSOR_HIDDEN) ||
                        (self->cursorMode == ORYOL_D3D11_CURSOR_DISABLED)) {
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
d3d11DisplayMgr::setupKeyTranslationTable() {
    Memory::Fill(this->publicKeys, sizeof(this->publicKeys), 0xFF);
    this->publicKeys[0x00B] = ORYOL_D3D11_KEY_0;
    this->publicKeys[0x002] = ORYOL_D3D11_KEY_1;
    this->publicKeys[0x003] = ORYOL_D3D11_KEY_2;
    this->publicKeys[0x004] = ORYOL_D3D11_KEY_3;
    this->publicKeys[0x005] = ORYOL_D3D11_KEY_4;
    this->publicKeys[0x006] = ORYOL_D3D11_KEY_5;
    this->publicKeys[0x007] = ORYOL_D3D11_KEY_6;
    this->publicKeys[0x008] = ORYOL_D3D11_KEY_7;
    this->publicKeys[0x009] = ORYOL_D3D11_KEY_8;
    this->publicKeys[0x00A] = ORYOL_D3D11_KEY_9;
    this->publicKeys[0x01E] = ORYOL_D3D11_KEY_A;
    this->publicKeys[0x030] = ORYOL_D3D11_KEY_B;
    this->publicKeys[0x02E] = ORYOL_D3D11_KEY_C;
    this->publicKeys[0x020] = ORYOL_D3D11_KEY_D;
    this->publicKeys[0x012] = ORYOL_D3D11_KEY_E;
    this->publicKeys[0x021] = ORYOL_D3D11_KEY_F;
    this->publicKeys[0x022] = ORYOL_D3D11_KEY_G;
    this->publicKeys[0x023] = ORYOL_D3D11_KEY_H;
    this->publicKeys[0x017] = ORYOL_D3D11_KEY_I;
    this->publicKeys[0x024] = ORYOL_D3D11_KEY_J;
    this->publicKeys[0x025] = ORYOL_D3D11_KEY_K;
    this->publicKeys[0x026] = ORYOL_D3D11_KEY_L;
    this->publicKeys[0x032] = ORYOL_D3D11_KEY_M;
    this->publicKeys[0x031] = ORYOL_D3D11_KEY_N;
    this->publicKeys[0x018] = ORYOL_D3D11_KEY_O;
    this->publicKeys[0x019] = ORYOL_D3D11_KEY_P;
    this->publicKeys[0x010] = ORYOL_D3D11_KEY_Q;
    this->publicKeys[0x013] = ORYOL_D3D11_KEY_R;
    this->publicKeys[0x01F] = ORYOL_D3D11_KEY_S;
    this->publicKeys[0x014] = ORYOL_D3D11_KEY_T;
    this->publicKeys[0x016] = ORYOL_D3D11_KEY_U;
    this->publicKeys[0x02F] = ORYOL_D3D11_KEY_V;
    this->publicKeys[0x011] = ORYOL_D3D11_KEY_W;
    this->publicKeys[0x02D] = ORYOL_D3D11_KEY_X;
    this->publicKeys[0x015] = ORYOL_D3D11_KEY_Y;
    this->publicKeys[0x02C] = ORYOL_D3D11_KEY_Z;
    this->publicKeys[0x028] = ORYOL_D3D11_KEY_APOSTROPHE;
    this->publicKeys[0x02B] = ORYOL_D3D11_KEY_BACKSLASH;
    this->publicKeys[0x033] = ORYOL_D3D11_KEY_COMMA;
    this->publicKeys[0x00D] = ORYOL_D3D11_KEY_EQUAL;
    this->publicKeys[0x029] = ORYOL_D3D11_KEY_GRAVE_ACCENT;
    this->publicKeys[0x01A] = ORYOL_D3D11_KEY_LEFT_BRACKET;
    this->publicKeys[0x00C] = ORYOL_D3D11_KEY_MINUS;
    this->publicKeys[0x034] = ORYOL_D3D11_KEY_PERIOD;
    this->publicKeys[0x01B] = ORYOL_D3D11_KEY_RIGHT_BRACKET;
    this->publicKeys[0x027] = ORYOL_D3D11_KEY_SEMICOLON;
    this->publicKeys[0x035] = ORYOL_D3D11_KEY_SLASH;
    this->publicKeys[0x056] = ORYOL_D3D11_KEY_WORLD_2;
    this->publicKeys[0x00E] = ORYOL_D3D11_KEY_BACKSPACE;
    this->publicKeys[0x153] = ORYOL_D3D11_KEY_DELETE;
    this->publicKeys[0x14F] = ORYOL_D3D11_KEY_END;
    this->publicKeys[0x01C] = ORYOL_D3D11_KEY_ENTER;
    this->publicKeys[0x001] = ORYOL_D3D11_KEY_ESCAPE;
    this->publicKeys[0x147] = ORYOL_D3D11_KEY_HOME;
    this->publicKeys[0x152] = ORYOL_D3D11_KEY_INSERT;
    this->publicKeys[0x15D] = ORYOL_D3D11_KEY_MENU;
    this->publicKeys[0x151] = ORYOL_D3D11_KEY_PAGE_DOWN;
    this->publicKeys[0x149] = ORYOL_D3D11_KEY_PAGE_UP;
    this->publicKeys[0x045] = ORYOL_D3D11_KEY_PAUSE;
    this->publicKeys[0x039] = ORYOL_D3D11_KEY_SPACE;
    this->publicKeys[0x00F] = ORYOL_D3D11_KEY_TAB;
    this->publicKeys[0x03A] = ORYOL_D3D11_KEY_CAPS_LOCK;
    this->publicKeys[0x145] = ORYOL_D3D11_KEY_NUM_LOCK;
    this->publicKeys[0x046] = ORYOL_D3D11_KEY_SCROLL_LOCK;
    this->publicKeys[0x03B] = ORYOL_D3D11_KEY_F1;
    this->publicKeys[0x03C] = ORYOL_D3D11_KEY_F2;
    this->publicKeys[0x03D] = ORYOL_D3D11_KEY_F3;
    this->publicKeys[0x03E] = ORYOL_D3D11_KEY_F4;
    this->publicKeys[0x03F] = ORYOL_D3D11_KEY_F5;
    this->publicKeys[0x040] = ORYOL_D3D11_KEY_F6;
    this->publicKeys[0x041] = ORYOL_D3D11_KEY_F7;
    this->publicKeys[0x042] = ORYOL_D3D11_KEY_F8;
    this->publicKeys[0x043] = ORYOL_D3D11_KEY_F9;
    this->publicKeys[0x044] = ORYOL_D3D11_KEY_F10;
    this->publicKeys[0x057] = ORYOL_D3D11_KEY_F11;
    this->publicKeys[0x058] = ORYOL_D3D11_KEY_F12;
    this->publicKeys[0x064] = ORYOL_D3D11_KEY_F13;
    this->publicKeys[0x065] = ORYOL_D3D11_KEY_F14;
    this->publicKeys[0x066] = ORYOL_D3D11_KEY_F15;
    this->publicKeys[0x067] = ORYOL_D3D11_KEY_F16;
    this->publicKeys[0x068] = ORYOL_D3D11_KEY_F17;
    this->publicKeys[0x069] = ORYOL_D3D11_KEY_F18;
    this->publicKeys[0x06A] = ORYOL_D3D11_KEY_F19;
    this->publicKeys[0x06B] = ORYOL_D3D11_KEY_F20;
    this->publicKeys[0x06C] = ORYOL_D3D11_KEY_F21;
    this->publicKeys[0x06D] = ORYOL_D3D11_KEY_F22;
    this->publicKeys[0x06E] = ORYOL_D3D11_KEY_F23;
    this->publicKeys[0x076] = ORYOL_D3D11_KEY_F24;
    this->publicKeys[0x038] = ORYOL_D3D11_KEY_LEFT_ALT;
    this->publicKeys[0x01D] = ORYOL_D3D11_KEY_LEFT_CONTROL;
    this->publicKeys[0x02A] = ORYOL_D3D11_KEY_LEFT_SHIFT;
    this->publicKeys[0x15B] = ORYOL_D3D11_KEY_LEFT_SUPER;
    this->publicKeys[0x137] = ORYOL_D3D11_KEY_PRINT_SCREEN;
    this->publicKeys[0x138] = ORYOL_D3D11_KEY_RIGHT_ALT;
    this->publicKeys[0x11D] = ORYOL_D3D11_KEY_RIGHT_CONTROL;
    this->publicKeys[0x036] = ORYOL_D3D11_KEY_RIGHT_SHIFT;
    this->publicKeys[0x15C] = ORYOL_D3D11_KEY_RIGHT_SUPER;
    this->publicKeys[0x150] = ORYOL_D3D11_KEY_DOWN;
    this->publicKeys[0x14B] = ORYOL_D3D11_KEY_LEFT;
    this->publicKeys[0x14D] = ORYOL_D3D11_KEY_RIGHT;
    this->publicKeys[0x148] = ORYOL_D3D11_KEY_UP;
    this->publicKeys[0x052] = ORYOL_D3D11_KEY_KP_0;
    this->publicKeys[0x04F] = ORYOL_D3D11_KEY_KP_1;
    this->publicKeys[0x050] = ORYOL_D3D11_KEY_KP_2;
    this->publicKeys[0x051] = ORYOL_D3D11_KEY_KP_3;
    this->publicKeys[0x04B] = ORYOL_D3D11_KEY_KP_4;
    this->publicKeys[0x04C] = ORYOL_D3D11_KEY_KP_5;
    this->publicKeys[0x04D] = ORYOL_D3D11_KEY_KP_6;
    this->publicKeys[0x047] = ORYOL_D3D11_KEY_KP_7;
    this->publicKeys[0x048] = ORYOL_D3D11_KEY_KP_8;
    this->publicKeys[0x049] = ORYOL_D3D11_KEY_KP_9;
    this->publicKeys[0x04E] = ORYOL_D3D11_KEY_KP_ADD;
    this->publicKeys[0x053] = ORYOL_D3D11_KEY_KP_DECIMAL;
    this->publicKeys[0x135] = ORYOL_D3D11_KEY_KP_DIVIDE;
    this->publicKeys[0x11C] = ORYOL_D3D11_KEY_KP_ENTER;
    this->publicKeys[0x037] = ORYOL_D3D11_KEY_KP_MULTIPLY;
    this->publicKeys[0x04A] = ORYOL_D3D11_KEY_KP_SUBTRACT;
}

} // namespace _priv
} // namespace Oryol
