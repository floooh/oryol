//------------------------------------------------------------------------------
//  soundMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "soundMgrBase.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Synth {
    
//------------------------------------------------------------------------------
soundMgrBase::soundMgrBase() :
isValid(false) {
    
}

//------------------------------------------------------------------------------
soundMgrBase::~soundMgrBase() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
soundMgrBase::Setup(const SynthSetup& setupParams) {
    o_assert(!this->isValid);
    this->isValid = true;
    this->setup = setupParams;
}

//------------------------------------------------------------------------------
void
soundMgrBase::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->setup = SynthSetup();
}

//------------------------------------------------------------------------------
bool
soundMgrBase::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
soundMgrBase::Update(Time::Duration timeDiff) {
    this->curTime += timeDiff.AsSeconds();
    // FIXME: process sound queue
}

//------------------------------------------------------------------------------
void
soundMgrBase::PushItem(item item) {
    item.absStartTime = this->curTime + item.timeOffset;
    item.absEndTime = item.absStartTime + item.sound.Duration();
    this->soundQueue.Insert(item);
}

} // namespace Synth
} // namespace Oryol
