#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11DisplayMgr
    @ingroup _priv
    @brief display manager implementation for D3D11
*/
#include "Gfx/private/winDisplayMgr.h"

// d3d11 forward declarations
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

namespace Oryol {
namespace _priv {

class d3d11DisplayMgr : public winDisplayMgr {
public:
    /// constructor
    d3d11DisplayMgr();
    /// destructor
    ~d3d11DisplayMgr();
    /// static singleton ptr
    static d3d11DisplayMgr* ptr;

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxDesc& desc);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// present the current rendered frame
    void Present();

    /// pointer to d3d11 device
    ID3D11Device* d3d11Device = nullptr;
    /// pointer to immediate mode device context
    ID3D11DeviceContext* d3d11DeviceContext = nullptr;
    /// pointer to swap chain
    IDXGISwapChain* dxgiSwapChain = nullptr;
    /// pointer to default render target back buffer texture
    ID3D11Texture2D* d3d11RenderTarget = nullptr;
    /// pointer to default render target view
    ID3D11RenderTargetView* d3d11RenderTargetView = nullptr;
    /// pointer to default depth/stencil buffer texture
    ID3D11Texture2D* d3d11DepthStencilBuffer = nullptr;
    /// pointer to default default depth/stencil view
    ID3D11DepthStencilView* d3d11DepthStencilView = nullptr;

    /// static callback function to get current render-target-view as void*
    static const void* d3d11GetRenderTargetView() {
        return d3d11DisplayMgr::ptr->d3d11RenderTargetView;
    }
    /// static callback function to get current depth-stencil view as void*
    static const void* d3d11GetDepthStencilView() {
        return d3d11DisplayMgr::ptr->d3d11DepthStencilView;
    }

    /// create swap chain and d3d device
    void createDeviceAndSwapChain();
    /// destroy the d3d device and swap chain
    void destroyDeviceAndSwapChain();
    /// create the default render target
    void createDefaultRenderTarget(int width, int height);
    /// destroy the default render target
    void destroyDefaultRenderTarget();
    /// called from windowResize when window did actually resize
    virtual void onWindowDidResize();
};

} // namespace _priv
} // namespace Oryol

