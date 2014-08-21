//------------------------------------------------------------------------------
//  soundMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "soundMgrBase.h"
#include "Core/Assert.h"
#include "Time/Clock.h"

namespace Oryol {
namespace Synth {
    
//------------------------------------------------------------------------------
soundMgrBase::soundMgrBase() :
isValid(false),
curTick(0) {
    // empty
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
    this->curTick = 0;
    this->waveGenerator.Setup(setupParams);
}

//------------------------------------------------------------------------------
void
soundMgrBase::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->setup = SynthSetup();
    this->waveGenerator.Discard();
}

//------------------------------------------------------------------------------
bool
soundMgrBase::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
soundMgrBase::Update() {
    o_assert_dbg(this->isValid);
}

//------------------------------------------------------------------------------
void
soundMgrBase::Play(uint32 voice, const Sound& snd, float32 timeOffset) {
    item soundItem;
    soundItem.sound = snd;
    soundItem.timeOffset = timeOffset;
    soundItem.absStartTick = this->curTick + synth::TimeToTicks(timeOffset, synth::SampleRate);
    this->waveGenerator.PushItem(soundItem);
}

} // namespace Synth
} // namespace Oryol
