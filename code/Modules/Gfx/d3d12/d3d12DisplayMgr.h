#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12DisplayMgr.h
    @ingroup _priv
    @brief display manager implementation for d3d12 renderer
*/
#include "Gfx/win/winDisplayMgr.h"

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

    /// create swap chain and d3d device
    void createDeviceAndSwapChain();
    /// destroy the d3d device and swap chain
    void destroyDeviceAndSwapChain();
    /// enable the debug layer (only if compiled with ORYOL_DEBUG)
    void enableDebugLayer();

};

} // namespace _priv
} // namespace Oryol

