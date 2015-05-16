//------------------------------------------------------------------------------
//  sound.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "sound.h"
#include "Sound/Sound.h"
#include "Assets/Sound/SoundGen.h"

using namespace Oryol;
using namespace SoundGen;

namespace Paclone {

//------------------------------------------------------------------------------
void
sound::CreateSoundEffects() {

    // the waka wake sound effect
    this->wa = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(1, 0.125f, 44100, [](float dt, int16* samples, int numSamples) {
        const float32 maxFreq = 500.0f;
        const float32 minFreq = 50.0f;
        NamcoVoice voice(dt, NamcoVoice::Pacman2);
        float32 t = 0.0f;
        Range range;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (range.In(t, 0.0f, 0.125f)) {
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, maxFreq, minFreq);
            }
            samples[i] = Sample::Int16(voice.Step() * 0.3f);
        }
    }));
    this->ka = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(1, 0.125f, 44100, [](float dt, int16* samples, int numSamples) {
        const float32 maxFreq = 500.0f;
        const float32 minFreq = 50.0f;
        NamcoVoice voice(dt, NamcoVoice::Pacman2);
        float32 t = 0.0f;
        Range range;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (range.In(t, 0.0f, 0.125f)) {
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, minFreq, maxFreq);
            }
            samples[i] = Sample::Int16(voice.Step() * 0.3f);
        }
    }));

    // the intro song
    this->introSong = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(1, 4.28f, 44100, [](float dt, int16* samples, int numSamples) {
        NamcoVoice voice0(dt, NamcoVoice::Pacman2);
        NamcoVoice voice1(dt, NamcoVoice::Pacman0);
        Range range;
        float32 t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {

            // voice0 is the 'bass'
            if (range.In(t, 0.0f, 0.4f)) {
                voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                voice0.Frequency = 65.0f;
            }
            else if (range.In(t, 0.4f, 0.933f)) {
                if (range.BeforeBegin(t, 0.53f)) {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 96.0f;
                }
                else {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 66.0f;
                }
            }
            else if (range.In(t, 0.933f, 1.466f)) {
                if (range.BeforeBegin(t, 1.07f)) {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 100.0f;
                }
                else {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 72.0f;
                }
            }
            else if (range.In(t, 1.466f, 2.0f)) {
                if (range.BeforeBegin(t, 1.6f)) {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 102.0f;
                }
                else {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 72.0f;
                }
            }
            else if (range.In(t, 2.0f, 2.53f)) {
                if (range.BeforeBegin(t, 2.134f)) {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 103.0f;
                }
                else {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 66.0f;
                }
            }
            else if (range.In(t, 2.53f, 2.933f)) {
                if (range.BeforeBegin(t, 2.667f)) {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 99.0f;
                }
                else {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                    voice0.Frequency = 65.0f;
                }
            }
            else if (range.In(t, 3.067f, 3.467f)) {
                voice0.Frequency = 84.0f;
                if (range.BeforeBegin(t, 3.2f)) {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                }
                else {
                    voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
                }
            }
            else if (range.In(t, 3.467f, 3.734f)) {
                voice0.Frequency = 105.0f;
                voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
            }
            else if (range.In(t, 3.734f, 4.0f)) {
                voice0.Frequency = 118.0f;
                voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
            }
            else if (range.In(t, 4.0f, 4.28f)) {
                voice0.Frequency = 135.0f;
                voice0.Volume = Mod::FadeOutSq(t, range.Begin, range.End);
            }
            else {
                voice0.Volume = 0.0f;
            }

            // the 'bleeps' are on voice1
            if (range.In(t, 0.0f, 0.067)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 540.0f;
            }
            else if (range.In(t, 0.134f, 0.200f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 1080.0f;
            }
            else if (range.In(t, 0.266f, 0.335f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 810.0f;
            }
            else if (range.In(t, 0.4f, 0.465f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 700.0f;
            }
            else if (range.In(t, 0.533, 0.667f)) {
                voice1.Volume = 1.0f;
                if (t < 0.6f) {
                    voice1.Frequency = 1085.0f;
                }
                else {
                    voice1.Frequency = 810.0f;
                }
            }
            else if (range.In(t, 0.8f, 0.933f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 722.0f;
            }
            else if (range.In(t, 1.07f, 1.134f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 570.0f;
            }
            else if (range.In(t, 1.2f, 1.267f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 1160.0f;
            }
            else if (range.In(t, 1.334f, 1.4f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 860.0f;
            }
            else if (range.In(t, 1.466, 1.535f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 717.0f;
            }
            else if (range.In(t, 1.6f, 1.735f)) {
                voice1.Volume = 1.0f;
                if (t < 1.667f) {
                    voice1.Frequency = 1141.0f;
                }
                else {
                    voice1.Frequency = 865.0f;
                }
            }
            else if (range.In(t, 1.866f, 2.0f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 720.0f;
            }
            else if (range.In(t, 2.135f, 2.2f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 537.0f;
            }
            else if (range.In(t, 2.267f, 2.334f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 1086.0f;
            }
            else if (range.In(t, 2.4f, 2.467)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 809.0f;
            }
            else if (range.In(t, 2.53f, 2.60f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 676.0f;
            }
            else if (range.In(t, 2.667, 2.8f)) {
                voice1.Volume = 1.0f;
                if (t < 2.733f) {
                    voice1.Frequency = 1084.0f;
                }
                else {
                    voice1.Frequency = 809.0f;
                }
            }
            else if (range.In(t, 2.933f, 3.067f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 680.0f;
            }
            else if (range.In(t, 3.2f, 3.4f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = Mod::Lerp(t, range.Begin, range.End, 637.0f, 725.0f);
            }
            else if (range.In(t, 3.467f, 3.668f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = Mod::Lerp(t, range.Begin, range.End, 720.0f, 806.0f);
            }
            else if (range.In(t, 3.734f, 3.934f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = Mod::Lerp(t, range.Begin, range.End, 804.0f, 908.0f);
            }
            else if (range.In(t, 4.0f, 4.134f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 1084.0f;
            }
            else {
                voice1.Volume = 0.0f;
            }
            samples[i] = Sample::Int16((voice0.Step() + voice1.Step()) * 0.5f);
        }
    }));

    // ghost frightened effect
    this->ghostFrightened = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(1, 0.1333f, 44100, [](float dt, int16* samples, int numSamples) {
        NamcoVoice voice(dt, NamcoVoice::Pacman4);
        Range range;
        float32 t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (range.In(t, 0.0f, 0.1333f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, 50.0f, 250.0f);
            }
            else {
                voice.Volume = 0.0f;
            }
            samples[i] = Sample::Int16(voice.Step() * 0.5f);
        }
    }));

    /// ghost normal background loop
    this->ghostNormal = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(2, 0.4f, 44100, [] (float dt, int16* samples, int numSamples) {
        NamcoVoice voice(dt, NamcoVoice::Pacman6);
        Range range;
        const float32 minFreq = 440.0f;
        const float32 maxFreq = 900.0f;
        float32 t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (range.In(t, 0.0f, 0.2f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, minFreq, maxFreq);
            }
            else if (range.In(t, 0.2f, 0.4f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, maxFreq, minFreq);
            }
            else {
                voice.Volume = 0.0f;
            }
            float val = voice.Step();
            val *= Mod::FadeIn(t, 0.0f, 0.01f);
            val *= Mod::FadeOut(t, 0.39f, 0.4f);
            samples[i] = Sample::Int16(val * 0.3f);
        }
    }));

    /// ghost alarm loop
    this->ghostAlarm = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(2, 0.25f, 44100, [] (float dt, int16* samples, int numSamples) {
        NamcoVoice voice(dt, NamcoVoice::Pacman0);
        Range range;
        const float32 maxFreq = 2500.0f;
        const float32 minFreq = 600.0f;
        float t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (range.In(t, 0.0f, 0.25f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, maxFreq, minFreq);
            }
            else {
                voice.Volume = 0.0f;
            }
            float val = voice.Step();
            val *= Mod::FadeIn(t, 0.0f, 0.01f);
            val *= Mod::FadeOut(t, 0.24f, 0.25f);
            samples[i] = Sample::Int16(val * 0.4f);
        }
    }));

    // eat ghost effect
    this->eatGhost = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(1, 0.512, 44100, [this](float dt, int16* samples, int numSamples) {
        NamcoVoice voice(dt, NamcoVoice::Pacman4);
        Range range;
        float32 t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (range.In(t, 0.0f, 0.512f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, 10.0f, 220.0f);
            }
            else {
                voice.Volume = 0.0f;
            }
            samples[i] = Sample::Int16(voice.Step() * 0.75f);
        }
    }));
}

//------------------------------------------------------------------------------
void
sound::Reset() {
    this->ghostFrightenedTick = 0;
    this->ghostNormalTick = -GhostNormalLengthTicks;
    this->ghostAlarmTick = 0;
}

//------------------------------------------------------------------------------
void
sound::IntroSong() {
    Sound::Play(this->introSong);
}

//------------------------------------------------------------------------------
void
sound::Wa() {
    Sound::Play(this->wa);
}

//------------------------------------------------------------------------------
void
sound::Ka() {
    Sound::Play(this->ka);
}

//------------------------------------------------------------------------------
void
sound::GhostFrightened(int tick) {
    if ((tick - this->ghostFrightenedTick) >= GhostFrightenedLengthTicks) {
        this->ghostFrightenedTick = tick;
        Sound::Play(this->ghostFrightened);
    }
}

//------------------------------------------------------------------------------
void
sound::GhostNormal(int tick) {
    if ((tick - this->ghostNormalTick) >= GhostNormalLengthTicks) {
        this->ghostNormalTick = tick;
        Sound::Play(this->ghostNormal);
    }
}

//------------------------------------------------------------------------------
void
sound::GhostAlarm(int tick) {
    if ((tick - this->ghostAlarmTick) >= GhostAlarmLengthTicks) {
        this->ghostAlarmTick = tick;
        Sound::Play(this->ghostAlarm);
    }
}

//------------------------------------------------------------------------------
void
sound::EatGhost() {
    Sound::Play(this->eatGhost);
}

} // namespace Paclone
