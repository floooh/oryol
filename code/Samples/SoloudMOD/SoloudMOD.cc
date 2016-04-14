//------------------------------------------------------------------------------
//  SoloudMOD.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/Containers/StaticArray.h"
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "IMUI/IMUI.h"
#include "soloud.h"
#include "soloud_modplug.h"

using namespace Oryol;

class SoloudMODApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();

private:
    SoLoud::Soloud soloud;
    static const int NumMods = 7;
    struct Mod {
        Mod() {};
        Mod(const char* name_, const char* path_) : path(path_), name(name_) { };

        SoLoud::Modplug mod;
        StringAtom path;
        StringAtom name;
        int handle = 0;
        float volume = 1.5f;
        bool valid = false;
        bool failed = false;
        Buffer data;
    };
    StaticArray<Mod,NumMods> mods;
};
OryolMain(SoloudMODApp);

//------------------------------------------------------------------------------
AppState::Code
SoloudMODApp::OnRunning() {

    Gfx::ApplyDefaultRenderTarget(ClearState::ClearColor(glm::vec4(0.2f, 0.4f, 0.8f, 1.0f)));
    IMUI::NewFrame();

    float* buf = this->soloud.getWave();
    float* fft = this->soloud.calcFFT();
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_Once);
    ImGui::Begin("MOD Player (MODs from SoLoud and modarchive.org)", nullptr, ImVec2(660, 400));
    for (int i = 0; i < NumMods; i++) {
        ImGui::PushID(i);
        auto& mod = this->mods[i];
        ImGui::Text("Song %d:", i); ImGui::SameLine();
        if (mod.valid) {
            if (0 == mod.handle) {
                if (ImGui::Button("Play!")) {
                    mod.handle = soloud.play(mod.mod, mod.volume);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", mod.name.AsCStr());
                }
            }
            else {
                if (ImGui::Button("Stop!")) {
                    soloud.stop(mod.handle);
                    mod.handle = 0;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", mod.name.AsCStr());
                }
                ImGui::SameLine();
                if (ImGui::SliderFloat("Volume", &mod.volume, 0.0f, 1.5f)) {
                    this->soloud.setVolume(mod.handle, mod.volume);
                }
            }
        }
        else if (mod.failed) {
            ImGui::TextColored(ImVec4(1,0,0,1), "Failed loading '%s'!", mod.path.AsCStr());
        }
        else {
            ImGui::Text("Loading '%s'...", mod.path.AsCStr());
        }
        ImGui::PopID();
    }

    ImGui::PlotLines("Wave", buf, 256, 0, nullptr, -1, 1, ImVec2(560, 80));
    ImGui::PlotHistogram("FFT", fft, 256/2, 0, nullptr, 0, 10, ImVec2(560, 80), 8);
    ImGui::End();

    ImGui::Render();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SoloudMODApp::OnInit() {

    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    #if ORYOL_DEBUG
    ioSetup.Assigns.Add("snd:", "http://localhost:8000/");
    #else
    ioSetup.Assigns.Add("snd:", ORYOL_SAMPLE_URL);
    #endif
    IO::Setup(ioSetup);

    Gfx::Setup(GfxSetup::Window(800, 500, "SoLoud MOD Demo"));
    Input::Setup();
    IMUI::Setup();

    // NOTE: filenames have .txt extension so that github pages will compress them
    this->mods[0] = Mod("Combat Signal by ???", "snd:comsi.s3m.txt");
    this->mods[1] = Mod("Mr Bruce gets stoned by ???", "snd:bruce.s3m.txt");
    this->mods[2] = Mod("Jungle Juice by disaster from !bass records!", "snd:jungle_juice.mod.txt");
    this->mods[3] = Mod("Disco Feva Baby! by Necros/PM/EGG", "snd:disco_feva_baby.s3m.txt");
    this->mods[4] = Mod("digital innovation2 by nuke of anarchy", "snd:dgtinnv1.mod.txt");
    this->mods[5] = Mod("sunshine by fender", "snd:sunshine.xm.txt");
    this->mods[6] = Mod("Industrial Porn by ...::DROZERiX::...", "snd:porn_industryy.xm.txt");
    this->soloud.init();
    this->soloud.setVisualizationEnable(true);

    // asynchronously load mods (don't start them yet)
    for (int i = 0; i < NumMods; i++) {
        IO::Load(this->mods[i].path,
            // successfully loaded
            [this, i](IO::LoadResult loadResult) {
                auto& mod = this->mods[i];
                mod.data = std::move(loadResult.Data);
                SoLoud::result loaded = mod.mod.loadMem(mod.data.Data(), mod.data.Size(), false, false);
                if (SoLoud::SO_NO_ERROR == loaded) {
                    mod.valid = true;
                    if (0 == i) {
                        mod.handle = this->soloud.play(mod.mod, mod.volume);
                    }
                }
                else {
                    mod.failed = true;
                }
            },
            // failed to load
            [this, i](const URL& url, IOStatus::Code ioStatus) {
                this->mods[i].failed = true;
            });
    }
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SoloudMODApp::OnCleanup() {
    soloud.deinit();
    IMUI::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
