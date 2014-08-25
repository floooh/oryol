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
    for (int i = 0; i < synth::NumVoices; i++) {
        this->voices[i].Setup(i, setupParams);
    }
    
    // add an initial NOP operation to first track of each voice,
    // this will generate all 0.0 samples instead of 1.0s
    Op nop;
    for (int i = 0; i < synth::NumVoices; i++) {
        this->AddOp(i, 0, nop, 0.0f);
    }
    
    // setup the GPU synthesizer
    this->gpuSynth.Setup(setupParams);
}

//------------------------------------------------------------------------------
void
soundMgrBase::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->setup = SynthSetup();
    for (voice& voice : this->voices) {
        voice.Discard();
    }
    this->gpuSynth.Discard();
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
soundMgrBase::AddOp(int32 voice, int32 track, const Op& op, float32 timeOffset) {
    o_assert_range_dbg(voice, synth::NumVoices);

    Op addOp = op;
    addOp.startTick     = this->curTick + synth::TimeToTicks(timeOffset, synth::SampleRate);
    addOp.freqLoopTicks = float32(synth::SampleRate) / addOp.Frequency;
    addOp.FadeInTicks   = synth::TimeToTicks(addOp.FadeIn, synth::SampleRate);
    this->voices[voice].AddOp(track, addOp);
}

} // namespace Synth
} // namespace Oryol
