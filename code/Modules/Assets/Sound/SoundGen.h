#pragma once
//------------------------------------------------------------------------------
/**
    Small sound generation helper classes.
*/
#include "Core/Types.h"
#include "Core/Assertion.h"
#include <cmath>
#include <cstdlib>

namespace Oryol {
namespace SoundGen {

//------------------------------------------------------------------------------
/**
    @class Oryol::SoundGen::Freq
    @ingroup Assets
    @brief maintain sample position for frequency
    
    The Freq object is initialized with a sample duration in seconds,
    and a frequency (which can optionally be dynamic). Each call to 
    the Step() method returns a sample position normalized to the
    0.0..1.0 range, and advanced the sample position. The resulting
    sample position can directly be fed into wave generators.
*/
class Freq {
public:
    /// construct with sample duration (default frequency is 440Hz)
    Freq(float32 dt_) {
        o_assert_dbg(dt_ > 0.0f);
        this->dt = dt_;
        this->step = this->dt * 440.0f;
        this->pos = 0.0f;
    };
    /// construct with sample duration and frequency
    Freq(float32 dt_, float32 freq) {
        o_assert_dbg(dt_ > 0.0f);
        o_assert_dbg(freq > 0.0f);
        this->dt = dt_;
        this->step = this->dt * freq;
        this->pos = 0.0f;
    };
    /// get next sample position, and advance
    float32 Step() {
        float32 p = this->pos;
        this->pos += this->step;
        if (this->pos >= 1.0f) {
            this->pos = std::fmod(this->pos, 1.0f);
        }
        return p;
    };
    /// get next sample position with variable frequency, and advance
    float32 Step(float32 freq) {
        o_assert_dbg(freq > 0.0f);
        this->step = this->dt * freq;
        float32 p = this->pos;
        this->pos += this->step;
        if (this->pos >= 1.0f) {
            this->pos = std::fmod(this->pos, 1.0f);
        }
        return p;
    };

private:
    float32 dt;             // duration of one sample in seconds
    float32 step;           // current step size
    float32 pos;            // current sample position
};

//------------------------------------------------------------------------------
/**
    @class Oryol::SoundGen::Wave
    @ingroup Assets
    @brief helper class to generate sample wave data
    
    The wave generator generates a sine, square, triangle and
    saw-tooth sample from a sample position usually provided 
    by a Freq object. The sample pos parameter is expected
    to be normalized to 1 Hz (so, pos is 0.0 at start, and 1.0 at
    a full second).
*/
class Wave {
public:
    /// get next sine wave value at normalized sample position
    static float32 Sine(float32 p) {
        return std::sin(p * M_PI * 2.0f);
    };
    /// get next square wave value at normalized sample position
    static float32 Square(float32 p) {
        return (p < 0.5f) ? -1.0f : 1.0f;
    };
    /// get next triangle wave value at normalized sample position
    static float32 Triangle(float32 p) {
        // FIXME: this should probably return 2 cycles
        if (p < 0.5f) {
            return (p * 4.0f) - 1.0f;
        }
        else {
            return 1.0f - ((p - 0.5f) * 4.0f);
        }
    };
    /// get sawtooth wave value at normalized sample position
    static float32 SawTooth(float32 p) {
        // FIXME: this should probably return 2 cycles
        return 1.0f - (p * 2.0f);
    };
    /// get inverse-sawtooth wave value at normalized sample position
    static float32 InvSawTooth(float32 p) {
        // FIXME: this should probably return 2 cycles
        return (p * 2.0f) - 1.0f;
    };
    /// return random noise
    static float32 RandNoise() {
        return (float32(std::rand() & 0xffff) / 32768.0f) - 1.0f;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::SoundGen::Fade
    @ingroup Assets
    @brief fade-in/out modulator for sound sample generation
    
    FIXME: this should give more options then just linear fading
    (e.g. http://easings.net/ )
*/
class Fade {
public:
    /// initialize with effect duration, fade-in and fade-out in seconds
    Fade(float32 bufferDuration, float32 fadeInDuration, float32 fadeOutDuration) {
        o_assert_dbg(bufferDuration > 0.0f);
        o_assert_dbg(fadeInDuration >= 0.0f);
        o_assert_dbg(fadeOutDuration >= 0.0f);
        this->duration = bufferDuration;
        this->fadeInPos = fadeInDuration;
        this->fadeOutPos = this->duration - fadeOutDuration;
        this->fadeInMul = 1.0f / fadeInDuration;
        this->fadeOutMul = 1.0f / fadeOutDuration;
    };
    /// get curren modulator (0.0 .. 1.0) at time position
    float32 Value(float32 t) const {
        if (t < this->fadeInPos) {
            return t * this->fadeInMul;
        }
        else if (t >= this->fadeOutPos) {
            return 1.0f - ((t - this->fadeOutPos) * this->fadeOutMul);
        }
        else {
            return 1.0f;
        }
    };
private:
    float32 duration;
    float32 fadeInPos;
    float32 fadeOutPos;
    float32 fadeInMul;
    float32 fadeOutMul;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::SoundGen::NamcoVoice
    @ingroup Assets
    @brief Namco WSG sound chip helper functions
    
    See here: http://www.lomont.org/Software/Games/PacMan/PacmanEmulation.pdf
    And here: http://www.vecoven.com/elec/pacman/code/pac_z80.html
    
    FIXME: rewrite the whole math to integer, and exactly to Namco spec!

*/
class NamcoVoice {
public:
    /// waveform Ids from the Pacman ROM:
    enum Wave {
        Pacman0 = 0,
        Pacman1,
        Pacman2,
        Pacman3,
        Pacman4,
        Pacman5,
        Pacman6,
        Pacman7,
        Pacman8,

        NumWaves
    };

    /// the number of samples in a wave form
    static const int NumWaveSamples = 32;
    /// the actual waveform data (see SoundGen.cc)
    static const char WaveData[NumWaves][NumWaveSamples];

    /// construct from WaveForm, sample duration, frequency and volume
    NamcoVoice(Wave wave_, float32 dt_, float32 freq, float32 vol) {
        this->wave = wave_;
        this->dt = dt_;
        this->step = this->dt * freq;
        this->pos = 0.0f;
        this->volume = vol;
    };

    /// set new wave form
    void SetWave(Wave wave_) {
        o_assert_range_dbg(wave, NumWaves);
        this->wave = wave_;
    };
    /// set new frequency
    void SetFrequency(float32 freq) {
        this->step = this->dt * freq;
    };
    /// set new volume
    void SetVolume(float32 vol) {
        this->volume = vol;
    };
    /// step the voice generator, return new sample value in range -1.0f..1.0f
    float32 Step() {
        if (this->volume > 0.0f) {
            int32 waveIndex = int32(32.0f * this->pos);
            float32 s = ((float32(WaveData[this->wave][waveIndex]) / 8.0f) - 1.0f);
            s *= this->volume;
            this->pos += this->step;
            if (this->pos >= 1.0f) {
                this->pos = std::fmod(this->pos, 1.0f);
            }
            return s;
        }
        else {
            return 0.0f;
        }
    };

private:
    Wave wave;
    float32 dt;
    float32 step;
    float32 pos;
    float32 volume;
};

} // namespace Oryol
} // namespace SoundGen
