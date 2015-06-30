//------------------------------------------------------------------------------
//  mtlDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlDisplayMgr.h"
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

    this->cocoa.init();
    this->createWindow(setup);
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());

    this->destroyWindow();
    this->cocoa.terminate();

    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::Present() {
    Log::Info("mtlDisplayMgr::Present()\n");
}

//------------------------------------------------------------------------------
bool
mtlDisplayMgr::QuitRequested() const {
    return this->cocoa.windowShouldClose();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::ProcessSystemEvents() {
    this->cocoa.pollEvents();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::createWindow(const GfxSetup& setup) {

    StringBuilder strBuilder(setup.Title);
    strBuilder.Append(" (Metal)");
    this->cocoa.createWindow(setup.Width, setup.Height, strBuilder.AsCStr());
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::destroyWindow() {
    this->cocoa.destroyWindow();
}

} // namespace _priv
} // namespace Oryol
