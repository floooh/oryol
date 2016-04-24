//------------------------------------------------------------------------------
//  SoundTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
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

    ClearState clearState;
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
        Power1,
        EatGhost,

        NumEffects
    };

    Effect effects[NumEffects];
    int uiCurEffect = Song1;
    static const char* uiEffectNames[NumEffects];
};
OryolMain(SoundTestApp);

const char* SoundTestApp::uiEffectNames[SoundTestApp::NumEffects] = {
    "Waka",
    "Song1",
    "Power1",
    "EatGhost"
};

//------------------------------------------------------------------------------
AppState::Code
SoundTestApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(1024, 480, "Sound Test"));
    Input::Setup();
    Sound::Setup(SoundSetup());
    IMUI::Setup();
    this->clearState.Color = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);

    this->effects[Waka].id = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(1, 0.25f, 44100, [this](float dt, int16_t* samples, int numSamples) {

        using namespace SoundGen;
        float* floatSamples = this->effects[Waka].Alloc(numSamples);

        const float maxFreq = 500.0f;
        const float minFreq = 50.0f;
        NamcoVoice voice(dt, NamcoVoice::Pacman2);
        float t = 0.0f;
        Range range;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (range.In(t, 0.0f, 0.1f)) {
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, maxFreq, minFreq);
            }
            else if (range.In(t, 0.15f, 0.25f)) {
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, minFreq, maxFreq);
            }
            float s = voice.Step() * 0.5f;
            samples[i] = Sample::Int16(s);
            floatSamples[i] = Sample::Float32(s);
        }
    }));

    this->effects[Song1].id = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(1, 4.28f, 44100, [this](float dt, int16_t* samples, int numSamples) {

        using namespace SoundGen;
        float* floatSamples = this->effects[Song1].Alloc(numSamples);

        NamcoVoice voice0(dt, NamcoVoice::Pacman2);
        NamcoVoice voice1(dt, NamcoVoice::Pacman0);
        Range range;
        float t = 0.0f;
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
            if (range.In(t, 0.0f, 0.067f)) {
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
            else if (range.In(t, 0.533f, 0.667f)) {
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
            else if (range.In(t, 1.466f, 1.535f)) {
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
            else if (range.In(t, 2.4f, 2.467f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 809.0f;
            }
            else if (range.In(t, 2.53f, 2.60f)) {
                voice1.Volume = 1.0f;
                voice1.Frequency = 676.0f;
            }
            else if (range.In(t, 2.667f, 2.8f)) {
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
            float val = voice0.Step();
            val += voice1.Step();
            val *= 0.5f;

            floatSamples[i] = Sample::Float32(val);
            samples[i] = Sample::Int16(val);
        }
    }));

    this->effects[Power1].id = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(1, 0.734f, 44100, [this](float dt, int16_t* samples, int numSamples) {
        using namespace SoundGen;

        float* floatSamples = this->effects[Power1].Alloc(numSamples);
        NamcoVoice voice(dt, NamcoVoice::Pacman4);
        Range range;
        float t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (range.In(t, 0.0f, 0.066f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, 166.0f, 250.0f);
            }
            else if (range.In(t, 0.066f, 0.2f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, 50.0f, 250.0f);
            }
            else if (range.In(t, 0.2f, 0.333f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, 50.0f, 250.0f);
            }
            else if (range.In(t, 0.334f, 0.467f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, 50.0f, 250.0f);
            }
            else if (range.In(t, 0.467f, 0.6f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, 50.0f, 250.0f);
            }
            else if (range.In(t, 0.6f, 0.734f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, 50.0f, 250.0f);
            }
            else {
                voice.Volume = 0.0f;
            }
            float val = voice.Step();
            val *= 0.5f;

            floatSamples[i] = Sample::Float32(val);
            samples[i] = Sample::Int16(val);
        }
    }));

    this->effects[EatGhost].id = Sound::CreateResource(SoundEffectSetup::FromSampleFunc(1, 0.512f, 44100, [this](float dt, int16_t* samples, int numSamples) {
        using namespace SoundGen;

        float* floatSamples = this->effects[EatGhost].Alloc(numSamples);
        NamcoVoice voice(dt, NamcoVoice::Pacman4);
        Range range;
        float t = 0.0f;
        for (int i = 0; i < numSamples; i++, t += dt) {
            if (range.In(t, 0.0f, 0.512f)) {
                voice.Volume = 1.0f;
                voice.Frequency = Mod::Lerp(t, range.Begin, range.End, 10.0f, 220.0f);
            }
            else {
                voice.Volume = 0.0f;
            }
            float val = voice.Step() * 0.75f;
            floatSamples[i] = Sample::Float32(val);
            samples[i] = Sample::Int16(val);
        }
    }));

    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SoundTestApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget(this->clearState);
    IMUI::NewFrame();

    // draw UI
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("SoundTest", nullptr, ImVec2(1024, 400));
    if (ImGui::Button("Play!")) {
        Sound::Play(this->effects[this->uiCurEffect].id);
    }
    ImGui::SameLine();
    ImGui::Combo("Effect", &this->uiCurEffect, this->uiEffectNames, NumEffects);
    Effect& effect = this->effects[this->uiCurEffect];
    if (effect.samples) {
        ImGui::PlotLines("##samples", effect.samples + effect.uiOffset, effect.uiNum, 0, nullptr, -1.0f, 1.0f, ImVec2(990, 80));
        ImGui::SliderInt("offset", &effect.uiOffset, 0, effect.numSamples - effect.uiNum);
        ImGui::SliderInt("num", &effect.uiNum, 1, effect.numSamples - effect.uiOffset);
    }
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

