#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pnaclDisplayMgr
    @ingroup _priv
    @brief PNaCl implementation of displayMgr
*/
#include "Gfx/Core/displayMgrBase.h"
#include "Gfx/gl/gl_decl.h"
#include "Core/pnacl/pnaclInstance.h"

namespace Oryol {
namespace _priv {

class pnaclDisplayMgr : public displayMgrBase {
public:
    /// constructor
    pnaclDisplayMgr();
    /// destructor
    ~pnaclDisplayMgr();
    
    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& gfxSetup);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// present the current rendered frame
    void Present();
    
    /// bind the default frame buffer
    void glBindDefaultFramebuffer();

private:
    int32 glFramebufferWidth;
    int32 glFramebufferHeight;
};

} // namespace _priv
} // namespace Oryol
