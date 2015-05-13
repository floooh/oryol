//------------------------------------------------------------------------------
//  SoundTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "Sound/Sound.h"
#include "Assets/Sound/Freq.h"
#include "Assets/Sound/Wave.h"
#include "Assets/Sound/Fade.h"
#include "glm/trigonometric.hpp"

using namespace Oryol;

class SoundTestApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    Id waka;
    Id brom;
    Id bleep;
};
OryolMain(SoundTestApp);

//------------------------------------------------------------------------------
AppState::Code
SoundTestApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(640, 480, "Sound Test"));
    Input::Setup();
    Sound::Setup(SoundSetup());

    this->waka = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(0.25f, 44100, [](float dt, int16* samples, int numSamples) {
        int32 wave2[32] = {
            // Pacman wave2
            7, 10, 12, 13, 14, 13, 12, 10,  7,  4,  2,  1,  0,  1,  2,  4,
            7, 11, 13, 14, 13, 11,  7,  3,  1,  0,  1,  3,  7,  14,  7, 0
        };
        Freq freq(dt);
        const float32 maxFreq = 500.0f;
        const float32 minFreq = 50.0f;
        const float32 freqStep = ((maxFreq - minFreq) / numSamples) * 2.0f;
        float32 f = maxFreq;
        float32 t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (t < 0.125f) {
                f -= freqStep;
            }
            else {
                f += freqStep;
            }
            float32 pos = freq.Step(f);
            int32 wave2Index = int32(32.0f * pos);
            o_assert_dbg((wave2Index >= 0) && (wave2Index < 32));
            float32 wackaValue = (float32(wave2[wave2Index]) / 8.0f) - 1.0f;
            float32 s = wackaValue;
            const int16 i16 = int16(s * 32767);
            samples[i] = i16;
        }
    }));

    this->brom = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(0.4f, 44100, [](float dt, int16* samples, int numSamples) {
        int32 wave0[32] = {
            // Pacman wave0
            7, 9, 10, 11, 12, 13, 13, 14, 14, 14, 13, 13, 12, 11, 10, 9,
            7, 5,  4,  3,  2,  1,  1,  0,  0,  0,  1,  1,  2,  3,  4, 5
        };
        int32 wave2[32] = {
            // Pacman wave2
            7, 10, 12, 13, 14, 13, 12, 10,  7,  4,  2,  1,  0,  1,  2,  4,
            7, 11, 13, 14, 13, 11,  7,  3,  1,  0,  1,  3,  7,  14,  7, 0
        };

        Freq bromFreq(dt, 65.0f);
        Freq bleep0Freq(dt, 539.0f);
        Freq bleep1Freq(dt, 1080.0f);
        Freq bleep2Freq(dt, 809.0f);

        float32 t = 0.0f;
        float32 amp = 1.0f;
        float32 damp = amp / numSamples;
        for (int i = 0; i < numSamples; i++, t += dt, amp -= damp) {

            float32 pos = bromFreq.Step();
            int32 waveIndex = int32(32.0f * pos);
            float32 val = ((float32(wave2[waveIndex]) / 8.0f) - 1.0f) * (amp * amp);
            if (t < 0.067) {
                pos = bleep0Freq.Step();
                waveIndex = int32(32.0f * pos);
                val += ((float32(wave0[waveIndex]) / 8.0f) - 1.0f);
            }
            else if ((t >= 0.134f) && (t < 0.200f)) {
                pos = bleep1Freq.Step();
                waveIndex = int32(32.0f * pos);
                val += ((float32(wave0[waveIndex]) / 8.0f) - 1.0f);
            }
            else if ((t >= 0.266f) && (t < 0.335f)) {
                pos = bleep2Freq.Step();
                waveIndex = int32(32.0f * pos);
                val += ((float32(wave0[waveIndex]) / 8.0f) - 1.0f);
            }
            if (val > 1.0f) {
                val = 1.0f;
            }
            else if (val < -1.0f) {
                val = -1.0f;
            }

            const int16 i16 = int16(val * 32767);
            samples[i] = i16;
        }
    }));

    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SoundTestApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.75f, 0.75f, 0.75f, 1.0f));
    if (Input::Keyboard().KeyDown(Key::N1)) {
        Sound::Play(this->waka);
    }
    if (Input::Keyboard().KeyDown(Key::N2)) {
        Sound::Play(this->brom);
    }
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SoundTestApp::OnCleanup() {
    Sound::Discard();
    Input::Discard();
    Gfx::Discard();
    return AppState::Destroy;
}

