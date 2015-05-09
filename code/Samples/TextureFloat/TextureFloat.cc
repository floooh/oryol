//------------------------------------------------------------------------------
//  TextureFloat.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class TextureFloatApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    Id renderTarget;
    Id offscreenDrawState;
    Id copyDrawState;
    
    glm::mat4 view;
    glm::mat4 proj;
    float32 time = 0.0f;
    TimePoint lastFrameTimePoint;
};
OryolMain(TextureFloatApp);

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnRunning() {
    
    this->time += 1.0f / 60.0f;
    
    // render plasma to offscreen render target
    Gfx::ApplyOffscreenRenderTarget(this->renderTarget);
    Gfx::ApplyDrawState(this->offscreenDrawState);
    Gfx::ApplyVariable(Shaders::Offscreen::Time, this->time);
    Gfx::Draw(0);
    
    // copy fullscreen quad
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->copyDrawState);
    Gfx::ApplyVariable(Shaders::Copy::Texture, this->renderTarget);
    Gfx::Draw(0);
    
    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("%.3fms", frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxSetup::Window(512, 512, "Oryol Float Texture Sample"));
    Dbg::Setup();

    // check required extensions
    if (!Gfx::Supports(GfxFeature::TextureFloat)) {
        o_error("ERROR: float_texture extension required!\n");
    }
    
    // create an offscreen float render target, same size as display,
    // configure texture sampler with point-filtering
    auto rtSetup = TextureSetup::RelSizeRenderTarget(1.0f, 1.0f);
    rtSetup.ColorFormat = PixelFormat::RGBA32F;
    rtSetup.MagFilter = TextureFilterMode::Nearest;
    rtSetup.MinFilter = TextureFilterMode::Nearest;
    this->renderTarget = Gfx::CreateResource(rtSetup);
    
    // fullscreen mesh, we'll reuse this several times
    Id fullscreenMesh = Gfx::CreateResource(MeshSetup::FullScreenQuad());

    // setup draw state for offscreen rendering to float render target
    Id offscreenProg = Gfx::CreateResource(Shaders::Offscreen::CreateSetup());
    this->offscreenDrawState = Gfx::CreateResource(DrawStateSetup::FromMeshAndProg(fullscreenMesh, offscreenProg));
    
    // fullscreen-copy mesh, shader and draw state
    Id copyProg = Gfx::CreateResource(Shaders::Copy::CreateSetup());
    this->copyDrawState = Gfx::CreateResource(DrawStateSetup::FromMeshAndProg(fullscreenMesh, copyProg));
    
    // setup static transform matrices
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 5.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
