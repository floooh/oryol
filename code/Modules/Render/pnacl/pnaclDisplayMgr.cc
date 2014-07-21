//------------------------------------------------------------------------------
//  pnaclDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pnaclDisplayMgr.h"
#include "Render/gl/glInfo.h"
#include "Render/gl/glExt.h"
#include "Render/gl/gl_impl.h"

namespace Oryol {
namespace Render {

using namespace Core;
    
//------------------------------------------------------------------------------
pnaclDisplayMgr::pnaclDisplayMgr() :
glFramebufferWidth(0),
glFramebufferHeight(0) {
    // empty
}

//------------------------------------------------------------------------------
pnaclDisplayMgr::~pnaclDisplayMgr() {
    // empty
}

//------------------------------------------------------------------------------
void
pnaclDisplayMgr::SetupDisplay(const RenderSetup& renderSetup) {
    o_assert(!this->IsDisplayValid());

    Log::Info("pnaclDisplayMgr::SetupDisplay() called!\n");
    displayMgrBase::SetupDisplay(renderSetup);

    // FIXME: create a new Graphics3D context with the actually 
    // requested setup attributes

    // dump GL information
    glInfo::PrintInfo();

    // setup extensions
    glExt::Setup();
    
    // update the displayAttrs with the actual frame buffer size
    this->glFramebufferWidth = pnaclInstance::Instance()->GetFramebufferWidth();
    this->glFramebufferHeight = pnaclInstance::Instance()->GetFramebufferHeight();
    Log::Info("pnaclDisplayMgr: actual framebuffer size w=%d, h=%d\n", this->glFramebufferWidth, this->glFramebufferHeight);
    this->displayAttrs.FramebufferWidth = this->glFramebufferWidth;
    this->displayAttrs.FramebufferHeight = this->glFramebufferHeight;
    this->displayAttrs.WindowWidth = this->glFramebufferWidth;
    this->displayAttrs.WindowHeight = this->glFramebufferHeight;
}

//------------------------------------------------------------------------------
void
pnaclDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    this->glFramebufferWidth = 0;
    this->glFramebufferHeight = 0;
    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
pnaclDisplayMgr::Present() {
    // swap buffer happens outside of the Oryol frame in pnaclInstance
    displayMgrBase::Present();
}

//------------------------------------------------------------------------------
void
pnaclDisplayMgr::glBindDefaultFramebuffer() {
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ORYOL_GL_CHECK_ERROR();
}

} // namespace Render
} // namespace Oryol