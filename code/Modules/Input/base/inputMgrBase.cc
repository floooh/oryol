//------------------------------------------------------------------------------
//  inputMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "inputMgrBase.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
inputMgrBase::inputMgrBase() :
cursorMode(CursorMode::Normal) {
    this->keyboard.setAttached(true);
    this->mouse.setAttached(true);
}
    
//------------------------------------------------------------------------------
inputMgrBase::~inputMgrBase() {
    // empty
}

//------------------------------------------------------------------------------
void
inputMgrBase::beginCaptureText() {
    this->keyboard.beginCaptureText();
}

//------------------------------------------------------------------------------
void
inputMgrBase::endCaptureText() {
    this->keyboard.endCaptureText();
}

//------------------------------------------------------------------------------
void
inputMgrBase::setCursorMode(CursorMode::Code mode) {
    o_assert_range_dbg(mode, CursorMode::NumCursorModes);
    this->cursorMode = mode;
}

} // namespace _priv
} // namespace Oryol