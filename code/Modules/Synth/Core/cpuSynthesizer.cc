//------------------------------------------------------------------------------
//  cpuSynthesizer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assert.h"
#include "cpuSynthesizer.h"
#if ORYOL_WINDOWS
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <cstdlib>

namespace Oryol {
namespace Synth {
    
//------------------------------------------------------------------------------
void
cpuSynthesizer::Synthesize(const opBundle& bundle) const {
    for (int32 voiceIndex = 0; voiceIndex < synth::NumVoices; voiceIndex++) {
        this->synthesizeVoice(voiceIndex, bundle);
    }
}

//------------------------------------------------------------------------------
void
cpuSynthesizer::synthesizeVoice(int32 voiceIndex, const opBundle& bundle) const {
    o_assert_dbg(synth::BufferSize == bundle.BufferNumBytes);
    o_assert_range_dbg(voiceIndex, synth::NumVoices);
    
    // the sample tick range covered by the buffer
    int16* samplePtr = (int16*) bundle.Buffer[voiceIndex];
    
    // for each sample...
    for (int32 curTick = bundle.StartTick[voiceIndex]; curTick < bundle.EndTick[voiceIndex]; curTick++) {
        float32 voiceSample = 1.0f;
        for (int trackIndex = 0; trackIndex < synth::NumTracks; trackIndex++) {
            const Op* opBegin = bundle.Begin[voiceIndex][trackIndex];
            const Op* opEnd   = bundle.End[voiceIndex][trackIndex];
            float32 trackSample = 1.0f;
            for (const Op* op = opBegin; op < opEnd; op++) {
                if ((curTick >= op->startTick) && (curTick < op->endTick)) {
                    
                    // compute current sample
                    float32 s = this->sample(curTick, op);
                    
                    // cross-fade with previous sample?
                    if (curTick < (op->startTick + op->FadeInTicks)) {
                        float32 t = float32(curTick - op->startTick) / float32(op->FadeInTicks);
                        trackSample = (s * t) + (trackSample * (1.0f - t));
                    }
                    else {
                        trackSample = s;
                    }
                }
            }
            voiceSample *= trackSample;
        }
        *samplePtr++ = int16(voiceSample * 32767.0f);
    }
}

//------------------------------------------------------------------------------
float32
cpuSynthesizer::sample(int32 curTick, const Op* op) const {
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
        #if ORYOL_ANDROID || ORYOL_LINUX
        return (std::sin(t * M_PI * 2.0f) * op->Amplitude) + op->Bias;
        #else
        return (std::sinf(t * M_PI * 2.0f) * op->Amplitude) + op->Bias;
        #endif
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