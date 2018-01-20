//------------------------------------------------------------------------------
//  mtlDisplayMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlDisplayMgr.h"
#include "Core/String/StringBuilder.h"
#if ORYOL_MACOS
#include "Core/private/osx/osxBridge.h"
#else
#include "Core/private/ios/iosBridge.h"
#endif

namespace Oryol {
namespace _priv {

#if ORYOL_MACOS
typedef osxBridge osBridge;
#else
typedef iosBridge osBridge;
#endif

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
mtlDisplayMgr::SetupDisplay(const GfxDesc& desc, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());

    displayMgrBase::SetupDisplay(desc, ptrs);

    this->configureWindow(desc);
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
mtlDisplayMgr::configureWindow(const GfxDesc& desc) {

    #if ORYOL_MACOS
    StringBuilder strBuilder(desc.Title);
    strBuilder.Append(" (Metal)");

    NSWindow* window = osxBridge::ptr()->appWindow;
    [window setTitle:[NSString stringWithUTF8String:strBuilder.AsCStr()]];
    [window setContentSize:NSMakeSize(desc.Width, desc.Height)];
    [window center];
    osBridge* bridge = osBridge::ptr();
    if (!desc.HighDPI) {
        CGSize drawableSize = { (CGFloat) desc.Width, (CGFloat) desc.Height };
        [bridge->mtkView setDrawableSize:drawableSize];
    }
    #elif ORYOL_IOS
    osBridge* bridge = osBridge::ptr();
    if (desc.HighDPI) {
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
    if (fbWidth == desc.Width * 2) {
        // we're on a Retina display
        bridge->mouseScale = 2.0;
    }
    #endif
    Log::Info("mtlDisplayMgr: actual framebuffer size w=%d, h=%d\n", fbWidth, fbHeight);
    this->displayAttrs.FramebufferWidth = fbWidth;
    this->displayAttrs.FramebufferHeight = fbHeight;
    this->displayAttrs.WindowWidth = winContentRect.size.width;
    this->displayAttrs.WindowHeight = winContentRect.size.height;
    [osBridge::ptr()->mtkView setSampleCount:desc.SampleCount];
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

//------------------------------------------------------------------------------
const void*
mtlDisplayMgr::mtlDevice() {
    return (__bridge const void*) osBridge::ptr()->mtlDevice;
}

//------------------------------------------------------------------------------
const void*
mtlDisplayMgr::mtlRenderPassDescriptor() {
    return (__bridge const void*) [osBridge::ptr()->mtkView currentRenderPassDescriptor];
}

//------------------------------------------------------------------------------
const void*
mtlDisplayMgr::mtlDrawable() {
    return (__bridge const void*) [osBridge::ptr()->mtkView currentDrawable];
}

} // namespace _priv
} // namespace Oryol
