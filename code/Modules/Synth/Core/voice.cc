//------------------------------------------------------------------------------
//  voice.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "voice.h"
#include "Core/Assert.h"
#include "Synth/Core/synth.h"

namespace Oryol {
namespace Synth {
    
//------------------------------------------------------------------------------
voice::voice() :
isValid(false),
oscillatorPos(0.0f),
oscillatorStep(0.0f) {
    for (auto& queue : this->tracks) {
        queue.Reserve(32);
    }
    for (int i = 0; i < synth::NumTracks; i++) {
        this->curOpIndex[i] = InvalidIndex;
        this->prvOpIndex[i] = InvalidIndex;
    }
}

//------------------------------------------------------------------------------
voice::~voice() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
voice::Setup(const SynthSetup& setupAttrs) {
    o_assert_dbg(!this->isValid);
    
    this->isValid = true;
    this->oscillatorPos = 0.0f;
    this->oscillatorStep = 0.0f;
}

//------------------------------------------------------------------------------
void
voice::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    for (auto& queue : this->tracks) {
        queue.Clear();
    }
}

//------------------------------------------------------------------------------
void
voice::AddOp(int32 track, const Op& op) {
    o_assert_dbg(this->isValid);
    o_assert_range_dbg(track, synth::NumTracks);
    this->tracks[track].Insert(op);
}

//------------------------------------------------------------------------------
void
voice::Synthesize(int32 startTick, void* buffer, int32 bufNumBytes) {
    o_assert_dbg(this->isValid);
    o_assert(synth::BufferSize == bufNumBytes);
    
    // first discard any ops that are no longer needed
    for (auto& track : this->tracks) {
        while ((track.Size() > 1) && (startTick > (track.Peek(1).startTick + track.Peek(1).FadeInTicks))) {
            track.Dequeue();
        }
    }
    
    // the sample tick range covered by the buffer
    const int32 endTick = startTick + synth::BufferNumSamples;
    int16* samplePtr = (int16*) buffer;
    
    // for each sample...
    for (int32 curTick = startTick; curTick < endTick; curTick++) {
        float32 sample = 1.0f;
        for (const auto& track : this->tracks) {
            const Op* opBegin = track.begin();
            const Op* opEnd   = track.end();
            for (const Op* op = opBegin; op < opEnd; op++) {
                if (((op + 1) == opEnd) || ((op < (opEnd - 1)) && (op[1].startTick > curTick))) {
                    o_assert_dbg(op->startTick <= curTick);
                
                    // sample current op
                    float32 s0 = this->sample(curTick, op);
                    
                    // cross-fade with previous op?
                    if ((op > opBegin) && (curTick < (op->startTick + op->FadeInTicks))) {
                        float32 t = float32(curTick - op->startTick) / float32(op->FadeInTicks);
                        float32 s1 = this->sample(curTick, op-1);
                        s0 = (s0 * t) + (s1 * (1.0f-t));
                    }
                    
                    // modulate sample from previous track
                    sample *= s0;
                    
                    // break out of loop
                    break;
                }
            }
        }

        // convert resulting sample to 16 bit and write to buffer
        int16 intSample = (sample * 0x7FFF);
        *samplePtr++ = intSample;
    }
}

//------------------------------------------------------------------------------
float32
voice::sample(int32 curTick, const Op* op) {
    
    // shortcut for const output
    if (Op::Const == op->Code) {
        return op->Amplitude + op->Bias;
    }
    else if (Op::Nop == op->Code) {
        return 0.0f;
    }
    
    // generate wave form
    int32 tick = (curTick - op->startTick) % op->freqLoopTicks;
    float32 t = float32(tick) / float32(op->freqLoopTicks); // t now 0..1 position in wave form
    if (Op::Sine == op->Code) {
        // sine wave
        return (std::sinf(t * M_PI * 2.0f) * op->Amplitude) + op->Bias;
    }
    else if (Op::Square == op->Code) {
        // square wave with Pulse as pulse with
        if (t <= op->Pulse) {
            return op->Amplitude + op->Bias;
        }
        else {
            return -op->Amplitude + op->Bias;
        }
    } else if (Op::Triangle == op->Code) {
        // triangle with shifted triangle top position
        // Pulse == 0.0: sawtooth
        // Pulse == 0.5: classic triangle
        // Pulse == 1.0: inverse sawtooth
        float32 s;
        if (t < op->Pulse) {
            s = t / op->Pulse;
        }
        else {
            s = 1.0f - ((t - op->Pulse) / (1.0f - op->Pulse));
        }
        return (((s * 2.0f) - 1.0f) * op->Amplitude) + op->Bias;
    }
    else {
        // noise
        // FIXME: replace with perlin or simplex noise
        // in order to get frequency output
        return float32((std::rand() & 0xFFFF) - 0x7FFF) / float32(0x8000);
    }
}

} // namespace Synth
} // namespace Oryol
