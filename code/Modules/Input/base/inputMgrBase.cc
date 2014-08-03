//------------------------------------------------------------------------------
//  inputMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "inputMgrBase.h"

namespace Oryol {
namespace Input {
    
//------------------------------------------------------------------------------
inputMgrBase::inputMgrBase() {
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

} // namespace Input
} // namespace Oryol