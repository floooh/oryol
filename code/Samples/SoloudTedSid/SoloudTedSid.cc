//------------------------------------------------------------------------------
//  SouloudTedSid.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "IMUI/IMUI.h"
#include "soloud.h"
#include "soloud_tedsid.h"
#include "soloud_biquadresonantfilter.h"
#include "soloud_echofilter.h"
#include "soloud_dcremovalfilter.h"
#include "soloud_bassboostfilter.h"

using namespace Oryol;

class SoloudTedSidApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();

private:
    SoLoud::Soloud soloud;
    SoLoud::TedSid music1, music2;
    SoLoud::BiquadResonantFilter biquad;
    SoLoud::EchoFilter echo;
    SoLoud::DCRemovalFilter dcRemoval;
    SoLoud::BassboostFilter bassBoost;
    int musicHandle1 = 0;
    int musicHandle2 = 0;
    float filterParam0[4];
    float filterParam1[4];
    float filterParam2[4];
    float music1Volume = 1.0f;
    float music2Volume = 0.0f;
    Buffer music1Data, music2Data;
};
OryolMain(SoloudTedSidApp);

//------------------------------------------------------------------------------
AppState::Code
SoloudTedSidApp::OnRunning() {

    this->soloud.setFilterParameter(0, 0, 0, this->filterParam0[0]);
    this->soloud.setFilterParameter(0, 0, 2, this->filterParam1[0]);
    this->soloud.setFilterParameter(0, 0, 3, this->filterParam2[0]);
    this->soloud.setFilterParameter(0, 1, 0, this->filterParam0[1]);
    this->soloud.setFilterParameter(0, 1, 1, this->filterParam1[1]);
    this->soloud.setFilterParameter(0, 2, 0, this->filterParam0[2]);
    this->soloud.setFilterParameter(0, 3, 0, this->filterParam0[3]);

    float* buf = this->soloud.getWave();
    float* fft = this->soloud.calcFFT();

    Gfx::ApplyDefaultRenderTarget(ClearState::ClearColor(glm::vec4(0.2f, 0.4f, 0.8f, 1.0f)));
    IMUI::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(500, 20), ImGuiSetCond_Once);
    ImGui::Begin("Output");
    ImGui::PlotLines("##Wave", buf, 256, 0, "Wave", -1, 1, ImVec2(264, 80));
    ImGui::PlotHistogram("##FFT", fft, 256/2, 0, "FFT", 0, 10, ImVec2(264, 80), 8);
    float sidregs[32 + 5];
    for (int i = 0; i < 32; i++) {
        sidregs[i] = this->soloud.getInfo(this->musicHandle1, i);
    }
    ImGui::PlotHistogram("##SID", sidregs, 32 + 5, 0, "          SID               TED", 0, 0xff, ImVec2(264, 80), 4);
    ImGui::Text("SID: %02X %02X %02X %02X %02X %02X %02X %02X",
        (int)this->soloud.getInfo(this->musicHandle1, 0),
        (int)this->soloud.getInfo(this->musicHandle1, 1),
        (int)this->soloud.getInfo(this->musicHandle1, 2),
        (int)this->soloud.getInfo(this->musicHandle1, 3),
        (int)this->soloud.getInfo(this->musicHandle1, 4),
        (int)this->soloud.getInfo(this->musicHandle1, 5),
        (int)this->soloud.getInfo(this->musicHandle1, 6),
        (int)this->soloud.getInfo(this->musicHandle1, 7));
    ImGui::Text("     %02X %02X %02X %02X %02X %02X %02X %02X",
        (int)this->soloud.getInfo(this->musicHandle1, 8),
        (int)this->soloud.getInfo(this->musicHandle1, 9),
        (int)this->soloud.getInfo(this->musicHandle1, 10),
        (int)this->soloud.getInfo(this->musicHandle1, 11),
        (int)this->soloud.getInfo(this->musicHandle1, 12),
        (int)this->soloud.getInfo(this->musicHandle1, 13),
        (int)this->soloud.getInfo(this->musicHandle1, 14),
        (int)this->soloud.getInfo(this->musicHandle1, 15));
    ImGui::Text("     %02X %02X %02X %02X %02X %02X %02X %02X",
        (int)this->soloud.getInfo(this->musicHandle1, 16),
        (int)this->soloud.getInfo(this->musicHandle1, 17),
        (int)this->soloud.getInfo(this->musicHandle1, 18),
        (int)this->soloud.getInfo(this->musicHandle1, 19),
        (int)this->soloud.getInfo(this->musicHandle1, 20),
        (int)this->soloud.getInfo(this->musicHandle1, 21),
        (int)this->soloud.getInfo(this->musicHandle1, 22),
        (int)this->soloud.getInfo(this->musicHandle1, 23));
    ImGui::Text("     %02X %02X %02X %02X %02X %02X %02X %02X",
        (int)this->soloud.getInfo(this->musicHandle1, 24),
        (int)this->soloud.getInfo(this->musicHandle1, 25),
        (int)this->soloud.getInfo(this->musicHandle1, 26),
        (int)this->soloud.getInfo(this->musicHandle1, 27),
        (int)this->soloud.getInfo(this->musicHandle1, 28),
        (int)this->soloud.getInfo(this->musicHandle1, 29),
        (int)this->soloud.getInfo(this->musicHandle1, 30),
        (int)this->soloud.getInfo(this->musicHandle1, 31));
    ImGui::Text("TED: %02X %02X %02X %02X %02X",
        (int)this->soloud.getInfo(this->musicHandle2, 64),
        (int)this->soloud.getInfo(this->musicHandle2, 65),
        (int)this->soloud.getInfo(this->musicHandle2, 66),
        (int)this->soloud.getInfo(this->musicHandle2, 67),
        (int)this->soloud.getInfo(this->musicHandle2, 68));
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_Once);
    ImGui::Begin("Control", nullptr, ImVec2(300, -1));
    ImGui::Text("Song volumes");
    if (ImGui::SliderFloat("Song1 vol", &this->music1Volume, 0, 1)) {
        this->soloud.setVolume(this->musicHandle1, this->music1Volume);
    }
    if (ImGui::SliderFloat("Song2 vol", &this->music2Volume, 0, 1)) {
        this->soloud.setVolume(this->musicHandle2, this->music2Volume);
    }
    ImGui::Separator();
    ImGui::Text("Biquad filter (lowpass)");
    ImGui::SliderFloat("Wet##4", &this->filterParam0[0], 0, 1);
    ImGui::SliderFloat("Frequency##4", &this->filterParam1[0], 0, 8000);
    ImGui::SliderFloat("Resonance##4", &this->filterParam2[0], 1, 20);
    ImGui::Separator();
    ImGui::Text("Bassboost filter");
    ImGui::SliderFloat("Wet##2", &this->filterParam0[1], 0, 1);
    ImGui::SliderFloat("Boost##2", &this->filterParam1[1], 0, 11);
    ImGui::Separator();
    ImGui::Text("Echo filter");
    ImGui::SliderFloat("Wet##3", &this->filterParam0[2], 0, 1);
    ImGui::Separator();
    ImGui::Text("DC removal filter");
    ImGui::SliderFloat("Wet##1", &this->filterParam0[3], 0, 1);
    ImGui::End();

    ImGui::Render();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SoloudTedSidApp::OnInit() {

    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    #if ORYOL_DEBUG
    ioSetup.Assigns.Add("snd:", "http://localhost:8000/");
    #else
    ioSetup.Assigns.Add("snd:", ORYOL_SAMPLE_URL);
    #endif
    IO::Setup(ioSetup);

    Gfx::Setup(GfxSetup::Window(800, 460, "SoLoud TED/SID Demo"));
    Input::Setup();
    IMUI::Setup();

    // need to compile in VS2013, can't use new initializers
    Memory::Clear(this->filterParam0, sizeof(this->filterParam0));
    Memory::Clear(this->filterParam1, sizeof(this->filterParam1));
    Memory::Clear(this->filterParam2, sizeof(this->filterParam2));
    this->filterParam1[0] = 1000;   
    this->filterParam1[1] = 2;
    this->filterParam2[0] = 2;
    this->music1.setLooping(true);
    this->music2.setLooping(true);
    this->echo.setParams(0.2f, 0.5f, 0.05f);
    this->biquad.setParams(SoLoud::BiquadResonantFilter::LOWPASS, 44100, 4000, 2);
    this->soloud.setGlobalFilter(0, &this->biquad);
    this->soloud.setGlobalFilter(1, &this->bassBoost);
    this->soloud.setGlobalFilter(2, &this->echo);
    this->soloud.setGlobalFilter(3, &this->dcRemoval);
    this->soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF|SoLoud::Soloud::ENABLE_VISUALIZATION);

    IO::Load("snd:modulation.sid.dump", [this](IO::LoadResult res) {
        this->music1Data = std::move(res.Data);
        this->music1.loadMem(this->music1Data.Data(), this->music1Data.Size(), false, false);
        this->musicHandle1 = this->soloud.play(this->music1, this->music1Volume);
    });
    IO::Load("snd:ted_storm.prg.dump", [this](IO::LoadResult res) {
        this->music2Data = std::move(res.Data);
        this->music2.loadMem(this->music2Data.Data(), this->music2Data.Size(), false, false);
        this->musicHandle2 = this->soloud.play(this->music2, this->music2Volume);
    });
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SoloudTedSidApp::OnCleanup() {
    soloud.deinit();
    IMUI::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
