//------------------------------------------------------------------------------
//  iosDisplayMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "iosDisplayMgr.h"
#include "Gfx/private/gl/gl_impl.h"
#include "Gfx/private/gl/glCaps.h"
#include <GLKit/GLKit.h>

namespace Oryol {
namespace _priv {

iosDisplayMgr* iosDisplayMgr::self = nullptr;

//------------------------------------------------------------------------------
iosDisplayMgr::iosDisplayMgr() :
glDefaultFramebuffer(0),
glFramebufferWidth(0),
glFramebufferHeight(0) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
iosDisplayMgr::~iosDisplayMgr() {
    o_assert(nullptr != self);
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
    self = nullptr;
}

//------------------------------------------------------------------------------
void
iosDisplayMgr::SetupDisplay(const GfxSetup& gfxSetup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());

    Log::Info("iosDisplayMgr::SetupDisplay() called!\n");
    displayMgrBase::SetupDisplay(gfxSetup, ptrs);
    
    // modify the color/depth/stencil format and content scaling of the GLKView
    GLKView* glkView = _priv::iosBridge::ptr()->glkView;
    if (gfxSetup.HighDPI) {
        glkView.contentScaleFactor = 2.0f;
    }
    else {
        glkView.contentScaleFactor = 1.0f;
    }
    switch (gfxSetup.ColorFormat) {
        case PixelFormat::R5G6B5:
            glkView.drawableColorFormat = GLKViewDrawableColorFormatRGB565;
            break;
            
        case PixelFormat::RGB8:
        case PixelFormat::RGBA8:
            glkView.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
            break;
            
        default:
            Log::Warn("iosDisplayMgr: requested color format not supported, falling back to R5G6B5!\n");
            glkView.drawableColorFormat = GLKViewDrawableColorFormatRGB565;
            break;
    }
    switch (gfxSetup.DepthFormat) {
        case PixelFormat::None:
            glkView.drawableDepthFormat = GLKViewDrawableDepthFormatNone;
            glkView.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
            
        case PixelFormat::DEPTH:
            glkView.drawableDepthFormat = GLKViewDrawableDepthFormat16;
            glkView.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
            
        case PixelFormat::DEPTHSTENCIL:
            glkView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
            glkView.drawableStencilFormat = GLKViewDrawableStencilFormat8;
            break;
            
        default:
            Log::Warn("iosDisplayMgr: requested depth format not supported, falling back to D16!\n");
            glkView.drawableDepthFormat = GLKViewDrawableDepthFormat16;
            glkView.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
    }
    /*
    FIXME: this doesn't work, find out why
    if (gfxSetup.Samples > 0) {
        glkView.drawableMultisample = GLKViewDrawableMultisample4X;
    }
    else {
        glkView.drawableMultisample = GLKViewDrawableMultisampleNone;
    }
    */
    if ([glkView.context API] == kEAGLRenderingAPIOpenGLES2) {
        glCaps::Setup(glCaps::GLES2);
    } else {
        glCaps::Setup(glCaps::GLES3);
    }
    
    
    // update the displayAttrs with the actual frame buffer size
    this->glFramebufferWidth = (int) glkView.drawableWidth;
    this->glFramebufferHeight = (int) glkView.drawableHeight;
    Log::Info("iosDisplayMgr: actual framebuffer size w=%d, h=%d\n", this->glFramebufferWidth, this->glFramebufferHeight);
    this->displayAttrs.FramebufferWidth = this->glFramebufferWidth;
    this->displayAttrs.FramebufferHeight = this->glFramebufferHeight;
    this->displayAttrs.WindowWidth = this->glFramebufferWidth;
    this->displayAttrs.WindowHeight = this->glFramebufferHeight;
}

//------------------------------------------------------------------------------
void
iosDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    this->glDefaultFramebuffer = 0;
    this->glFramebufferWidth = 0;
    this->glFramebufferHeight = 0;
    glCaps::Discard();
    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
iosDisplayMgr::Present() {
    // the view is actually presented outside in the GLKView (the entire
    // Oryol frame runs from within the GLKView's draw request.
    displayMgrBase::Present();
}

//------------------------------------------------------------------------------
void
iosDisplayMgr::glBindDefaultFramebuffer() {
    GLKView* glkView = _priv::iosBridge::ptr()->glkView;
    [glkView bindDrawable];
}

} // namespace _priv
} // namespace Oryol
