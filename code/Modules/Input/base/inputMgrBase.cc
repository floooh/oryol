//------------------------------------------------------------------------------
//  inputMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "inputMgrBase.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
inputMgrBase::inputMgrBase() :
valid(false),
cursorMode(CursorMode::InvalidCursorMode) {
    // empty
}
    
//------------------------------------------------------------------------------
inputMgrBase::~inputMgrBase() {
    o_assert_dbg(!this->isValid());
}

//------------------------------------------------------------------------------
void
inputMgrBase::setup(const InputSetup& setup) {
    o_assert_dbg(!this->isValid());
    this->valid = true;
    this->inputSetup = setup;
    this->cursorMode = CursorMode::Normal;
    this->keyboard.Attached = true;
    this->mouse.Attached = true;
}

//------------------------------------------------------------------------------
void
inputMgrBase::discard() {
    o_assert_dbg(this->isValid());
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
inputMgrBase::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
const InputSetup&
inputMgrBase::getInputSetup() const {
    return this->inputSetup;
}

//------------------------------------------------------------------------------
void
inputMgrBase::beginCaptureText() {
    o_assert_dbg(this->valid);
    this->keyboard.beginCaptureText();
}

//------------------------------------------------------------------------------
void
inputMgrBase::endCaptureText() {
    o_assert_dbg(this->valid);
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