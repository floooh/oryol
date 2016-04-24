//------------------------------------------------------------------------------
//  Paclone.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "Dbg/Dbg.h"
#include "Sound/Sound.h"
#include "canvas.h"
#include "game.h"
#include "shaders.h"
#include "glm/common.hpp"

using namespace Oryol;
using namespace Paclone;

#if ORYOL_RASPBERRYPI || ORYOL_IOS || ORYOL_ANDROID
#define USE_CRTEFFECT (0)
#else
#define USE_CRTEFFECT (1)
#endif

class PacloneApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

private:
    Direction getInput();
    void applyViewPort();

    Id crtRenderTarget;
    DrawState crtDrawState;
    #if USE_CRTEFFECT
    CRTShader::FSParams crtParams;
    #else
    NoCRTShader::FSParams crtParams;
    #endif

    canvas spriteCanvas;
    game gameState;
    sound sounds;
    int tick = 0;
    int viewPortX = 0;
    int viewPortY = 0;
    int viewPortW = 0;
    int viewPortH = 0;
    Direction input = NoDirection;
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

    auto quadSetup = MeshSetup::FullScreenQuad(Gfx::QueryFeature(GfxFeature::OriginTopLeft));
    this->crtDrawState.Mesh[0] = Gfx::CreateResource(quadSetup);
    #if USE_CRTEFFECT
    Id shd = Gfx::CreateResource(CRTShader::Setup());
    #else
    Id shd = Gfx::CreateResource(NoCRTShader::Setup());
    #endif
    auto ps = PipelineSetup::FromLayoutAndShader(quadSetup.Layout, shd);
    this->crtDrawState.Pipeline = Gfx::CreateResource(ps);
    this->crtDrawState.FSTexture[CRTTextures::Canvas] = this->crtRenderTarget;

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
    this->viewPortY = 0;
    this->viewPortH = fbHeight;
    this->viewPortW = (const int) (fbHeight * aspect);
    this->viewPortX = (fbWidth - viewPortW) / 2;
    Gfx::ApplyViewPort(this->viewPortX, this->viewPortY, this->viewPortW, this->viewPortH);
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
    Gfx::ApplyDrawState(this->crtDrawState);
    Gfx::ApplyUniformBlock(this->crtParams);
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
    const Keyboard& kbd = Input::Keyboard();
    if (kbd.Attached) {
        if (kbd.KeyPressed(Key::Left))       this->input = Left;
        else if (kbd.KeyPressed(Key::Right)) this->input = Right;
        else if (kbd.KeyPressed(Key::Up))    this->input = Up;
        else if (kbd.KeyPressed(Key::Down))  this->input = Down;
    }
    const Touchpad& tpad = Input::Touchpad();
    const Mouse& mouse = Input::Mouse();
    if (tpad.Attached || mouse.Attached) {
        if (tpad.Tapped || tpad.Panning || mouse.ButtonPressed(Mouse::Button::LMB)) {
            float x, y;
            if (tpad.Tapped || tpad.Panning) {
                x = tpad.Position[0].x;
                y = tpad.Position[0].y;
            }
            else {
                x = mouse.Position.x;
                y = mouse.Position.y;
            }
            const Int2 pacmanPos = this->gameState.PacmanPos();
            const float relX = float(pacmanPos.x) / float(this->spriteCanvas.CanvasWidth());
            const float relY = float(pacmanPos.y) / float(this->spriteCanvas.CanvasHeight());
            const float px = this->viewPortX + this->viewPortW * relX;
            const float py = this->viewPortY + this->viewPortH * relY;
            const float dx = x - px;
            const float dy = y - py;
            if (glm::abs(dx) > glm::abs(dy)) {
                // it's a horizontal movement
                this->input = dx < 0.0f ? Left : Right;
            }
            else {
                // it's a vertical movement
                this->input = dy < 0.0f ? Up : Down;
            }
        }
    }
    return this->input;
}
