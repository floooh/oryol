//------------------------------------------------------------------------------
//  TextureFloat.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Core/Time/Clock.h"
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
    DrawState offscreenDrawState;
    DrawState copyDrawState;
    glm::mat4 view;
    glm::mat4 proj;
    OffscreenShader::FSParams offscreenFSParams;
    TimePoint lastFrameTimePoint;
    ClearState noClearState = ClearState::ClearNone();
};
OryolMain(TextureFloatApp);

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnRunning() {

    this->offscreenFSParams.Time += 1.0f / 60.0f;
    
    // render plasma to offscreen render target, do not clear
    Gfx::ApplyRenderTarget(this->renderTarget, this->noClearState);
    Gfx::ApplyDrawState(this->offscreenDrawState);
    Gfx::ApplyUniformBlock(this->offscreenFSParams);
    Gfx::Draw();
    
    // copy fullscreen quad
    Gfx::ApplyDefaultRenderTarget(this->noClearState);
    Gfx::ApplyDrawState(this->copyDrawState);
    Gfx::Draw();

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
    if (!Gfx::QueryFeature(GfxFeature::TextureFloat)) {
        o_error("ERROR: float_texture extension required!\n");
    }
    
    // create an offscreen float render target, same size as display,
    // configure texture sampler with point-filtering
    auto rtSetup = TextureSetup::RelSizeRenderTarget(1.0f, 1.0f);
    rtSetup.ColorFormat = PixelFormat::RGBA32F;
    rtSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
    rtSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
    this->renderTarget = Gfx::CreateResource(rtSetup);

    // fullscreen mesh, we'll reuse this several times
    auto quadSetup = MeshSetup::FullScreenQuad();
    Id quadMesh = Gfx::CreateResource(quadSetup);
    this->offscreenDrawState.Mesh[0] = quadMesh;
    this->copyDrawState.Mesh[0] = quadMesh;

    // setup draw state for offscreen rendering to float render target
    Id offscreenShader = Gfx::CreateResource(OffscreenShader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(quadSetup.Layout, offscreenShader);
    ps.BlendState.ColorFormat = rtSetup.ColorFormat;
    ps.BlendState.DepthFormat = rtSetup.DepthFormat;
    this->offscreenDrawState.Pipeline = Gfx::CreateResource(ps);
    this->offscreenFSParams.Time = 0.0f;

    // fullscreen-copy resources
    Id copyShader = Gfx::CreateResource(CopyShader::Setup());
    ps = PipelineSetup::FromLayoutAndShader(quadSetup.Layout, copyShader);
    this->copyDrawState.Pipeline = Gfx::CreateResource(ps);
    this->copyDrawState.FSTexture[Textures::Texture] = this->renderTarget;

    // setup static transform matrices
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
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
