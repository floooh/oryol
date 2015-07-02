#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlDisplayMgr
    @ingroup _priv
    @brief Metal implementation of displayMgr
*/
#include "Gfx/Core/displayMgrBase.h"
#include "Gfx/mtl/cocoa.h"

namespace Oryol {
namespace _priv {

class mtlDisplayMgr : public displayMgrBase {
public:
    /// constructor
    mtlDisplayMgr();
    /// destructor
    ~mtlDisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// process window system events (call near start of frame)
    void ProcessSystemEvents();
    /// present the current rendered frame
    void Present();
    /// check whether the window system requests to quit the application
    bool QuitRequested() const;

    /// create the app window
    void createWindow(const GfxSetup& setup);
    /// destroy the app window
    void destroyWindow();
    /// create the depth and stencil buffers
    void createDepthStencilBuffer(int width, int height);
    /// destroy the depth and stencil buffers
    void destroyDepthStencilBuffer();
    /// create MSAA render buffer
    void createMSAABuffer(int width, int height);
    /// destroy MSAA render buffer
    void destroyMSAABuffer();

    class cocoa cocoa;
    ORYOL_OBJC_TYPED_ID(MTLTexture) depthStencilBuffer;
    ORYOL_OBJC_TYPED_ID(MTLTexture) msaaBuffer;
};

} // namespace _priv
} // namespace Oryol


