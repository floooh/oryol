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
#include <d3d11.h>

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

    /// get pointer to d3d11 device
    ID3D11Device* getD3D11Device() const;
    /// get pointer to d3d11 device context
    ID3D11DeviceContext* getD3D11DeviceContext() const;
    /// get pointer to swap chain
    IDXGISwapChain* getDXGISwapChain() const;
    /// get pointer to render target view (back buffer render target)
    ID3D11RenderTargetView* getRenderTargetView() const;
    /// get pointer to depth/stencil buffer view of default render target
    ID3D11DepthStencilView* getDepthStencilView() const;
    
    /// pointer to d3d11 device
    ID3D11Device* d3d11Device;
    /// pointer to immediate mode device context
    ID3D11DeviceContext* d3d11DeviceContext;
    /// pointer to swap chain
    IDXGISwapChain* dxgiSwapChain;
    /// d3d feature level
    D3D_FEATURE_LEVEL featureLevel;
    /// pointer to default render target back buffer texture
    ID3D11Texture2D* backBuffer;
    /// pointer to default render target view
    ID3D11RenderTargetView* renderTargetView;
    /// pointer to default depth/stencil buffer texture
    ID3D11Texture2D* depthStencilBuffer;
    /// pointer to default default depth/stencil view
    ID3D11DepthStencilView* depthStencilView;

private:
    /// register the window class
    void registerWindowClass();
    /// unregister the window class
    void unregisterWindowClass();
    /// create the application window
    void createWindow(const GfxSetup& gfxSetup);
    /// destroy the application window
    void destroyWindow();
    /// create swap chain and d3d device
    void createDeviceAndSwapChain(const GfxSetup& gfxSetup);
    /// destroy the d3d device and swap chain
    void destroyDeviceAndSwapChain();
    /// create the default render target
    void createDefaultRenderTarget(const GfxSetup& gfxSetup);
    /// destroy the default render target
    void destroyDefaultRenderTarget();
    /// compute actual window size from client rect size plus window chrome
    void computeWindowSize(int clientWidth, int clientHeight, int& outWidth, int& outHeight);
    /// custom winproc
    static LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool quitRequested;
    HWND hwnd;
    DWORD dwStyle;
    DWORD dwExStyle;
    
    DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
    D3D11_TEXTURE2D_DESC depthStencilDesc;
};

//------------------------------------------------------------------------------
inline ID3D11Device*
d3d11DisplayMgr::getD3D11Device() const {
    o_assert_dbg(this->d3d11Device);
    return this->d3d11Device;
}

//------------------------------------------------------------------------------
inline ID3D11DeviceContext*
d3d11DisplayMgr::getD3D11DeviceContext() const {
    o_assert_dbg(this->d3d11DeviceContext);
    return this->d3d11DeviceContext;
}

//------------------------------------------------------------------------------
inline IDXGISwapChain*
d3d11DisplayMgr::getDXGISwapChain() const {
    o_assert_dbg(this->dxgiSwapChain);
    return this->dxgiSwapChain;
}

} // namespace _priv
} // namespace Oryol

