//------------------------------------------------------------------------------
//  Paclone.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "Dbg/Dbg.h"
#include "canvas.h"
#include "game.h"
#include "shaders/shaders.h"

using namespace Oryol;
using namespace Paclone;

class PacloneApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

private:
    Direction getInput();
    void applyViewPort();

    GfxId canvasRenderTarget;
    GfxId crtEffect;
    canvas spriteCanvas;
    game gameState;
    int32 tick;
};
OryolMain(PacloneApp);

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnInit() {
    
    this->tick = 0;
    const int canvasWidth = Width * 8;
    const int canvasHeight = Height * 8;
    const int dispWidth = canvasWidth * 2;
    const int dispHeight = canvasHeight * 2;
    Gfx::Setup(GfxSetup::Window(dispWidth, dispHeight, "Oryol Pacman Clone Sample"));
    Input::Setup();
    Dbg::Setup();
    
    // setup canvas and game state
    this->spriteCanvas.Setup(Width, Height, 8, 8, NumSprites);
    this->gameState.Init(&this->spriteCanvas);
    
    // setup a offscreen render target and copy-shader
    auto rtSetup = TextureSetup::RenderTarget(canvasWidth, canvasHeight);
    rtSetup.MinFilter = TextureFilterMode::Linear;
    rtSetup.MagFilter = TextureFilterMode::Linear;
    this->canvasRenderTarget = Gfx::CreateResource(rtSetup);
    GfxId mesh = Gfx::CreateResource(MeshSetup::FullScreenQuad());
    GfxId prog = Gfx::CreateResource(Shaders::CRT::CreateSetup());
    this->crtEffect = Gfx::CreateResource(DrawStateSetup::FromMeshAndProg(mesh, prog));

    return App::OnInit();
}

//------------------------------------------------------------------------------
void
PacloneApp::applyViewPort() {
    float aspect = float(Width) / float(Height);
    const int fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    const int fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    const int viewPortY = 0;
    const int viewPortH = fbHeight;
    const int viewPortW = fbHeight * aspect;
    const int viewPortX = (fbWidth - viewPortW) / 2;
    Gfx::ApplyViewPort(viewPortX, viewPortY, viewPortW, viewPortH);
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnRunning() {

    Direction input = this->getInput();
    this->gameState.Update(this->tick, &this->spriteCanvas, input);
    
    // render into offscreen render target
    Gfx::ApplyOffscreenRenderTarget(this->canvasRenderTarget);
    Gfx::Clear(PixelChannel::RGBA, glm::vec4(0.0f));
    this->spriteCanvas.Render();
    Dbg::DrawTextBuffer();
    
    // copy offscreen render target into backbuffer
    glm::vec2 dispRes(Gfx::DisplayAttrs().FramebufferWidth, Gfx::DisplayAttrs().FramebufferHeight);
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::RGBA, glm::vec4(0.0f));
    this->applyViewPort();
    Gfx::ApplyDrawState(this->crtEffect);
    Gfx::ApplyVariable(Shaders::CRT::Canvas, this->canvasRenderTarget);
    Gfx::ApplyVariable(Shaders::CRT::Resolution, dispRes);
    Gfx::Draw(0);
    Gfx::CommitFrame();
    this->tick++;

    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnCleanup() {
    this->canvasRenderTarget.Release();
    this->crtEffect.Release();
    this->gameState.Cleanup();
    this->spriteCanvas.Discard();
    Dbg::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
Direction
PacloneApp::getInput() {
    // FIXME: add more input options
    Direction input = NoDirection;
    const Keyboard& kbd = Input::Keyboard();
    if (kbd.Attached) {
        if (kbd.KeyPressed(Key::Left))       input = Left;
        else if (kbd.KeyPressed(Key::Right)) input = Right;
        else if (kbd.KeyPressed(Key::Up))    input = Up;
        else if (kbd.KeyPressed(Key::Down))  input = Down;
    }
    return input;
}
