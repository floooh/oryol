//------------------------------------------------------------------------------
//  pnaclDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pnaclDisplayMgr.h"
#include "Gfx/gl/glInfo.h"
#include "Gfx/gl/glExt.h"
#include "Gfx/gl/gl_impl.h"

namespace Oryol {
namespace _priv {

using namespace _priv;

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
pnaclDisplayMgr::SetupDisplay(const GfxSetup& gfxSetup) {
    o_assert(!this->IsDisplayValid());

    Log::Info("pnaclDisplayMgr::SetupDisplay() called!\n");
    displayMgrBase::SetupDisplay(gfxSetup);

    // FIXME: create a new Graphics3D context with the actually 
    // requested setup attributes

    // setup platform constants and extensions
    glInfo::Setup();
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
    glExt::Discard();
    glInfo::Discard();
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

} // namespace _priv
} // namespace Oryol