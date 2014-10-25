#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SynthOp
    @ingroup Synth
    @brief a single wave synthesis 'instruction'
    
    Op objects are instructions for the waveform synthesizer. To create the
    final output of a voice, several the output waveforms of several
    ops are combined (usually modulated). For instance, the Op on the
    first voice track might generate a high frequency sawtooth wave, and
    Ops on the second track might modulate this with an ASDR envelope,
    and an Op on the third track could apply a final output volume.
*/
#include "Core/Types.h"
#include "Synth/Core/synth.h"

namespace Oryol {
    
class SynthOp {
public:
    /// the op-code
    enum Code {
        Nop = 0,        // no operation, pass-thru, this is the default
        Const,          // output a constant Amplitude, Frequency not used
        Sine,           // sine wave
        SawTooth,       // sawtooth wave
        Triangle,       // triangle wave
        Square,         // square wave
        Noise,          // generate noise
        
        NumCodes
    } Code = Nop;
    /// amplitude (MinSampleVal .. MaxSampleVal)
    int32 Amp = _priv::synth::MaxSampleVal;
    /// amplitude bias (can be used to move wave into positive area)
    int32 Bias = 0;
    /// frequency
    int32 Frequency = 440;
    /// start tick (private, computed)
    int32 startTick = 0;
    /// end tick (private, computed)
    int32 endTick = (1<<30);
    /// less-then operator for sorting by StartTick
    bool operator<(const SynthOp& rhs) const {
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
    
} // namespace Oryol
