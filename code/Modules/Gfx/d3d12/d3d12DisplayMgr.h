#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12DisplayMgr.h
    @ingroup _priv
    @brief display manager implementation for d3d12 renderer
*/
#include "Gfx/win/winDisplayMgr.h"
#include "Gfx/d3d12/d3d12_decl.h"
#include "Gfx/d3d12/d3d12Config.h"
#include "Core/Containers/StaticArray.h"

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

    /// called from windowResize when window did actually resize
    virtual void onWindowDidResize();

    /// create the dxgi factory object (must be called first)
    void createDXGIFactory();
    /// destory the dxgi factory object
    void destroyDXGIFactory();
    /// create d3d device and command queue
    void createDeviceObjects();
    /// destroy d3d device and command queue
    void destroyDeviceObjects();
    /// create dxgi swapchain object
    void createSwapChain();
    /// destroy swapchain object
    void destroySwapChain();
    /// enable the debug layer (only if compiled with ORYOL_DEBUG)
    void enableDebugLayer();
    /// create the default (backbuffer) render target 
    void createDefaultRenderTarget(int width, int height);
    /// destroy the default render target
    void destroyDefaultRenderTarget();

    IDXGIFactory4* dxgiFactory;
    ID3D12Device* d3d12Device;
    ID3D12CommandQueue* d3d12CommandQueue;
    IDXGISwapChain3* dxgiSwapChain;
    ID3D12DescriptorHeap* d3d12RTVHeap;
    StaticArray<ID3D12Resource*, d3d12Config::NumFrames> d3d12RenderTargets;

    int32 rtvDescriptorSize;    
    uint32 curBackbufferIndex;
};

} // namespace _priv
} // namespace Oryol

