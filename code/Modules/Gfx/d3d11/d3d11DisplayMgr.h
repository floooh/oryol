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

    bool quitRequested;
    HWND hwnd;
    DWORD dwStyle;
    DWORD dwExStyle;
};

} // namespace _priv
} // namespace Oryol

