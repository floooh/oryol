#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlDisplayMgr
    @ingroup _priv
    @brief Metal implementation of displayMgr
*/
#include "Gfx/private/displayMgrBase.h"

namespace Oryol {
namespace _priv {

class mtlDisplayMgr : public displayMgrBase {
public:
    /// constructor
    mtlDisplayMgr();
    /// destructor
    ~mtlDisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxDesc& desc);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// check whether the window system requests to quit the application
    bool QuitRequested() const;

    /// configure the app window
    void configureWindow(const GfxDesc& desc);
    /// callback for window-resize
    #if ORYOL_MACOS
    static void onFramebufferSize(int w, int h);
    #endif
    /// get Metal device as bridged pointer
    static const void* mtlDevice();
    /// get a new MTLRenderPassDescriptor as bridged pointer
    static const void* mtlRenderPassDescriptor();
    /// get a new MTLDrawable as bridged pointer
    static const void* mtlDrawable();

    /// ptr to self for onFramebufferSize
    static mtlDisplayMgr* self;
};

} // namespace _priv
} // namespace Oryol


