#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::eglDisplayMgr
    @ingroup _priv
    @brief display manager class for EGL platforms
*/
#include "Gfx/private/displayMgrBase.h"
#include "Gfx/private/gl.h"
#include <EGL/egl.h>

namespace Oryol {
namespace _priv {

class eglDisplayMgr : public displayMgrBase {
public:
    /// destructor
    ~eglDisplayMgr();

    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxDesc& desc);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// present the current rendered frame
    void Present();
    
    bool useGLES2 = false;
    EGLDisplay eglDisplay = nullptr;
    EGLConfig eglConfig = nullptr;
    EGLSurface eglSurface = nullptr;
    EGLContext eglContext = nullptr;
};

} // namespace _priv
} // namespace Oryol
