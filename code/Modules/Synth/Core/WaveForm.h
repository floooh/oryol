#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::WaveForm
    @brief sound synthesis wave forms
*/
#include "Core/Types.h"

namespace Oryol {
namespace Synth {
    
class WaveForm {
public:
    /// waveform types
    enum Code {
        Sine = 0,
        Triangle,
        SawTooth,
        Noise,
        
        NumWaveForms,
        InvalidWaveForm,
    };
};
    
} // namespace Synth
} // namespace Oryol