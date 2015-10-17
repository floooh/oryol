//------------------------------------------------------------------------------
//  mtlDisplayMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlDisplayMgr.h"
#include "mtlTypes.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace _priv {

mtlDisplayMgr* mtlDisplayMgr::self = nullptr;

//------------------------------------------------------------------------------
mtlDisplayMgr::mtlDisplayMgr() {
    o_assert_dbg(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
mtlDisplayMgr::~mtlDisplayMgr() {
    o_assert_dbg(self);
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
    self = nullptr;
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());

    displayMgrBase::SetupDisplay(setup, ptrs);

    this->configureWindow(setup);
    #if ORYOL_MACOS
    osBridge::ptr()->showWindow();
    osBridge::ptr()->callbacks.fbsize = mtlDisplayMgr::onFramebufferSize;
    #endif
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
bool
mtlDisplayMgr::QuitRequested() const {
    #if ORYOL_MACOS
    return osBridge::ptr()->shouldClose();
    #else
    return false;
    #endif
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::configureWindow(const GfxSetup& setup) {

    #if ORYOL_MACOS
    StringBuilder strBuilder(setup.Title);
    strBuilder.Append(" (Metal)");

    NSWindow* window = osxBridge::ptr()->appWindow;
    [window setTitle:[NSString stringWithUTF8String:strBuilder.AsCStr()]];
    [window setContentSize:NSMakeSize(setup.Width, setup.Height)];
    [window center];

    // NOTE: this will upscale on Retina displays, to render at Retina
    // resolution just drop the setDrawableSiz, or set to
    // [mtkView convertSizeToBacking:mtkView.bounds.size], but this
    // also needs fixes in the ApplyScissorRect and ApplyViewport functions!
    CGSize drawableSize = { (CGFloat) setup.Width, (CGFloat) setup.Height };
    [osBridge::ptr()->mtkView setDrawableSize:drawableSize];
    #elif ORYOL_IOS
    osBridge* bridge = osBridge::ptr();
    if (gfxSetup.HighDPI) {
        [bridge->mtkView setContentScaleFactor:2.0f];
    }
    else {
        [bridge->mtkView setContentScaleFactor:1.0f];
    }
    // get actual rendering size
    int fbWidth = (int) [bridge->mtkView drawableSize].width;
    int fbHeight = (int) [bridge->mtkView drawableSize].height;
    Log::Info("mtlDisplayMgr: actual framebuffer size w=%d, h=%d\n", fbWidth, fbHeight);
    this->displayAttrs.FramebufferWidth = fbWidth;
    this->displayAttrs.FramebufferHeight = fbHeight;
    this->displayAttrs.WindowWidth = fbWidth;
    this->displayAttrs.WindowHeight = fbHeight;
    #else
    #error "mtlDisplayMgr: undefined platform!"
    #endif
    [osBridge::ptr()->mtkView setSampleCount:setup.SampleCount];
}

//------------------------------------------------------------------------------
#if ORYOL_MACOS
void
mtlDisplayMgr::onFramebufferSize(int w, int h) {
    o_assert_dbg((w > 0) && (h > 0));
    if (self) {
        self->displayAttrs.FramebufferWidth = w;
        self->displayAttrs.FramebufferHeight = h;
        self->displayAttrs.WindowWidth = w;
        self->displayAttrs.WindowHeight = h;
    }
}
#endif

} // namespace _priv
} // namespace Oryol
