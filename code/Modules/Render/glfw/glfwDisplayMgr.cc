//------------------------------------------------------------------------------
//  glfwDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glfwDisplayMgr.h"
#include "GLFW/glfw3.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
glfwDisplayMgr::glfwDisplayMgr() :
glfwWindow(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
glfwDisplayMgr::~glfwDisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::SetupDisplay(const DisplaySetup& setup) {
    // FIXME
    displayMgrBase::SetupDisplay(setup);
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::DiscardDisplay() {
    // FIXME!
    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::ProcessSystemEvents() {
    // FIXME!
    displayMgrBase::ProcessSystemEvents();
}


//------------------------------------------------------------------------------
void
glfwDisplayMgr::Present() {
    // FIXME!
    displayMgrBase::Present();
}

} // namespace Render
} // namespace Oryol