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
using namespace Oryol::Core;
using namespace Oryol::Debug;
using namespace Oryol::Render;
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
    Sound sound;
    int32 frameCount = 0;
};
OryolMain(SynthTestApp);

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(640, 400, "Oryol Synth Test Sample"));
    this->debug  = DebugFacade::CreateSingle();
    this->input  = InputFacade::CreateSingle();
    this->synth  = SynthFacade::CreateSingle(SynthSetup());
    
    this->sound.Triangle = true;
    this->sound.Freq = 500;
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SynthTestApp::OnRunning() {

    if (this->render->BeginFrame()) {
        
        bool play = false;
        const Keyboard& kbd = this->input->Keyboard();
        if (kbd.KeyDown(Key::T)) {
            this->sound.Triangle = true;
            this->sound.Sawtooth = this->sound.Square = this->sound.Noise = false;
            play = true;
        }
        if (kbd.KeyDown(Key::S)) {
            this->sound.Sawtooth = true;
            this->sound.Triangle = this->sound.Square = this->sound.Noise = false;
            play = true;
        }
        if (kbd.KeyDown(Key::Q)) {
            this->sound.Square = true;
            this->sound.Triangle = this->sound.Sawtooth = this->sound.Noise = false;
            play = true;
        }
        if (kbd.KeyDown(Key::N)) {
            this->sound.Noise = true;
            this->sound.Triangle = this->sound.Sawtooth = this->sound.Square = false;
            play = true;
        }
        for (int i = 0; i < 10; i++) {
            if (kbd.KeyDown(Key::Code(Key::N1 + i))) {
                this->sound.Freq = 110 * (i + 1);
                play = true;
            }
        }
        if (play) {
            this->synth->Play(0, this->sound);
        }
    
        this->synth->Update();
        this->debug->Print("\n (T)riangle, S(Q)uare, (S)awtooth, (N)oise wave form\n\r");
        this->debug->Print("  Keys 1..9 for frequency\n");
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::RGBA, glm::vec4(0.5f), 1.0f, 0);
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
