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

    Id sound;
};
OryolMain(SoundTestApp);

//------------------------------------------------------------------------------
AppState::Code
SoundTestApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(640, 480, "Sound Test"));
    Input::Setup();
    Sound::Setup(SoundSetup());

    this->sound = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(0.25f, 22050, [](float dt, int16* samples, int numSamples) {
        int32 wacka[32] = {
            // Pacman wave2
            7, 10, 12, 13, 14, 13, 12, 10,  7,  4,  2,  1,  0,  1,  2,  4,
            7, 11, 13, 14, 13, 11,  7,  3,  1,  0,  1,  3,  7,  14,  7, 0
        };
        Freq freq(dt);
        float f = 400.0f;
        float32 t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (t < 0.125f) {
                f -= 0.1f;
            }
            else {
                f += 0.1f;
            }
            float32 pos = freq.Step(f);
            int32 wackaIndex = int32(31.0f * pos);
            o_assert_dbg((wackaIndex >= 0) && (wackaIndex < 32));
            float32 wackaValue = (float32(wacka[wackaIndex]) / 8.0f) - 1.0f;
            float32 s = wackaValue;
            const int16 i16 = int16(s * 32767);
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
    if (Input::Keyboard().KeyDown(Key::Space)) {
        Sound::Play(this->sound);
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

