#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::Sound
    @brief a set of sound parameters
    
    These sound parameters roughly mimic the SID register set for 
    one voice but converted to more convenient float32 units in Hz and 
    seconds. To manipulate sound output, enqueue Sound objects at
    specific points in time.
    
    See here for details:
    http://www.waitingforfriday.com/index.php/Commodore_SID_6581_Datasheet
*/
#include "Core/Types.h"
#include "Synth/Core/WaveForm.h"

namespace Oryol {
namespace Synth {
    
class Sound {
public:
    enum Trigger {
        On,
        Off,
        Hold,
    };

    /// volume (0.0 to 1.0)
    float32 Volume = 1.0f;
    /// oscillator frequency in Hz (Freq = Fout = Fn * 0.0596)
    float32 Freq = 1000.0f;
    /// relative pulse width for pulse waveform (0.0 .. 1.0)
    float32 PulseWidth = 0.5f;
    /// attack duration in seconds
    float32 Attack = 0.0f;
    /// decay duration in seconds
    float32 Decay = 0.0f;
    /// release duration in seconds
    float32 Release = 0.0f;
    /// sustain level (0.0 to 1.0)
    float32 SustainLevel = 0.5f;
    /// the Gate bit which controls the Envelop Generator
    Trigger Gate = Hold;
    /// the Sync bit (not implemented yet)
    bool Sync = false;
    /// the RingMod bit (not implemented yet)
    bool RingMod = false;
    /// generate Triangle wave if set
    bool Triangle = false;
    /// generate Sawtooth wave if set
    bool Sawtooth = false;
    /// generate Square wave if set (controlled by PulseWidth)
    bool Square = false;
    /// generate Noise wave if set
    bool Noise = false;
    
    /// FIXME: FILTER BITS
};
    
} // namespace Synth
} // namespace Oryol
