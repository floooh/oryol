//------------------------------------------------------------------------------
//  cpuSynthesizer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assert.h"
#include "Core/Memory/Memory.h"
#include "cpuSynthesizer.h"
#if ORYOL_WINDOWS
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <cstdlib>

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
cpuSynthesizer::Setup(const SynthSetup& /*setupParams*/) {
    this->setupWaves();
    Memory::Clear(this->freqCounters, sizeof(this->freqCounters));
}

//------------------------------------------------------------------------------
void
cpuSynthesizer::Synthesize(const opBundle& bundle) {
    for (int32 voiceIndex = 0; voiceIndex < synth::NumVoices; voiceIndex++) {
        this->synthesizeVoice(voiceIndex, bundle);
    }
}

//------------------------------------------------------------------------------
void
cpuSynthesizer::synthesizeVoice(int32 voiceIndex, const opBundle& bundle) {
    o_assert_dbg(synth::BufferSize == bundle.BufferNumBytes);
    o_assert_range_dbg(voiceIndex, synth::NumVoices);
    
    // the sample tick range covered by the buffer
    int16* samplePtr = (int16*) bundle.Buffer[voiceIndex];
    
    // for each sample...
    for (int32 curTick = bundle.StartTick[voiceIndex]; curTick < bundle.EndTick[voiceIndex]; curTick++) {
        int32 voiceSample = synth::MaxSampleVal;
        for (int trackIndex = 0; trackIndex < synth::NumTracks; trackIndex++) {
            const SynthOp* opBegin = bundle.Begin[voiceIndex][trackIndex];
            const SynthOp* opEnd   = bundle.End[voiceIndex][trackIndex];
            int32 trackSample = synth::MaxSampleVal;
            for (const SynthOp* op = opBegin; op < opEnd; op++) {
                if ((curTick >= op->startTick) && (curTick < op->endTick)) {
                    trackSample = this->sample(voiceIndex, trackIndex, op);
                }
            }
            // FIXME: implement track modes: Add, Mod, Clamp...
            voiceSample = (voiceSample * trackSample) >> 16;
        }
        *samplePtr++ = int16(voiceSample);
    }
}

//------------------------------------------------------------------------------
int32
cpuSynthesizer::sample(int32 voiceIndex, int32 trackIndex, const SynthOp* op) {
    
    // non-sample waves
    if (SynthOp::Const == op->Code) {
        return op->Amp + op->Bias;
    }
    else if (SynthOp::Nop == op->Code) {
        return 0;
    }

static uint32 freq = 22 << 5;
freq++;
if (freq > (220 << 5)) {
    freq = 22 << 5;
}
    
    // update the frequency counter
//    uint32 t = (((op->Frequency * NumWaveSamples) << 12) / synth::SampleRate);
uint32 t = ((((freq>>6) * NumWaveSamples) << 12) / synth::SampleRate);
    this->freqCounters[voiceIndex][trackIndex] += t;
    
    // sample a canned wave (fixme: frequency)
    uint32 tick = (this->freqCounters[voiceIndex][trackIndex] >> 12) % NumWaveSamples;
    return this->waves[op->Code][tick];
}

//------------------------------------------------------------------------------
void
cpuSynthesizer::setupWaves() {
    // sample size must be even
    o_assert_dbg(0 == (NumWaveSamples & 1));
    const int32 halfSamples = NumWaveSamples/2;

    // sine wave
    for (int32 i = 0; i < NumWaveSamples; i++) {
        float32 t = float32(i) / NumWaveSamples;
        float32 s = std::sin(t * M_PI * 2.0f) * synth::MaxSampleVal;
        this->waves[SynthOp::Sine][i] = int32(s);
    }
    
    // triangle
    for (int32 i = 0; i < halfSamples; i++) {
        float32 t = float32(i) / halfSamples;
        float32 s;
        if (t < 0.5f) {
            s = t / 0.5f;
        }
        else {
            s = 1.0f - ((t - 0.5f) / 0.5f);
        }
        s = s * synth::MaxSampleVal;
        this->waves[SynthOp::Triangle][i] = int32(s);
        this->waves[SynthOp::Triangle][i + halfSamples] = int32(-s);
    }
    
    // sawtooth
    for (int32 i = 0; i < halfSamples; i++) {
        float32 t = float32(i) / halfSamples;
        float32 s = t;
        s = ((s * 2.0f) - 1.0f) * synth::MaxSampleVal;
        this->waves[SynthOp::SawTooth][i] = int32(s);
        this->waves[SynthOp::SawTooth][i + halfSamples] = int32(s);
    }
    
    // square
    for (int32 i = 0; i < NumWaveSamples; i++) {
        if (i < (NumWaveSamples/2)) {
            this->waves[SynthOp::Square][i] = synth::MaxSampleVal;
        }
        else {
            this->waves[SynthOp::Square][i] = synth::MinSampleVal;
        }
    }
    
    // noise
    for (int32 i = 0; i < NumWaveSamples; i++) {
        this->waves[SynthOp::Noise][i] = (std::rand() & 0xFFFF) - 0x7FFF;
    }
    
    #define namco(x) ((x - 8)*4096)
    int32 n1[32] = {
         7, 12, 14, 14, 13, 11,  9, 10,
        11, 11, 10,  9,  6,  4,  3,  5,
         7,  9, 11, 10,  8,  5,  4,  3,
         3,  4,  5,  3,  1,  0,  0,  2
    };

    int32 n2[32] = {
         0,  8, 15,  7,
         1,  8, 14,  7,
         2,  8, 13,  7,
         3,  8, 12,  7,
         4,  8, 11,  7,
         5,  8, 10,  7,
         6,  8,  9,  7,
         7,  8,  8,  7,
    };

    int32 n3[32] = {
        7, 8,
        6, 9,
        5, 10,
        4, 11,
        3, 12,
        2, 13,
        1, 14,
        0, 15,
        0, 15,
        1, 14,
        2, 13,
        3, 12,
        4, 11,
        5, 10,
        6, 9,
        7, 8,
    };
    
    int32 n4[32] = {
         7, 15, 14, 13,
        15, 15, 14, 13,
        15, 15, 14, 13,
        15, 15, 14, 13,
        15, 15, 14, 13,
        15, 15, 15, 11,
         7, 15, 14, 13,
        15, 15, 14, 13
    };
    
    int32 n5[32] = {
         7, 10, 12, 13,
        14, 13, 12, 10,
         7,  4,  2,  1,
         0,  1,  2,  4,
         7, 11, 13, 14,
        13, 11,  7,  3,
         1,  0,  1,  3,
        7,  14,  7,  0
    };
    
    int32 n6[32] = {
         7, 13, 11, 8,
        11, 13,  9, 6,
        11, 14, 12, 7,
         9, 10,  6, 2,
         7, 12,  8, 4,
         5,  7,  2, 0,
         3,  7,  5, 1,
         3,  6,  3, 1,
    };
    
    for (int i = 0; i < 32; i++) {
        this->waves[SynthOp::Sine][i] = namco(n6[i]);
    }
}

} // namespace _priv
} // namespace Oryol
