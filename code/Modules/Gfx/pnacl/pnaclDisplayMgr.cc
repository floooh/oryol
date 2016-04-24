//------------------------------------------------------------------------------
//  pnaclDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pnaclDisplayMgr.h"
#include "Gfx/gl/glInfo.h"
#include "Gfx/gl/glExt.h"
#include "Gfx/gl/gl_impl.h"
#include "Core/String/StringBuilder.h"
#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"

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
pnaclDisplayMgr::SetupDisplay(const GfxSetup& gfxSetup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());

    Log::Info("pnaclDisplayMgr::SetupDisplay() called!\n");
    displayMgrBase::SetupDisplay(gfxSetup, ptrs);

    // const int32_t numSampleBuffers = gfxSetup.Samples > 1 ? 1 : 0;
    const int32_t attribList[] = {
        PP_GRAPHICS3DATTRIB_RED_SIZE, PixelFormat::NumBits(gfxSetup.ColorFormat, PixelChannel::Red),
        PP_GRAPHICS3DATTRIB_GREEN_SIZE, PixelFormat::NumBits(gfxSetup.ColorFormat, PixelChannel::Green),
        PP_GRAPHICS3DATTRIB_BLUE_SIZE, PixelFormat::NumBits(gfxSetup.ColorFormat, PixelChannel::Blue),
        PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 0,
        PP_GRAPHICS3DATTRIB_DEPTH_SIZE, PixelFormat::NumBits(gfxSetup.DepthFormat, PixelChannel::Depth),
        PP_GRAPHICS3DATTRIB_STENCIL_SIZE, PixelFormat::NumBits(gfxSetup.DepthFormat, PixelChannel::Stencil),
        PP_GRAPHICS3DATTRIB_WIDTH, gfxSetup.Width,
        PP_GRAPHICS3DATTRIB_HEIGHT, gfxSetup.Height,
        /*
        MSAA currently broken: https://code.google.com/p/chromium/issues/detail?id=285475
        PP_GRAPHICS3DATTRIB_SAMPLES, gfxSetup.Samples,
        PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, numSampleBuffers,
        */
        PP_GRAPHICS3DATTRIB_SWAP_BEHAVIOR, PP_GRAPHICS3DATTRIB_BUFFER_DESTROYED,
        PP_GRAPHICS3DATTRIB_NONE
    };
    pnaclInstance::Instance()->initGL(attribList);

    // check if we need to resize the canvas
    const int reqWidth = gfxSetup.Width;
    const int reqHeight = gfxSetup.Height;
    const int canvasWidth = pnaclInstance::Instance()->GetCanvasWidth();
    const int canvasHeight = pnaclInstance::Instance()->GetCanvasHeight();
    if ((reqWidth != canvasWidth) || (reqHeight != canvasHeight)) {
        this->requestCanvasResize(reqWidth, reqHeight);
    }

    // setup platform constants and extensions
    glInfo::Setup();
    glExt::Setup();
    
    // NOTE: we put in the requested framebuffer size regardless of the
    // actual size, the framebuffer will be resized in the next frame
    // on the web page
    this->glFramebufferWidth = reqWidth; 
    this->glFramebufferHeight = reqHeight;
    this->displayAttrs.FramebufferWidth = this->glFramebufferWidth;
    this->displayAttrs.FramebufferHeight = this->glFramebufferHeight;
    this->displayAttrs.WindowWidth = this->glFramebufferWidth;
    this->displayAttrs.WindowHeight = this->glFramebufferHeight;

    // register DidChangeView event handler
    Log::Info("registering DidChangeView handler\n");
    pnaclInstance::Instance()->enableViewEvents([this] (const pp::View& e) {
        return this->handleViewEvent(e);
    });
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

//------------------------------------------------------------------------------
void
pnaclDisplayMgr::requestCanvasResize(int newWidth, int newHeight) {
    StringBuilder strBuilder;
    strBuilder.Format(128, "{\"msg\":\"resize\", \"w\":%d, \"h\":%d}", newWidth, newHeight);
    pnaclInstance::Instance()->putMsg(strBuilder.AsCStr());
}

//------------------------------------------------------------------------------
bool
pnaclDisplayMgr::handleViewEvent(const pp::View& view) {

    const int newWidth = pnaclInstance::Instance()->GetCanvasWidth();
    const int newHeight = pnaclInstance::Instance()->GetCanvasHeight();
    if ((newWidth != this->glFramebufferWidth) || (newHeight != this->glFramebufferHeight)) {

        this->glFramebufferWidth = newWidth;
        this->glFramebufferHeight = newHeight;

        // update display attributes
        this->displayAttrs.FramebufferWidth = this->glFramebufferWidth;
        this->displayAttrs.FramebufferHeight = this->glFramebufferHeight;
        this->displayAttrs.WindowWidth = this->glFramebufferWidth;
        this->displayAttrs.WindowHeight = this->glFramebufferHeight;
    
        Log::Info("framebuffer size changed to w=%d, h=%d\n", newWidth, newHeight);
    }
    return true;
}

} // namespace _priv
} // namespace Oryol
