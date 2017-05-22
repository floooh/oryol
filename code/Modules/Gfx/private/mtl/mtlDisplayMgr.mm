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
    osBridge* bridge = osBridge::ptr();
    if (!setup.HighDPI) {
        CGSize drawableSize = { (CGFloat) setup.Width, (CGFloat) setup.Height };
        [bridge->mtkView setDrawableSize:drawableSize];
    }
    #elif ORYOL_IOS
    osBridge* bridge = osBridge::ptr();
    if (gfxSetup.HighDPI) {
        [bridge->mtkView setContentScaleFactor:2.0f];
        bridge->mouseScale = 2.0f;
    }
    else {
        [bridge->mtkView setContentScaleFactor:1.0f];
    }
    #else
    #error "mtlDisplayMgr: undefined platform!"
    #endif
    [[bridge->mtkView layer] setMagnificationFilter:kCAFilterNearest];

    // get actual rendering size
    #if ORYOL_IOS
    const CGRect winContentRect = [bridge->mtkView frame];
    #else
    const NSRect winContentRect = [bridge->mtkView frame];
    #endif
    CGSize fbSize = [bridge->mtkView drawableSize];
    int fbWidth = (int) fbSize.width;
    int fbHeight = (int) fbSize.height;
    #if ORYOL_OSX
    if (fbWidth == setup.Width * 2) {
        // we're on a Retina display
        bridge->mouseScale = 2.0;
    }
    #endif
    Log::Info("mtlDisplayMgr: actual framebuffer size w=%d, h=%d\n", fbWidth, fbHeight);
    this->displayAttrs.FramebufferWidth = fbWidth;
    this->displayAttrs.FramebufferHeight = fbHeight;
    this->displayAttrs.WindowWidth = winContentRect.size.width;
    this->displayAttrs.WindowHeight = winContentRect.size.height;
    [osBridge::ptr()->mtkView setSampleCount:setup.SampleCount];
}

//------------------------------------------------------------------------------
#if ORYOL_MACOS
void
mtlDisplayMgr::onFramebufferSize(int w, int h) {
    o_assert_dbg((w > 0) && (h > 0));
    if (self) {
        // need to get the actual size
        CGSize fbSize = [osxBridge::ptr()->mtkView drawableSize];
        self->displayAttrs.FramebufferWidth = (int) fbSize.width;
        self->displayAttrs.FramebufferHeight = (int) fbSize.height;
        self->displayAttrs.WindowWidth = w;
        self->displayAttrs.WindowHeight = h;
    }
}
#endif

} // namespace _priv
} // namespace Oryol
