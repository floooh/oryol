#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::Sound
    @brief a sound effect with Waveform and envelope curve
*/
#include "Core/Types.h"
#include "Synth/Core/WaveForm.h"

namespace Oryol {
namespace Synth {
    
class Sound {
public:
    /// the wave form
    WaveForm::Code Wave = WaveForm::Sine;

    /// attack duration
    float32 AttackDuration = 0.0f;
    /// attack amplitude
    float32 AttackAmplitude = 0.0f;
    /// decay duration
    float32 DecayDuration = 0.0f;
    /// decay amplitude
    float32 DecayAmplitude = 0.0f;
    /// sustain duration
    float32 SustainDuration = 0.0f;
    /// sustain amplitude
    float32 SustainAmplitude = 0.0f;
    /// release duration
    float32 ReleaseDuration = 0.0f;
    /// release amplitude
    float32 ReleaseAmplitude = 0.0f;

    /// set attack duration and amplitude
    void Attack(float32 duration, float32 amplitude) {
        this->AttackDuration = duration;
        this->AttackAmplitude = amplitude;
    };
    /// set decay duration and amplitude
    void Decay(float32 duration, float32 amplitude) {
        this->DecayDuration = duration;
        this->DecayAmplitude = amplitude;
    };
    /// set sustain duration and amplitude
    void Sustain(float32 duration, float32 amplitude) {
        this->SustainDuration = duration;
        this->SustainAmplitude = amplitude;
    };
    /// set release duration and amplitude
    void Release(float32 duration, float32 amplitude) {
        this->ReleaseDuration = duration;
        this->ReleaseAmplitude = amplitude;
    };
    

    /// get duration of the entire envelope
    float32 Duration() const {
        return this->AttackDuration +
               this->DecayDuration +
               this->SustainDuration +
               this->ReleaseDuration;
    };
};
    
} // namespace Synth
} // namespace Oryol
