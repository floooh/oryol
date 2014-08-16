//------------------------------------------------------------------------------
//  Paclone.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Input/InputFacade.h"
#include "canvas.h"
#include "game.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Input;
using namespace Paclone;

class PacloneApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();

private:
    game::Direction getInput();

    RenderFacade* render;
    InputFacade* input;
    canvas spriteCanvas;
    game gameState;
    int32 tick;
};
OryolMain(PacloneApp);

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnInit() {
    
    this->tick = 0;
    const int displayWidth = game::Width * 8 * 2;
    const int displayHeight = game::Height * 8 * 2;
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(displayWidth, displayHeight, "Oryol Pacman Clone Sample"));
    this->input = InputFacade::CreateSingle();
    
    this->spriteCanvas.Setup(game::Width, game::Height, 8, 8, game::NumSprites);
    this->gameState.Init(&this->spriteCanvas);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnRunning() {
    if (this->render->BeginFrame()) {

        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::All, glm::vec4(0.0f), 1.0f, 0);
        game::Direction input = this->getInput();
        this->gameState.Update(this->tick, &this->spriteCanvas, input);
        this->spriteCanvas.Render();
        
        this->render->EndFrame();
        this->tick++;
    }

    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnCleanup() {
    this->gameState.Cleanup();
    this->spriteCanvas.Discard();
    InputFacade::DestroySingle();
    RenderFacade::DestroySingle();
    this->input = 0;
    this->render = 0;
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
game::Direction
PacloneApp::getInput() {
    // FIXME: add more input options
    game::Direction input = game::NoDirection;
    const Keyboard& kbd = this->input->Keyboard();
    if (kbd.Attached()) {
        if (kbd.KeyPressed(Key::Left))       input = game::Left;
        else if (kbd.KeyPressed(Key::Right)) input = game::Right;
        else if (kbd.KeyPressed(Key::Up))    input = game::Up;
        else if (kbd.KeyPressed(Key::Down))  input = game::Down;
    }
    return input;
}
