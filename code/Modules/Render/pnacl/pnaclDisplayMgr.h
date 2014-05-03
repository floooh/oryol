#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::pnaclDisplayMgr
    @brief PNaCl implementation of displayMgr
*/
#include "Render/base/displayMgrBase.h"
#include "Render/gl/gl_decl.h"
#include "Core/pnacl/pnaclInstance.h"

namespace Oryol {
namespace Render {

class pnaclDisplayMgr : public displayMgrBase {
public:
    /// constructor
    pnaclDisplayMgr();
    /// destructor
    ~pnaclDisplayMgr();
    
    /// setup the display system, must happen before rendering
    void SetupDisplay(const RenderSetup& renderSetup);
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

} // namespace Render
} // namespace Oryol
