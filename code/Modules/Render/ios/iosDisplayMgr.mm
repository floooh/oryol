//------------------------------------------------------------------------------
//  iosDisplayMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "iosDisplayMgr.h"
#include "Render/gl/gl_impl.h"
#include "Render/gl/glInfo.h"
#include "Render/gl/glExt.h"
#include <GLKit/GLKit.h>

namespace Oryol {
namespace Render {

using namespace Core;

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
iosDisplayMgr::SetupDisplay(const RenderSetup& renderSetup) {
    o_assert(!this->IsDisplayValid());

    Log::Info("iosDisplayMgr::SetupDisplay() called!\n");
    displayMgrBase::SetupDisplay(renderSetup);
    
    // modify the color/depth/stencil format of the GLKView
    GLKView* glkView = iosBridge::Instance()->iosGetGLKView();
    PixelFormat::Code colorFmt = renderSetup.GetDisplayAttrs().GetColorPixelFormat();
    PixelFormat::Code depthFmt = renderSetup.GetDisplayAttrs().GetDepthPixelFormat();
    switch (colorFmt) {
        case PixelFormat::R5G6B5:
            glkView.drawableColorFormat = GLKViewDrawableColorFormatRGB565;
            break;
            
        case PixelFormat::R8G8B8:
        case PixelFormat::R8G8B8A8:
            glkView.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
            break;
            
        default:
            Log::Warn("iosDisplayMgr: requested color format not supported, falling back to R5G6B5!\n");
            glkView.drawableColorFormat = GLKViewDrawableColorFormatRGB565;
            break;
    }
    switch (depthFmt) {
        case PixelFormat::None:
            glkView.drawableDepthFormat = GLKViewDrawableDepthFormatNone;
            glkView.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
            
        case PixelFormat::D16:
            glkView.drawableDepthFormat = GLKViewDrawableDepthFormat16;
            glkView.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
            
        case PixelFormat::D24S8:
            glkView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
            glkView.drawableStencilFormat = GLKViewDrawableStencilFormat8;
            break;
            
        default:
            Log::Warn("iosDisplayMgr: requested depth format not supported, falling back to D16!\n");
            glkView.drawableDepthFormat = GLKViewDrawableDepthFormat16;
            glkView.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
    }
    
    // dump GL information
    glInfo::PrintInfo();

    // setup extensions
    glExt::Setup();
    
    // update the displayAttrs with the actual frame buffer size
    this->glFramebufferWidth = glkView.drawableWidth;
    this->glFramebufferHeight = glkView.drawableHeight;
    Log::Info("iosDisplayMgr: actual framebuffer size w=%d, h=%d\n", this->glFramebufferWidth, this->glFramebufferHeight);
    this->displayAttrs.SetFramebufferWidth(this->glFramebufferWidth);
    this->displayAttrs.SetFramebufferHeight(this->glFramebufferHeight);
    this->displayAttrs.SetWindowWidth(this->glFramebufferWidth);
    this->displayAttrs.SetWindowHeight(this->glFramebufferHeight);
}

//------------------------------------------------------------------------------
void
iosDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    this->glDefaultFramebuffer = 0;
    this->glFramebufferWidth = 0;
    this->glFramebufferHeight = 0;
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
    GLKView* glkView = iosBridge::Instance()->iosGetGLKView();
    [glkView bindDrawable];
}

} // namespace Render
} // namespace Oryol
