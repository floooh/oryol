//------------------------------------------------------------------------------
//  Paclone.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "canvas.h"

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
};
OryolMain(PacloneApp);

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnInit() {

    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(736, 544, "Oryol Pacman Clone Sample"));
    this->canvas.Setup(23, 17);
    
    // FIXME: just some testing
    this->canvas.FillRect(0, 0, 23, 16, Paclone::Sheet::Pill);

    this->canvas.HoriWall(0, 0, 22);
    this->canvas.HoriWall(0, 16, 22);
    this->canvas.VertWall(0, 0, 16);
    this->canvas.VertWall(22, 0, 16);
    
    this->canvas.RectWall(2, 2, 2, 1);
    this->canvas.RectWall(6, 2, 3, 1);
    
    this->canvas.VertWall(11, 0, 3);

    this->canvas.RectWall(13, 2, 3, 1);
    this->canvas.RectWall(18, 2, 2, 1);
    
    this->canvas.Set(10, 5, Paclone::Sheet::Blinky);
    this->canvas.Set(12, 5, Paclone::Sheet::Pinky);
    this->canvas.Set(10, 7, Paclone::Sheet::Clyde);
    this->canvas.Set(12, 7, Paclone::Sheet::Inky);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnRunning() {
    if (this->render->BeginFrame()) {

        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::All, glm::vec4(0.0f), 1.0f, 0);
        this->canvas.Update();
        this->canvas.Render();

        this->render->EndFrame();
    }

    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnCleanup() {
    this->canvas.Discard();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
