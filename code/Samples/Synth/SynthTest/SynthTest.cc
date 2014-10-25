//------------------------------------------------------------------------------
//  SynthTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
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
    static const int NumTracks = 4;
    SynthOp op;
    const char* modType = "Silence";
};
OryolMain(SynthTestApp);

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(640, 400, "Oryol Synth Test Sample"));
    Dbg::Setup();
    Input::Setup();
    SynthSetup synthSetup;
    synthSetup.UseGPUSynthesizer = false;
    Synth::Setup(synthSetup);
    
    this->op.Code = SynthOp::Square;
    this->op.Frequency = 660;
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnRunning() {

    const Keyboard& kbd = Input::Keyboard();
    if (kbd.KeyDown(Key::Up)) {
        this->op.Frequency += 11;
        if (this->op.Frequency > 4400) {
            this->op.Frequency = 4400;
        }
        Synth::AddOp(0, 0, this->op);
    }
    if (kbd.KeyDown(Key::Down)) {
        this->op.Frequency -= 11;
        if (this->op.Frequency < 11) {
            this->op.Frequency = 11;
        }
        Synth::AddOp(0, 0, this->op);
    }
    if (kbd.KeyDown(Key::T)) {
        this->op.Code = SynthOp::Triangle;
        Synth::AddOp(0, 0, this->op);
    }
    else if (kbd.KeyDown(Key::Q)) {
        this->op.Code = SynthOp::Square;
        Synth::AddOp(0, 0, this->op);
    }
    else if (kbd.KeyDown(Key::S)) {
        this->op.Code = SynthOp::Sine;
        Synth::AddOp(0, 0, this->op);
    }
    else if (kbd.KeyDown(Key::N)) {
        this->op.Code = SynthOp::Noise;
        Synth::AddOp(0, 0, this->op);
    }
    
    // modulation
    if (kbd.KeyDown(Key::N1)) {
        this->modType = "Silence";
        SynthOp mod;
        mod.Code = SynthOp::Const;
        mod.Amp = 0;
        Synth::AddOp(0, 1, mod);
        Synth::AddOp(0, 0, this->op);
    }
    if (kbd.KeyDown(Key::N2)) {
        this->modType = "Constant One";
        SynthOp mod;
        mod.Code = SynthOp::Const;
        Synth::AddOp(0, 1, mod);
        Synth::AddOp(0, 0, this->op);
    }
    if (kbd.KeyDown(Key::N3)) {
        this->modType = "LowFreq Sine";
        SynthOp mod;
        mod.Code = SynthOp::Sine;
        mod.Frequency = 5;
        Synth::AddOp(0, 1, mod);
        Synth::AddOp(0, 0, this->op);
    }
    if (kbd.KeyDown(Key::N4)) {
        this->modType = "HiFreq Sawtooth";
        SynthOp mod;
        mod.Code = SynthOp::Triangle;
        mod.Frequency = 55;
        mod.Amp = 0.5f;
        mod.Bias = 0.5f;
        Synth::AddOp(0, 1, mod);
        Synth::AddOp(0, 0, this->op);
    }
    /*
    if (kbd.KeyDown(Key::N5)) {
        // ASDR modulation
        this->modType = "ADSR";
        SynthOp attack;
        attack.Code = SynthOp::Const;
        attack.FadeIn = 200;
        attack.Amp = Synth::Amp(1.0f);
        Synth::AddOp(0, 1, attack);
        SynthOp decay;
        decay.Code = SynthOp::Const;
        decay.FadeIn = 4000;
        decay.Amp = Synth::Amp(0.2f);
        Synth::AddOp(0, 1, decay, 0.01f);
        SynthOp release;
        release.Code = SynthOp::Const;
        release.FadeIn = 20000;
        release.Amp = 0;
        Synth::AddOp(0, 1, release, 0.3f);
        Synth::AddOp(0, 0, this->op);
    }
    */

    Synth::Update();
    Dbg::Print("\n\n");
    Dbg::PrintF(" Waveform (T,S,Q,N): %s\n\r", SynthOp::ToString(this->op.Code));
    Dbg::PrintF(" Freq (up/down): %d\n\r", this->op.Frequency);
    Dbg::PrintF(" Modulation (1,2,3,4,5): %s\n\r", this->modType);
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::RGBA, glm::vec4(0.5f), 1.0f, 0);
    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();
    this->frameCount++;
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnCleanup() {
    Synth::Discard();
    Dbg::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
