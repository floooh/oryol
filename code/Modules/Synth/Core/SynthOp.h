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
    /// operation to perform on accumulated value
    enum OpT {
        Nop,            // simply pass-thru accumulated value (default)
        Modulate,       // multiply with accumulated value
        Add,            // add-saturate with accumulated value
        Replace,        // replace accum with current value
        ModFreq,        // modulate frequency with accumulated value
    } Op = Nop;

    /// oscillator waveform
    enum WaveT {
        Const,          // output a constant Amplitude, Frequency not used (default)
        Sine,           // sine wave
        SawTooth,       // sawtooth wave
        Triangle,       // triangle wave
        Square,         // square wave
        Noise,          // generate noise
        Custom0,
        Custom1,
        Custom2,
        Custom3,
        Custom4,
        Custom5,
        Custom6,
        Custom7,
        
        NumWaves
    } Wave = Const;
    /// amplitude (MinSampleVal .. MaxSampleVal)
    int32 Amp = _priv::synth::MaxSampleVal;
    /// amplitude bias (can be used to move wave into positive area)
    int32 Bias = 0;
    /// frequency
    int32 Freq = 440;
    /// start tick (private, computed)
    int32 startTick = 0;
    /// end tick (private, computed)
    int32 endTick = (1<<30);
    /// less-then operator for sorting by StartTick
    bool operator<(const SynthOp& rhs) const {
        return this->startTick < rhs.startTick;
    };
    /// convert op-code to string
    static const char* ToString(enum OpT op) {
        switch (op) {
            case Nop:       return "Nop";
            case Modulate:  return "Modulate";
            case Add:       return "Add";
            case ModFreq:   return "ModFreq";
            default:        return "Invalid";
        }
    }
    /// convert wave-form to string
    static const char* ToString(enum WaveT w) {
        switch (w) {
            case Const:     return "Const";
            case Sine:      return "Sine";
            case Triangle:  return "Triangle";
            case Square:    return "Square";
            case Noise:     return "Noise";
            case Custom0:   return "Custom0";
            case Custom1:   return "Custom1";
            case Custom2:   return "Custom2";
            case Custom3:   return "Custom3";
            case Custom4:   return "Custom4";
            case Custom5:   return "Custom5";
            case Custom6:   return "Custom6";
            case Custom7:   return "Custom7";
            default:        return "Invalid";
        }
    };
};
    
} // namespace Oryol
