//------------------------------------------------------------------------------
//  SoundTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "Sound/Sound.h"
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

    this->sound = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(2000, [](float t, float dt, int16* samples, int numSamples) {
        for (int i = 0; i < numSamples; i++, t += dt * 4000.0f) {
            const int16 sample = int16(glm::sin(t) * 32767);
            samples[i] = sample;
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

