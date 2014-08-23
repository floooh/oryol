//------------------------------------------------------------------------------
//  generator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "generator.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Synth {
    
//------------------------------------------------------------------------------
generator::generator() :
isValid(false),
gateTick(0),
oscillatorPos(0.0f),
oscillatorStep(0.0f) {
    this->itemQueue.Reserve(32);
}

//------------------------------------------------------------------------------
generator::~generator() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
generator::Setup(const SynthSetup& setupAttrs) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(this->itemQueue.Empty());
    
    this->isValid = true;
    this->gateTick = 0;
    this->oscillatorPos = 0.0f;
    this->oscillatorStep = 0.0f;
}

//------------------------------------------------------------------------------
void
generator::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->itemQueue.Clear();
}

//------------------------------------------------------------------------------
void
generator::PushItem(const item& item) {
    o_assert_dbg(this->isValid);
    this->itemQueue.Insert(item);
}

//------------------------------------------------------------------------------
void
generator::Generate(int32 startTick, void* buffer, int32 bufNumBytes) {
    o_assert_dbg(this->isValid);
    o_assert(synth::BufferSize == bufNumBytes);
    
    // the sample tick range covered by the buffer
    const int32 endTick = startTick + synth::BufferNumSamples;
    int16* samplePtr = (int16*) buffer;
    
    // for each tick / sample
    const item* curItem = this->itemQueue.begin();
    const item* onePastLastItem = this->itemQueue.end();
    for (int32 curTick = startTick; curTick < endTick; curTick++) {
        
        // check if new item must be pulled
        if ((curItem != onePastLastItem) && (curItem->absStartTick <= curTick)) {
            if (curItem->sound.Gate != Sound::Hold) {
                this->gateTick = curItem->absStartTick;
            }
            this->curSound = curItem->sound;
            this->oscillatorStep = this->curSound.Freq / synth::SampleRate;
            curItem++;
        }
        
        // get wave form sample
        float32 sample = this->oscillator();
        
        // FIXME: amplitude modulation
        
        // FIXME: filtering
        
        // convert to int16 and write to buffer
        int16 intSample = (sample * 0x7FFF);
        *samplePtr++ = intSample;
    }
    
    // discard items behind the play cursor
    while (!this->itemQueue.Empty() && this->itemQueue.Peek(0).absStartTick < endTick) {
        this->itemQueue.Dequeue();
    }
}

//------------------------------------------------------------------------------
float32
generator::oscillator() {
    
    float32 sample = 0.0f;
    float32 t = this->oscillatorPos;
    if (this->curSound.Triangle) {
        sample = synth::Triangle(t);
    }
    else if (this->curSound.Sawtooth) {
        sample = synth::Sawtooth(t);
    }
    else if (this->curSound.Square) {
        sample = synth::Square(t, this->curSound.PulseWidth);
    }
    else if (this->curSound.Sawtooth) {
        sample = synth::Sawtooth(t);
    }
    else if (this->curSound.Noise) {
        sample = synth::Noise(t);
    }
    this->oscillatorPos += this->oscillatorStep;
    if (this->oscillatorPos >= 1.0f) {
        this->oscillatorPos -= 1.0f;
    }
    return sample;
}

} // namespace Synth
} // namespace Oryol
