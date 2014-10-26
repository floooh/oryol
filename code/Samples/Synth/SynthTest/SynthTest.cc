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
    Dbg::Setup();
    Input::Setup();
    SynthSetup synthSetup;
    synthSetup.UseGPUSynthesizer = false;
    Synth::Setup(synthSetup);
    
    this->freqOp.Op = SynthOp::Replace;
    this->freqOp.Wave = SynthOp::Custom0;
    this->freqOp.Freq = 5;
    this->freqOp.Amp = ToAmp(0.5f);
    this->freqOp.Bias = ToAmp(0.25f);
    
    this->sndOp.Op   = SynthOp::ModFreq;
    this->sndOp.Wave = SynthOp::Custom0;
    this->sndOp.Freq = 660;
    this->sndOp.Amp  = (1<<15);
    
    Synth::AddOp(0, 0, this->freqOp);
    Synth::AddOp(0, 1, this->sndOp);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnRunning() {

    const Keyboard& kbd = Input::Keyboard();
    if (kbd.KeyDown(Key::Escape)) {
        if (this->sndOp.Op == SynthOp::ModFreq) {
            this->sndOp.Op = SynthOp::Replace;
        }
        else {
            this->sndOp.Op = SynthOp::ModFreq;
        }
        Synth::AddOp(0, 1, this->sndOp);
    }
    
    if (kbd.KeyDown(Key::Up)) {
        if (kbd.KeyPressed(Key::LeftShift)) {
            this->freqOp.Freq += 2;
            Synth::AddOp(0, 0, this->freqOp);
        }
        else {
            this->sndOp.Freq += 11;
            if (this->sndOp.Freq > 4400) {
                this->sndOp.Freq = 4400;
            }
            Synth::AddOp(0, 1, this->sndOp);
        }
    }
    if (kbd.KeyDown(Key::Down)) {
        if (kbd.KeyPressed(Key::LeftShift)) {
            this->freqOp.Freq -= 1;
            if (this->freqOp.Freq < 1) {
                this->freqOp.Freq = 1;
            }
            Synth::AddOp(0, 0, this->freqOp);
        }
        else {
            this->sndOp.Freq -= 11;
            if (this->sndOp.Freq < 11) {
                this->sndOp.Freq = 11;
            }
            Synth::AddOp(0, 1, this->sndOp);
        }
    }
    
    for (int i = 0; i < 8; i++) {
        if (kbd.KeyDown(Key::Code(Key::N1 + i))) {
            if (kbd.KeyPressed(Key::LeftShift)) {
                this->freqOp.Wave = SynthOp::WaveT(SynthOp::Custom0 + i);
                Synth::AddOp(0, 0, this->freqOp);
            }
            else {
                this->sndOp.Wave = SynthOp::WaveT(SynthOp::Custom0 + i);
                Synth::AddOp(0, 1, this->sndOp);
            }
        }
    }
    
    // modulation
    /*
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
        mod.Freq = 5;
        Synth::AddOp(0, 1, mod);
        Synth::AddOp(0, 0, this->op);
    }
    if (kbd.KeyDown(Key::N4)) {
        this->modType = "HiFreq Sawtooth";
        SynthOp mod;
        mod.Code = SynthOp::Triangle;
        mod.Freq = 55;
        mod.Amp = 0.5f;
        mod.Bias = 0.5f;
        Synth::AddOp(0, 1, mod);
        Synth::AddOp(0, 0, this->op);
    }
    */
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
    Dbg::PrintF(" Frequency Modulation (Esc): %s\n\r", this->sndOp.Op == SynthOp::ModFreq ? "on" : "off");
    Dbg::PrintF(" Sound Wave (1..8): %s\n\r", SynthOp::ToString(this->sndOp.Wave));
    Dbg::PrintF(" Sound Freq (up/down): %d\n\r", this->sndOp.Freq);
    Dbg::PrintF(" Freq Mod Wave (Shift 1..8): %s\n\r", SynthOp::ToString(this->freqOp.Wave));
    Dbg::PrintF(" Freq Mod Freq (Shift up/down): %d\n\r", this->freqOp.Freq);
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
