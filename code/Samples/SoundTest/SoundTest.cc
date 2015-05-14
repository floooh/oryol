//------------------------------------------------------------------------------
//  SoundTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "Sound/Sound.h"
#include "IMUI/IMUI.h"
#include "Assets/Sound/SoundGen.h"
#include "glm/trigonometric.hpp"

using namespace Oryol;

class SoundTestApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    struct Effect {
        Id id;
        float* samples = nullptr;   // duplicate for visualization via imgui!
        int numSamples = 0;
        int uiOffset = 0;
        int uiNum = 0;

        ~Effect() {
            if (this->samples) {
                Memory::Free(this->samples);
                this->samples = nullptr;
            }
        };
        float* Alloc(int numSamples_) {
            o_assert(nullptr == this->samples);
            o_assert(numSamples_ > 0);
            this->uiOffset = 0;
            this->uiNum = numSamples_;
            this->numSamples = numSamples_;
            this->samples = (float*) Memory::Alloc(this->numSamples * sizeof(float));
            return this->samples;
        };
    };

    enum EffectId {
        Waka = 0,
        Song1,

        NumEffects
    };

    Effect effects[NumEffects];
    int uiCurEffect = Song1;
    const char* uiEffectNames[NumEffects] = {
        "Waka",
        "Song1"
    };
};
OryolMain(SoundTestApp);

//------------------------------------------------------------------------------
AppState::Code
SoundTestApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(1024, 480, "Sound Test"));
    Input::Setup();
    Sound::Setup(SoundSetup());
    IMUI::Setup();

    this->effects[Waka].id = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(0.25f, 44100, [this](float dt, int16* samples, int numSamples) {

        using namespace SoundGen;
        float* floatSamples = this->effects[Waka].Alloc(numSamples);

        const float32 maxFreq = 500.0f;
        const float32 minFreq = 50.0f;
        const float32 freqStep = ((maxFreq - minFreq) / numSamples) * 2.0f;
        float32 f = maxFreq;
        NamcoVoice voice(NamcoVoice::Pacman2, dt, f, 1.0f);
        float32 t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {
            voice.SetFrequency(f);
            float32 s = voice.Step() * 0.5f;
            const int16 i16 = int16(s * 32767);
            samples[i] = i16;
            floatSamples[i] = s;
            if (t < 0.125f) {
                f -= freqStep;
            }
            else {
                f += freqStep;
            }
        }
    }));

    this->effects[Song1].id = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(4.28f, 44100, [this](float dt, int16* samples, int numSamples) {

        using namespace SoundGen;
        float* floatSamples = this->effects[Song1].Alloc(numSamples);

        NamcoVoice voice0(NamcoVoice::Pacman2, dt, 65.0f, 0.0f);
        NamcoVoice voice1(NamcoVoice::Pacman0, dt, 440.0f, 0.0f);

        float32 t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {

            float val;

            // voice0 is the 'bass'
            float vol;
            if (t < 0.4f) {
                vol = 1.0f - (t / 0.4f);
                vol = vol * vol;
                voice0.SetVolume(vol);
                voice0.SetFrequency(65.0f);
            }
            else if (t < 0.933f) {
                if (t < 0.53f) {
                    vol = 1.0f - ((t - 0.4f) / (0.933f - 0.4f));
                    voice0.SetFrequency(96.0f);
                }
                else {
                    vol = 1.0f - ((t - 0.53f) / (0.933f - 0.53f));
                    voice0.SetFrequency(66.0f);
                }
                vol = vol * vol;
                voice0.SetVolume(vol);
            }
            else if ((t >= 0.933f) && (t < 1.466f)) {
                if (t < 1.07f) {
                    vol = 1.0f - ((t - 0.933f) / (1.466f - 0.933f));
                    voice0.SetFrequency(100.0f);
                }
                else {
                    vol = 1.0f - ((t - 1.07f) / (1.466f - 1.07f));
                    voice0.SetFrequency(72.0f);
                }
                vol = vol * vol;
                voice0.SetVolume(vol);
            }
            else if ((t >= 1.466f) && (t < 2.0f)) {
                if (t < 1.6f) {
                    vol = 1.0f - ((t - 1.466f) / (2.0f - 1.466f));
                    voice0.SetFrequency(102.0f);
                }
                else {
                    vol = 1.0f - ((t - 1.6f) / (2.0f - 1.6f));
                    voice0.SetFrequency(72.0f);
                }
                vol = vol * vol;
                voice0.SetVolume(vol);
            }
            else if ((t >= 2.0f) && (t < 2.53f)) {
                if (t < 2.134f) {
                    vol = 1.0f - ((t - 2.0f) / (2.53f - 2.0f));
                    voice0.SetFrequency(103.0f);
                }
                else {
                    vol = 1.0f - ((t - 2.134f) / (2.53 - 2.134f));
                    voice0.SetFrequency(66.0f);
                }
                vol = vol * vol;
                voice0.SetVolume(vol);
            }
            else if ((t >= 2.53f) && (t < 2.933f)) {
                if (t < 2.667f) {
                    vol = 1.0f - ((t - 2.53f) / (2.933f - 2.53f));
                    voice0.SetFrequency(99.0f);
                }
                else {
                    vol = 1.0f - ((t - 2.667f) / (2.933f - 2.667f));
                    voice0.SetFrequency(65.0f);
                }
                vol = vol * vol;
                voice0.SetVolume(vol);
            }
            else if ((t >= 3.067f) && (t < 3.467f)) {
                voice0.SetFrequency(84.0f);
                if (t < 3.2f) {
                    vol = 1.0f - ((t - 3.067f) / (3.467f - 3.067f));
                }
                else {
                    vol = 1.0f - ((t - 3.2f) / (3.467f - 3.2f));
                }
                vol = vol * vol;
                voice0.SetVolume(vol);
            }
            else if ((t >= 3.467f) && (t < 3.734f)) {
                voice0.SetFrequency(105.0f);
                vol = 1.0f - ((t - 3.467f) / (3.734f - 3.467f));
                vol = vol * vol;
                voice0.SetVolume(vol);
            }
            else if ((t >= 3.734f) && (t < 4.0f)) {
                voice0.SetFrequency(118.0f);
                vol = 1.0f - ((t - 3.734f) / (4.0f - 3.734f));
                vol = vol * vol;
                voice0.SetVolume(vol);
            }
            else if ((t >= 4.0f) && (t < 4.28f)) {
                voice0.SetFrequency(135.0f);
                vol = 1.0f - ((t - 4.0f) / (4.28f - 4.0f));
                vol = vol * vol;
                voice0.SetVolume(vol);
            }
            else {
                voice0.SetVolume(0.0f);
            }

            // the 'bleeps' are on voice1
            if (t < 0.067) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(540.0f);
            }
            else if ((t >= 0.134f) && (t < 0.200f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(1080.0f);
            }
            else if ((t >= 0.266f) && (t < 0.335f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(810.0f);
            }
            else if ((t >= 0.4f) && (t < 0.465f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(700.0f);
            }
            else if ((t >= 0.533) && (t < 0.667f)) {
                voice1.SetVolume(1.0f);
                if (t < 0.6f) {
                    voice1.SetFrequency(1085.0f);
                }
                else {
                    voice1.SetFrequency(810.0f);
                }
            }
            else if ((t >= 0.8f) && (t < 0.933f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(722.0f);
            }
            else if ((t >= 1.07f) && (t < 1.134f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(570.0f);
            }
            else if ((t >= 1.2f) && (t < 1.267f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(1160.0f);
            }
            else if ((t >= 1.334f) && (t < 1.4f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(860.0f);
            }
            else if ((t >= 1.466) && (t < 1.535f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(717.0f);
            }
            else if ((t >= 1.6f) && (t < 1.735f)) {
                voice1.SetVolume(1.0f);
                if (t < 1.667f) {
                    voice1.SetFrequency(1141.0f);
                }
                else {
                    voice1.SetFrequency(865.0f);
                }
            }
            else if ((t >= 1.866f) && (t < 2.0f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(720.0f);
            }
            else if ((t >= 2.135f) && (t < 2.2f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(537.0f);
            }
            else if ((t >= 2.267f) && (t < 2.334f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(1086.0f);
            }
            else if ((t >= 2.4f) && (t < 2.467)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(809.0f);
            }
            else if ((t >= 2.53f) && (t < 2.60f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(676.0f);
            }
            else if ((t >= 2.667) && (t < 2.8f)) {
                voice1.SetVolume(1.0f);
                if (t < 2.733f) {
                    voice1.SetFrequency(1084.0f);
                }
                else {
                    voice1.SetFrequency(809.0f);
                }
            }
            else if ((t >= 2.933f) && (t < 3.067f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(680.0f);
            }
            else if ((t >= 3.2f) && (t < 3.4f)) {
                voice1.SetVolume(1.0f);
                float f0 = 637.0f;
                float f1 = 725.0f;
                float f = f0 + (f1 - f0) * ((t - 3.2f) / (3.4f - 3.2f));
                voice1.SetFrequency(f);
            }
            else if ((t >= 3.467f) && (t < 3.668f)) {
                voice1.SetVolume(1.0f);
                float f0 = 720.0f;
                float f1 = 806.0f;
                float f = f0 + (f1 - f0) * ((t - 3.467) / (3.668f - 3.467f));
                voice1.SetFrequency(f);
            }
            else if ((t >= 3.734f) && (t < 3.934f)) {
                voice1.SetVolume(1.0f);
                float f0 = 804.0f;
                float f1 = 908.0f;
                float f = f0 + (f1 - f0) * ((t - 3.734f) / (3.934f - 3.734f));
                voice1.SetFrequency(f);
            }
            else if ((t >= 4.0f) && (t < 4.134f)) {
                voice1.SetVolume(1.0f);
                voice1.SetFrequency(1084.0f);
            }
            else {
                voice1.SetVolume(0.0f);
            }
            val = voice0.Step() * 0.5f;
            val += voice1.Step() * 0.5f;
            if (val > 1.0f) {
                val = 1.0f;
            }
            else if (val < -1.0f) {
                val = -1.0f;
            }

            const int16 i16 = int16(val * 32767);
            floatSamples[i] = val;
            samples[i] = i16;
        }
    }));

    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SoundTestApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget();
    IMUI::NewFrame();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.75f, 0.75f, 0.75f, 1.0f));

    // draw UI
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("SoundTest", nullptr, ImVec2(1024, 400));
    if (ImGui::Button("Play!")) {
        Sound::Play(this->effects[this->uiCurEffect].id);
    }
    ImGui::SameLine();
    ImGui::Combo("Effect", &this->uiCurEffect, this->uiEffectNames, NumEffects);
    Effect& effect = this->effects[this->uiCurEffect];
    ImGui::PlotLines("##samples", effect.samples + effect.uiOffset, effect.uiNum, 0, nullptr, -1.0f, 1.0f, ImVec2(990, 80));
    ImGui::SliderInt("offset", &effect.uiOffset, 0, effect.numSamples - effect.uiNum);
    ImGui::SliderInt("num", &effect.uiNum, 1, effect.numSamples - effect.uiOffset);
    ImGui::End();

    ImGui::Render();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SoundTestApp::OnCleanup() {
    IMUI::Discard();
    Sound::Discard();
    Input::Discard();
    Gfx::Discard();
    return AppState::Destroy;
}

