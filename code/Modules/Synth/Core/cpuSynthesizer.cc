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
        int32 accum = 0;
        for (int trackIndex = 0; trackIndex < synth::NumTracks; trackIndex++) {
            const SynthOp* op = bundle.Op(voiceIndex, trackIndex, curTick);
            if (op) {
                o_assert_dbg(op);
                int32 s = this->sample(voiceIndex, trackIndex, accum, op);
                switch (op->Op) {
                    case SynthOp::Modulate:
                        accum = (accum * s) >> 15;
                        break;
                    case SynthOp::Add:
                        accum = accum + s;
                        if (accum < synth::MinSampleVal) accum = synth::MinSampleVal;
                        else if (accum > synth::MaxSampleVal) accum = synth::MaxSampleVal;
                        break;
                    case SynthOp::Replace:
                    case SynthOp::ModFreq:
                        accum = s;
                        break;
                    default:
                        break;
                }
            }
        }
        *samplePtr++ = int16(accum);
    }
}

//------------------------------------------------------------------------------
int32
cpuSynthesizer::sample(int32 voiceIndex, int32 trackIndex, int32 accum, const SynthOp* op) {
    
    // non-sample waves
    if (SynthOp::Const == op->Wave) {
        return op->Amp + op->Bias;
    }
    else {
        // update the frequency counter
        uint32 f = op->Freq;
        if (op->Op == SynthOp::ModFreq) {
            accum += (1<<15);
            f = (f * accum) >> 16;
        }
        uint32 t = (((f * NumWaveSamples) << 12) / synth::SampleRate);
        this->freqCounters[voiceIndex][trackIndex] += t;
    
        // sample a canned wave
        uint32 tick = (this->freqCounters[voiceIndex][trackIndex] >> 12) % NumWaveSamples;
        int32 s = ((this->waves[op->Wave][tick] * op->Amp) >> 15) + op->Bias;
        return s;
    }
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
    
    // Pacman arcade machine waveforms, see here:
    // http://www.lomont.org/Software/Games/PacMan/PacmanEmulation.pdf
    #define namco(x) ((x - 8)*4096)
    int32 pacmanWaves[8][NumWaveSamples] = {
        {
            // Pacman wave0
            7, 9, 10, 11, 12, 13, 13, 14, 14, 14, 13, 13, 12, 11, 10, 9,
            7, 5,  4,  3,  2,  1,  1,  0,  0,  0,  1,  1,  2,  3,  4, 5
        },
        {
            // Pacman wave1
            7, 12, 14, 14, 13, 11,  9, 10, 11, 11, 10,  9,  6,  4,  3,  5,
            7,  9, 11, 10,  8,  5,  4,  3,  3,  4,  5,  3,  1,  0,  0,  2
        },
        {
            // Pacman wave2
            7, 10, 12, 13, 14, 13, 12, 10,  7,  4,  2,  1,  0,  1,  2,  4,
            7, 11, 13, 14, 13, 11,  7,  3,  1,  0,  1,  3,  7,  14,  7, 0
        },
        {
            // Pacmane wave3
            7, 13, 11, 8, 11, 13, 9, 6, 11, 14, 12, 7, 9, 10, 6, 2,
            7, 12, 8, 4, 5, 7, 2, 0, 3, 7, 5, 1, 3, 6, 3, 1
        },
        {
            // Pacman wave4
            0,  8, 15,  7, 1,  8, 14,  7, 2,  8, 13,  7, 3,  8, 12,  7,
            4,  8, 11,  7, 5,  8, 10,  7, 6,  8,  9,  7, 7,  8,  8,  7
        },
        {
            // Pacman wave5
            7, 8, 6, 9, 5, 10, 4, 11, 3, 12, 2, 13, 1, 14, 0, 15,
            0, 15, 1, 14, 2, 13, 3, 12, 4, 11, 5, 10, 6, 9, 7, 8
        },
        {
            // Pacman wave6
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
        },
        {
            // Pacman wav7
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
        }
    };

    for (int w = 0; w < 8; w++) {
        for (int i = 0; i < NumWaveSamples; i++) {
            this->waves[SynthOp::Custom0 + w][i] = namco(pacmanWaves[w][i]);
        }
    }
}

} // namespace _priv
} // namespace Oryol
