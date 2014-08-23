#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::Op
    @brief a single wave synthesis 'instruction'
    
    Op objects are instructions for the waveform synthesizer. To create the
    final output of a voice, several the output waveforms of several
    ops are combined (usually modulated). For instance, the Op on the
    first voice track might generate a high frequency sawtooth wave, and
    Ops on the second track might modulate this with an ASDR envelope,
    and an Op on the third track could apply a final output volume.
*/
#include "Core/Types.h"

namespace Oryol {
namespace Synth {
    
class Op {
public:
    /// the op-code
    enum Code {
        Nop = 0,        // no operation, pass-thru, this is the default
        Const,          // output a constant Amplitude, Frequency not used
        Sine,           // generate a sine wave
        Triangle,       // generate a triangle/sawtooth/invsawtooth wave
        Square,         // generate a square wave
        Noise,          // generate noise
        
        NumCodes
    } Code = Nop;
    /// amplitude (usually -1.0 to 1.0)
    float32 Amplitude = 1.0f;
    /// amplitude bias (can be used to move wave into positive area)
    float32 Bias = 0.0f;
    /// peak position/pulse width for Triangle/Square wave
    float32 Pulse = 0.5f;
    /// frequency
    union {
        float32 Frequency = 440.0f;
        int32 freqLoopTicks;
    };
    /// fade-in duration in seconds
    union {
        float32 FadeIn = 0.0f;
        int32 FadeInTicks;
    };
    /// start tick (private, computed)
    int32 startTick = 0;
    /// less-then operator for sorting by StartTick
    bool operator<(const Op& rhs) const {
        return this->startTick < rhs.startTick;
    };
    /// convert op-code to string
    static const char* ToString(enum Code c) {
        switch (c) {
            case Nop:      return "Nop";
            case Const:    return "Const";
            case Sine:     return "Sine";
            case Triangle: return "Triangle";
            case Square:   return "Square";
            case Noise:    return "Noise";
            default:       return "Invalid";
        }
    };
};
    
} // namespace Synth
} // namespace Oryol