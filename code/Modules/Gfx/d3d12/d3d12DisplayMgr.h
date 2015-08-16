#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12DisplayMgr.h
    @ingroup _priv
    @brief display manager implementation for d3d12 renderer
*/
#include "Gfx/Core/displayMgrBase.h"

namespace Oryol {
namespace _priv {

class d3d12DisplayMgr : public displayMgrBase {
public:
    /// constructor
    d3d12DisplayMgr();
    /// destructor
    ~d3d12DisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& gfxSetup, const gfxPointers& ptrs);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// process window system events (call near start of frame)
    void ProcessSystemEvents();
    /// present the current rendered frame
    void Present();
    /// check whether the window system requests to quit the application
    bool QuitRequested() const;
};

} // namespace _priv
} // namespace Oryol

