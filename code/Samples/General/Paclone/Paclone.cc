//------------------------------------------------------------------------------
//  Paclone.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "canvas.h"
#include "game.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;

class PacloneApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();

private:
    RenderFacade* render;
    Paclone::canvas canvas;
    Paclone::game game;
    int32 tick;
};
OryolMain(PacloneApp);

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnInit() {
    
    this->tick = 0;
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(736, 544, "Oryol Pacman Clone Sample"));
    this->canvas.Setup(23, 17);
    this->game.Init(&this->canvas);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnRunning() {
    if (this->render->BeginFrame()) {

        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::All, glm::vec4(0.0f), 1.0f, 0);
        this->game.Update(this->tick, &this->canvas);
        this->canvas.Render();
        
        this->render->EndFrame();
        this->tick++;
    }

    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnCleanup() {
    this->game.Cleanup();
    this->canvas.Discard();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
