//------------------------------------------------------------------------------
//  Paclone.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "Dbg/Dbg.h"
#include "Sound/Sound.h"
#include "canvas.h"
#include "game.h"
#include "shaders.h"

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

    Id crtEffect;
    Id crtRenderTarget;
    Id crtTexBlock;
    Shaders::CRT::FSParams crtParams;

    canvas spriteCanvas;
    game gameState;
    sound sounds;
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
    Sound::Setup(SoundSetup());
    Dbg::Setup();
    
    // setup a offscreen render target, copy-shader and texture block
    auto rtSetup = TextureSetup::RenderTarget(canvasWidth, canvasHeight);
    rtSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    rtSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    this->crtRenderTarget = Gfx::CreateResource(rtSetup);
    Id mesh = Gfx::CreateResource(MeshSetup::FullScreenQuad(Gfx::QueryFeature(GfxFeature::OriginTopLeft)));
    Id shd = Gfx::CreateResource(Shaders::CRT::Setup());
    this->crtEffect = Gfx::CreateResource(DrawStateSetup::FromMeshAndShader(mesh, shd));
    auto tbSetup = Shaders::CRT::FSTextures::Setup(shd);
    tbSetup.Slot[Shaders::CRT::FSTextures::Canvas] = this->crtRenderTarget;
    this->crtTexBlock = Gfx::CreateResource(tbSetup);

    // setup canvas and game state
    this->spriteCanvas.Setup(rtSetup, Width, Height, 8, 8, NumSprites);
    this->sounds.CreateSoundEffects();
    this->gameState.Init(&this->spriteCanvas, &this->sounds);

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
    const int viewPortW = (const int) (fbHeight * aspect);
    const int viewPortX = (fbWidth - viewPortW) / 2;
    Gfx::ApplyViewPort(viewPortX, viewPortY, viewPortW, viewPortH);
}

//------------------------------------------------------------------------------
AppState::Code
PacloneApp::OnRunning() {

    Direction input = this->getInput();
    this->gameState.Update(this->tick, &this->spriteCanvas, &this->sounds, input);

    // render into offscreen render target
    Gfx::ApplyRenderTarget(this->crtRenderTarget);
    this->spriteCanvas.Render();
    Dbg::DrawTextBuffer();
    
    // copy offscreen render target into backbuffer
    this->crtParams.Resolution = glm::vec2(Gfx::DisplayAttrs().FramebufferWidth, Gfx::DisplayAttrs().FramebufferHeight);
    Gfx::ApplyDefaultRenderTarget();
    this->applyViewPort();
    Gfx::ApplyDrawState(this->crtEffect);
    Gfx::ApplyUniformBlock(this->crtParams);
    Gfx::ApplyTextureBlock(this->crtTexBlock);
    Gfx::Draw(0);
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
    Dbg::Discard();
    Sound::Discard();
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
