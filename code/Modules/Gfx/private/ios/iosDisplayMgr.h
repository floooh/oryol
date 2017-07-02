#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::iosDisplayMgr
    @ingroup _priv
    @brief display manager for IOS
*/
#include "Gfx/private/displayMgrBase.h"
#include "Gfx/private/gl/gl_decl.h"
#include "Core/private/ios/iosBridge.h"

namespace Oryol {
namespace _priv {

class iosDisplayMgr : public displayMgrBase {
public:
    /// constructor
    iosDisplayMgr();
    /// destructor
    ~iosDisplayMgr();
    
    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& gfxSetup, const gfxPointers& ptrs);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// present the current rendered frame
    void Present();
    
    /// bind the default frame buffer
    void glBindDefaultFramebuffer();

    static iosDisplayMgr* self;
    GLuint glDefaultFramebuffer;
    GLint glFramebufferWidth;
    GLint glFramebufferHeight;
};
    
} // namespace _priv
} // namespace Oryol
