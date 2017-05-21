#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::emscDisplayMgr
    @ingroup _priv
    @brief display manager class for emscripten platform
*/
#include "Gfx/private/displayMgrBase.h"
#include <emscripten/html5.h>

namespace Oryol {

// emscripten-specific soft-fullscreen control functions
extern "C" {
extern void enter_soft_fullscreen();
extern void leave_soft_fullscreen();
extern bool is_soft_fullscreen_active();
}

namespace _priv {

class emscDisplayMgr : public displayMgrBase {
public:
    /// constructor
    emscDisplayMgr();
    /// destructor
    ~emscDisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& gfxSetup, const gfxPointers& ptrs);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    
    /// bind the default frame buffer
    void glBindDefaultFramebuffer();
    /// emscripten callback when canvas size has changed (for soft-fullscreen)
    static EM_BOOL emscCanvasSizeChanged(int eventType, const void* reserved, void* userData);
    /// emscripten callback when window size has changed (for HTMLUseCanvasSize)
    static EM_BOOL emscWindowSizeChanged(int eventType, const EmscriptenUiEvent* uiEvent, void* userData);

    static emscDisplayMgr* self;
    int storedCanvasWidth;
    int storedCanvasHeight;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;
};

} // namespace _priv
} // namespace Oryol
