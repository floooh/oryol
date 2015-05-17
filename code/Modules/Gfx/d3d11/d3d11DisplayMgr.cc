//------------------------------------------------------------------------------
//  d3d11DisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11DisplayMgr.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11DisplayMgr::d3d11DisplayMgr() {
    // FIXME
}

//------------------------------------------------------------------------------
d3d11DisplayMgr::~d3d11DisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
}   

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::SetupDisplay(const GfxSetup& setup) {
    o_assert(!this->IsDisplayValid());
    
    displayMgrBase::SetupDisplay(setup);

}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());

    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
bool
d3d11DisplayMgr::QuitRequested() const {
    // FIXME
    return true;
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::ProcessSystemEvents() {
    // FIXME
    displayMgrBase::ProcessSystemEvents();
}

} // namespace _priv
} // namespace Oryol
