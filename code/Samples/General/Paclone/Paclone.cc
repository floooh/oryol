//------------------------------------------------------------------------------
//  Paclone.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "canvas.h"
#include "game.h"

using namespace Oryol;
using namespace Paclone;

class PacloneApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

private:
    game::Direction getInput();

    canvas spriteCanvas;
    game gameState;
    int32 tick;
    int32 viewPortX;
    int32 viewPortY;
    int32 viewPortW;
    int32 viewPortH;
};
OryolMain(PacloneApp);

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnInit() {
    
    this->tick = 0;
    const int dispWidth = game::Width * 8 * 2;
    const int dispHeight = game::Height * 8 * 2;
    Gfx::Setup(GfxSetup::Window(dispWidth, dispHeight, "Oryol Pacman Clone Sample"));
    Input::Setup();
    
    this->spriteCanvas.Setup(game::Width, game::Height, 8, 8, game::NumSprites);
    this->gameState.Init(&this->spriteCanvas);

    // get actual display width and height and compute viewport
    float aspect = float(dispWidth) / float(dispHeight);
    const int actWidth = Gfx::DisplayAttrs().WindowWidth;
    const int actHeight = Gfx::DisplayAttrs().WindowHeight;
    this->viewPortY = 0;
    this->viewPortH = actHeight;
    this->viewPortW = actHeight * aspect;
    this->viewPortX = (actWidth - this->viewPortW) / 2;
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnRunning() {

    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyViewPort(this->viewPortX, this->viewPortY, this->viewPortW, this->viewPortH);
    Gfx::Clear(PixelChannel::All, glm::vec4(0.0f), 1.0f, 0);
    game::Direction input = this->getInput();
    this->gameState.Update(this->tick, &this->spriteCanvas, input);
    this->spriteCanvas.Render();
    
    Gfx::CommitFrame();
    this->tick++;

    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnCleanup() {
    this->gameState.Cleanup();
    this->spriteCanvas.Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
game::Direction
PacloneApp::getInput() {
    // FIXME: add more input options
    game::Direction input = game::NoDirection;
    const Keyboard& kbd = Input::Keyboard();
    if (kbd.Attached) {
        if (kbd.KeyPressed(Key::Left))       input = game::Left;
        else if (kbd.KeyPressed(Key::Right)) input = game::Right;
        else if (kbd.KeyPressed(Key::Up))    input = game::Up;
        else if (kbd.KeyPressed(Key::Down))  input = game::Down;
    }
    return input;
}
