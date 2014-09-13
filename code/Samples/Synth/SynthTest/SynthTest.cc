//------------------------------------------------------------------------------
//  SynthTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Debug/DebugFacade.h"
#include "Input/InputFacade.h"
#include "Synth/SynthFacade.h"

using namespace Oryol;
using namespace Oryol::Input;
using namespace Oryol::Synth;

// derived application class
class SynthTestApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    DebugFacade* debug;
    InputFacade* input;
    SynthFacade* synth;
    int32 frameCount = 0;
    static const int NumTracks = 4;
    Op op;
    const char* modType = "Silence";
};
OryolMain(SynthTestApp);

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::AsWindow(640, 400, false, "Oryol Synth Test Sample"));
    this->debug  = DebugFacade::CreateSingle();
    this->input  = InputFacade::CreateSingle();
    SynthSetup synthSetup;
    synthSetup.UseGPUSynthesizer = false;
    this->synth = SynthFacade::CreateSingle(synthSetup);
    
    this->op.Code = Op::Square;
    this->op.FadeIn = 0.05f;
    this->op.Frequency = 660;
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnRunning() {

    if (this->render->BeginFrame()) {
        
        const Keyboard& kbd = this->input->Keyboard();
        if (kbd.KeyDown(Key::Left)) {
            this->op.Pulse -= 0.05f;
            if (this->op.Pulse < 0.0f) {
                this->op.Pulse = 0.0f;
            }
            this->synth->AddOp(0, 0, this->op);
        }
        if (kbd.KeyDown(Key::Right)) {
            this->op.Pulse += 0.05f;
            if (this->op.Pulse > 1.0f) {
                this->op.Pulse = 1.0f;
            }
            this->synth->AddOp(0, 0, this->op);
        }
        if (kbd.KeyDown(Key::Up)) {
            this->op.Frequency += 110;
            if (this->op.Frequency > 4400) {
                this->op.Frequency = 4400;
            }
            this->synth->AddOp(0, 0, this->op);
        }
        if (kbd.KeyDown(Key::Down)) {
            this->op.Frequency -= 110;
            if (this->op.Frequency < 110) {
                this->op.Frequency = 110;
            }
            this->synth->AddOp(0, 0, this->op);
        }
        if (kbd.KeyDown(Key::T)) {
            this->op.Code = Op::Triangle;
            this->synth->AddOp(0, 0, this->op);
        }
        else if (kbd.KeyDown(Key::Q)) {
            this->op.Code = Op::Square;
            this->synth->AddOp(0, 0, this->op);
        }
        else if (kbd.KeyDown(Key::S)) {
            this->op.Code = Op::Sine;
            this->synth->AddOp(0, 0, this->op);
        }
        else if (kbd.KeyDown(Key::N)) {
            this->op.Code = Op::Noise;
            this->synth->AddOp(0, 0, this->op);
        }
        
        // modulation
        if (kbd.KeyDown(Key::N1)) {
            this->modType = "Silence";
            Op mod;
            mod.Code = Op::Const;
            mod.FadeIn = 0.1f;
            mod.Amplitude = 0.0f;
            this->synth->AddOp(0, 1, mod);
            this->synth->AddOp(0, 0, this->op);
        }
        if (kbd.KeyDown(Key::N2)) {
            this->modType = "Constant One";
            Op mod;
            mod.Code = Op::Const;
            mod.FadeIn = 0.1f;
            this->synth->AddOp(0, 1, mod);
            this->synth->AddOp(0, 0, this->op);
        }
        if (kbd.KeyDown(Key::N3)) {
            this->modType = "LowFreq Sine";
            Op mod;
            mod.Code = Op::Sine;
            mod.FadeIn = 0.1f;
            mod.Frequency = 5.0f;
            mod.Amplitude = 0.5f;
            mod.Bias = 0.5f;
            this->synth->AddOp(0, 1, mod);
            this->synth->AddOp(0, 0, this->op);
        }
        if (kbd.KeyDown(Key::N4)) {
            this->modType = "HiFreq Sawtooth";
            Op mod;
            mod.Code = Op::Triangle;
            mod.FadeIn = 0.1f;
            mod.Frequency = 55.0f;
            mod.Pulse = 0.0f;
            mod.Amplitude = 0.5f;
            mod.Bias = 0.5f;
            this->synth->AddOp(0, 1, mod);
            this->synth->AddOp(0, 0, this->op);
        }
        if (kbd.KeyDown(Key::N5)) {
            // ASDR modulation
            this->modType = "ADSR";
            Op attack;
            attack.Code = Op::Const;
            attack.FadeIn = 0.01f;
            attack.Amplitude = 1.0f;
            this->synth->AddOp(0, 1, attack);
            Op decay;
            decay.Code = Op::Const;
            decay.FadeIn = 0.2f;
            decay.Amplitude = 0.2f;
            this->synth->AddOp(0, 1, decay, 0.01f);
            Op release;
            release.Code = Op::Const;
            release.FadeIn = 0.8f;
            release.Amplitude = 0.0f;
            this->synth->AddOp(0, 1, release, 0.3f);
            this->synth->AddOp(0, 0, this->op);
        }
    
        this->synth->Update();
        this->debug->Print("\n\n");
        this->debug->PrintF(" Waveform (T,S,Q,N): %s\n\r", Op::ToString(this->op.Code));
        this->debug->PrintF(" Freq (up/down): %.2f\n\r", this->op.Frequency);
        this->debug->PrintF(" Pulse (left/right): %.2f\n\r", this->op.Pulse);
        this->debug->PrintF(" Modulation (1,2,3,4,5): %s\n\r", this->modType);
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(PixelChannel::RGBA, glm::vec4(0.5f), 1.0f, 0);
        this->debug->DrawTextBuffer();
        this->render->EndFrame();
        this->frameCount++;
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnCleanup() {
    this->synth  = nullptr;
    this->debug  = nullptr;
    this->input  = nullptr;
    this->render = nullptr;
    SynthFacade::DestroySingle();
    DebugFacade::DestroySingle();
    InputFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
