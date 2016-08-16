//------------------------------------------------------------------------------
//  uwpInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "uwpInputMgr.h"
#include "Core/Core.h"

namespace Oryol {
namespace _priv {

uwpInputMgr* uwpInputMgr::self = nullptr;

//------------------------------------------------------------------------------
uwpInputMgr::uwpInputMgr() :
runLoopId(RunLoop::InvalidId) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
uwpInputMgr::~uwpInputMgr() {
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
void
uwpInputMgr::setup(const InputSetup& setup) {
    inputMgrBase::setup(setup);
    // FIXME: find out if running on desktop or mobile,
    // and set the attached-flags
    // FIXME: register event handlers

    // attach our reset callback to the global runloop
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
void
uwpInputMgr::discard() {
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;
    inputMgrBase::discard();
}

} // namespace _priv
} // namespace Oryol
