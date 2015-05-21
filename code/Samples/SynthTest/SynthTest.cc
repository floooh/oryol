//------------------------------------------------------------------------------
//  SynthTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "IMUI/IMUI.h"
#include "Input/Input.h"
#include "Synth/Synth.h"

using namespace Oryol;

class SynthTestApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    int32 frameCount = 0;
    bool modFreqEnabled = true;
    float volume = 0.05f;
    static const int NumTracks = 4;
    SynthOp freqOp;     // frequency modulation
    SynthOp sndOp;      // sound waveform
    SynthOp volOp;      // volume modulation
};
OryolMain(SynthTestApp);

#define ToAmp(x) (int32)(x * (1<<15))

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(640, 400, "Oryol Synth Test Sample"));
    IMUI::Setup();
    Input::Setup();
    SynthSetup synthSetup;
    synthSetup.UseGPUSynthesizer = false;
    synthSetup.InitialVolume = this->volume;
    Synth::Setup(synthSetup);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.5f, 1.0f, 0.5f, 0.7f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.5f, 1.0f, 0.5f, 0.20f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5f, 0.8f, 0.5f, 0.5f);

    this->freqOp.Op = SynthOp::Replace;
    this->freqOp.Wave = SynthOp::Sine;
    this->freqOp.Freq = 5;
    this->freqOp.Amp = ToAmp(0.5f);
    this->freqOp.Bias = ToAmp(0.25f);
    
    this->sndOp.Op   = SynthOp::ModFreq;
    this->sndOp.Wave = SynthOp::Custom0;
    this->sndOp.Freq = 660;
    this->sndOp.Amp  = (1<<15);
    
    Synth::AddOp(0, 0, this->freqOp);
    Synth::AddOp(0, 1, this->sndOp);

    // type compatibility test between ImGui and SynthOp::WaveT
    o_assert_dbg(sizeof(this->sndOp.Wave) == sizeof(int));
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnRunning() {

    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(ClearTarget::All, glm::vec4(0.5f));

    static const char* waveFormNames[SynthOp::NumWaves] = {
        "Const", "Sine", "SawTooth", "Triangle", "Square", "Noise",
        "Custom0", "Custom1", "Custom2", "Custom3", "Custom4", "Custom5", "Custom6", "Custom7"
    };

    IMUI::NewFrame();
    ImGui::Begin("Synth Test", nullptr, ImVec2(540, 180), 1.0f, 0);
    if (ImGui::SliderFloat("Volume", &this->volume, 0.001f, 1.0f, "%.5f", 3.0f)) {
        Synth::UpdateVolume(this->volume);
    }
    if (ImGui::Checkbox("Enable Frequency Modulation", &this->modFreqEnabled)) {
        if (this->modFreqEnabled) {
            this->sndOp.Op = SynthOp::ModFreq;
        }
        else {
            this->sndOp.Op = SynthOp::Replace;
        }
        Synth::AddOp(0, 1, this->sndOp);
    }
    if (SynthOp::ModFreq == this->sndOp.Op) {
        if (ImGui::SliderInt("Modulation Frequency", &this->freqOp.Freq, 1, 60)) {
            Synth::AddOp(0, 0, this->freqOp);
        }
        if (ImGui::Combo("Modulation Waveform", (int*) &this->freqOp.Wave, waveFormNames, SynthOp::NumWaves)) {
            Synth::AddOp(0, 0, this->freqOp);
        }
    }
    if (ImGui::SliderInt("Sound Frequency", &this->sndOp.Freq, 11, 4400)) {
        Synth::AddOp(0, 1, this->sndOp);
    }
    if (ImGui::Combo("Sound Waveform", (int*) &this->sndOp.Wave, waveFormNames, SynthOp::NumWaves)) {
        Synth::AddOp(0, 1, this->sndOp);
    }
    ImGui::End();

    Synth::Update();
    ImGui::Render();
    Gfx::CommitFrame();
    this->frameCount++;
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnCleanup() {
    Synth::Discard();
    Input::Discard();
    IMUI::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
