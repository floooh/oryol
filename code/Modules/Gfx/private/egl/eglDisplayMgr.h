#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::eglDisplayMgr
    @ingroup _priv
    @brief display manager class for EGL platforms
*/
#include "Gfx/private/displayMgrBase.h"
#include "Gfx/private/gl/gl_decl.h"
#include <EGL/egl.h>

namespace Oryol {
namespace _priv {

class eglDisplayMgr : public displayMgrBase {
public:
    /// constructor
    eglDisplayMgr();
    /// destructor
    ~eglDisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& GfxSetup, const gfxPointers& ptrs);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// present the current rendered frame
    void Present();
    
    /// bind the default frame buffer
    void glBindDefaultFramebuffer();
    
    EGLDisplay eglDisplay;
    EGLConfig eglConfig;
    EGLSurface eglSurface;
    EGLContext eglContext;
};

} // namespace _priv
} // namespace Oryol
