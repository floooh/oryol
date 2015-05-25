//------------------------------------------------------------------------------
//  d3d11DisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11DisplayMgr.h"
#include "d3d11Types.h"
#include "Core/String/StringConverter.h"
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

static LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static d3d11DisplayMgr* self = nullptr;

//------------------------------------------------------------------------------
d3d11DisplayMgr::d3d11DisplayMgr() :
d3d11Device(nullptr),
d3d11DeviceContext(nullptr),
dxgiSwapChain(nullptr),
backBuffer(nullptr),
renderTargetView(nullptr),
depthStencilBuffer(nullptr),
depthStencilView(nullptr),
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
    DXGI_SWAP_CHAIN_DESC inOutSwapChainDesc;
    this->createDeviceAndSwapChain(setup, inOutSwapChainDesc);
    this->createDefaultRenderTarget(setup, inOutSwapChainDesc);
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
d3d11DisplayMgr::createDeviceAndSwapChain(const GfxSetup& setup, DXGI_SWAP_CHAIN_DESC& outSwapChainDesc) {
    o_assert_dbg(nullptr == this->d3d11Device);
    o_assert_dbg(nullptr == this->d3d11DeviceContext);
    o_assert_dbg(nullptr == this->dxgiSwapChain);
    o_assert_dbg(0 != this->hwnd);

    int createFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
    #if ORYOL_DEBUG
        createFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    Memory::Clear(&outSwapChainDesc, sizeof(outSwapChainDesc));
    outSwapChainDesc.BufferDesc.Width = setup.Width;
    outSwapChainDesc.BufferDesc.Height = setup.Height;
    outSwapChainDesc.BufferDesc.Format = d3d11Types::asSwapChainFormat(setup.ColorFormat);
    outSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    outSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    outSwapChainDesc.SampleDesc.Count = setup.Samples < 2 ? 1 : setup.Samples;
    outSwapChainDesc.SampleDesc.Quality = setup.Samples < 2 ? 0 : D3D11_STANDARD_MULTISAMPLE_PATTERN;
    outSwapChainDesc.BufferCount = 1;
    outSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    outSwapChainDesc.OutputWindow = this->hwnd;
    outSwapChainDesc.Windowed = setup.Windowed;
    outSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    outSwapChainDesc.Flags = 0;

    D3D_FEATURE_LEVEL d3dFeatureLevel;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        NULL,                           // pAdapter (use default adapter)
        D3D_DRIVER_TYPE_HARDWARE,       // DriverType
        NULL,                           // Software
        createFlags,                    // Flags
        NULL,                           // pFeatureLevels
        0,                              // FeatureLevels
        D3D11_SDK_VERSION,              // SDKVersion
        &outSwapChainDesc,                 // pSwapChainDesc
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
d3d11DisplayMgr::createDefaultRenderTarget(const GfxSetup& setup, const DXGI_SWAP_CHAIN_DESC& swapChainDesc) {
    o_assert_dbg(this->d3d11Device);
    o_assert_dbg(this->dxgiSwapChain);
    o_assert_dbg(nullptr == this->backBuffer);
    o_assert_dbg(nullptr == this->renderTargetView);
    o_assert_dbg(nullptr == this->depthStencilBuffer);
    o_assert_dbg(nullptr == this->depthStencilView);
    HRESULT hr;

    // setup color buffer (obtain from swap chain and create render target view)
    hr = this->dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &this->backBuffer);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(this->backBuffer);
    hr = this->d3d11Device->CreateRenderTargetView(this->backBuffer, NULL, &this->renderTargetView);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(this->renderTargetView);

    // setup depth/stencil buffer (if required)
    if (PixelFormat::None != setup.DepthFormat) {
        
        D3D11_TEXTURE2D_DESC depthStencilDesc;
        Memory::Clear(&depthStencilDesc, sizeof(depthStencilDesc));
        depthStencilDesc.Width = setup.Width;
        depthStencilDesc.Height = setup.Height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = d3d11Types::asTextureFormat(setup.DepthFormat);
        depthStencilDesc.SampleDesc = swapChainDesc.SampleDesc;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;
        hr = this->d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &this->depthStencilBuffer);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(this->depthStencilBuffer);

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        Memory::Clear(&dsvDesc, sizeof(dsvDesc));
        dsvDesc.Format = depthStencilDesc.Format;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;
        hr = this->d3d11Device->CreateDepthStencilView(this->depthStencilBuffer, &dsvDesc, &this->depthStencilView);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(this->depthStencilView);
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::destroyDefaultRenderTarget() {
    if (this->backBuffer) {
        this->backBuffer->Release();
        this->backBuffer = nullptr;
    }
    if (this->renderTargetView) {
        this->renderTargetView->Release();
        this->renderTargetView = nullptr;
    }
    if (this->depthStencilBuffer) {
        this->depthStencilBuffer->Release();
        this->depthStencilBuffer = nullptr;
    }
    if (this->depthStencilView) {
        this->depthStencilView->Release();
        this->depthStencilView = nullptr;
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

    DestroyWindow((HWND)this->hwnd);
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
winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
