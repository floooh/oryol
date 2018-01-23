#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::iosDisplayMgr
    @ingroup _priv
    @brief display manager for IOS
*/
#include "Gfx/private/displayMgrBase.h"
#include "Gfx/private/gl.h"

namespace Oryol {
namespace _priv {

class iosDisplayMgr : public displayMgrBase {
public:
    /// constructor
    iosDisplayMgr();
    /// destructor
    ~iosDisplayMgr();
    
    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxDesc& desc);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// present the current rendered frame
    void Present();
    
    static iosDisplayMgr* self;
    bool useGLES2 = false;
    GLint glFramebufferWidth = 0;
    GLint glFramebufferHeight = 0;
};
    
} // namespace _priv
} // namespace Oryol
