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
    this->curTime = Clock::Now();
    if (this->firstFrame) {
        this->firstFrame = false;
        this->startTime = this->curTime;
    }
}

//------------------------------------------------------------------------------
void
soundMgrBase::Play(uint32 voice, const Sound& snd, float32 pitch, float32 volume, float32 timeOffset) {

    this->
    item sndItem;
    sndItem.sound = snd;
    sndItem.pitch = pitch;
    sndItem.volume = volume;
    sndItem.timeOffset = timeOffset;
    sndItem.absStartTick = synth::TimeToTicks(this->curTime.
    
}

//------------------------------------------------------------------------------
void
soundMgrBase::pushItem(item item) {
    o_assert_dbg(this->isValid);
    // FIXME FIXME FIXME
}

} // namespace Synth
} // namespace Oryol
