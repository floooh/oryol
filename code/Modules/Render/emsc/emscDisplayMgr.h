#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::emscDisplayMgr
    @brief display manager class for emscripten platform
*/
#include "Render/base/displayMgrBase.h"
#include <emscripten/html5.h>

namespace Oryol {
namespace Render {

class emscDisplayMgr : public displayMgrBase {
public:
    /// constructor
    emscDisplayMgr();
    /// destructor
    ~emscDisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const RenderSetup& renderSetup);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    
    /// bind the default frame buffer
    void glBindDefaultFramebuffer();

private:    
    /// emscripten callback for switching fullscreen mode
    static EM_BOOL emscFullscreenChanged(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData);

    int storedCanvasWidth;
    int storedCanvasHeight;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;
};

} // namespace Render
} // namespace Oryol
