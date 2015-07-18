//------------------------------------------------------------------------------
//  mtlDisplayMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlDisplayMgr.h"
#include "mtlTypes.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlDisplayMgr::mtlDisplayMgr() {
    // empty
}

//------------------------------------------------------------------------------
mtlDisplayMgr::~mtlDisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());

    displayMgrBase::SetupDisplay(setup, ptrs);

    this->configureWindow(setup);
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::Present() {
    // hmm... nothing to do here?
}

//------------------------------------------------------------------------------
bool
mtlDisplayMgr::QuitRequested() const {
    return osxAppBridge::ptr()->shouldClose();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::ProcessSystemEvents() {
    // empty, system events will actually be polled in osxAppProxy::onFrame()
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::configureWindow(const GfxSetup& setup) {

    StringBuilder strBuilder(setup.Title);
    strBuilder.Append(" (Metal)");

    NSWindow* window = osxAppBridge::ptr()->appWindow;
    [window setTitle:[NSString stringWithUTF8String:strBuilder.AsCStr()]];
    [window setContentSize:NSMakeSize(setup.Width, setup.Height)];
    [window center];

    [osxAppBridge::ptr()->mtkView setSampleCount:setup.SampleCount];
}

} // namespace _priv
} // namespace Oryol
