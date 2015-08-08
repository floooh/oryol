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
    osxBridge::ptr()->showWindow();
    osxBridge::ptr()->callbacks.fbsize = mtlDisplayMgr::onFramebufferSize;
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
    return osxBridge::ptr()->shouldClose();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::configureWindow(const GfxSetup& setup) {

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
    [osxBridge::ptr()->mtkView setDrawableSize:drawableSize];
    [osxBridge::ptr()->mtkView setSampleCount:setup.SampleCount];
}

//------------------------------------------------------------------------------
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

} // namespace _priv
} // namespace Oryol
