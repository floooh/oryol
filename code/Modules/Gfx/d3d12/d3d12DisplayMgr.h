#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12DisplayMgr.h
    @ingroup _priv
    @brief display manager implementation for d3d12 renderer
*/
#include "Gfx/win/winDisplayMgr.h"
#include "Gfx/d3d12/d3d12_decl.h"

namespace Oryol {
namespace _priv {

class d3d12DisplayMgr : public winDisplayMgr {
public:
    /// constructor
    d3d12DisplayMgr();
    /// destructor
    ~d3d12DisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& gfxSetup, const gfxPointers& ptrs);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// present the current rendered frame
    void Present();

    /// pointer to dxgi factory
    IDXGIFactory4* dxgiFactory;
    /// pointer to d3d12 device
    ID3D12Device* d3d12Device;
    /// pointer to d3d12 command queue
    ID3D12CommandQueue* d3d12CommandQueue;
    /// pointer to swap chain object
    IDXGISwapChain3* dxgiSwapChain; 

    /// called from windowResize when window did actually resize
    virtual void onWindowDidResize();

    /// create the dxgi factory object (must be called first)
    void createDXGIFactory();
    /// create d3d device and command queue
    void createDeviceObjects();
    /// create dxgi swapchain object
    void createSwapChain();
    /// destory the dxgi factory object
    void destroyDXGIFactory();
    /// destroy d3d device and command queue
    void destroyDeviceObjects();
    /// destroy swapchain object
    void destroySwapChain();
    /// enable the debug layer (only if compiled with ORYOL_DEBUG)
    void enableDebugLayer();

};

} // namespace _priv
} // namespace Oryol

