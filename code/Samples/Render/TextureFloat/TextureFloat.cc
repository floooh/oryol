//------------------------------------------------------------------------------
//  TextureFloat.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/Render.h"
#include "Dbg/Dbg.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class TextureFloatApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
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
    // render one frame
    if (Render::BeginFrame()) {
        
        this->time += 1.0f / 60.0f;
        
        // render plasma to offscreen render target
        Render::ApplyOffscreenRenderTarget(this->renderTarget);
        Render::ApplyDrawState(this->offscreenDrawState);
        Render::ApplyVariable(Shaders::Offscreen::Time, this->time);
        Render::Draw(0);
        
        // copy fullscreen quad
        Render::ApplyDefaultRenderTarget();
        Render::ApplyDrawState(this->copyDrawState);
        Render::ApplyVariable(Shaders::Copy::Texture, this->renderTarget);
        Render::Draw(0);
        
        Dbg::DrawTextBuffer();
        Render::EndFrame();
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("%.3fms", frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnInit() {
    // setup rendering system
    auto renderSetup = RenderSetup::AsWindow(512, 512, false, "Oryol Float Texture Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    Render::Setup(renderSetup);
    Dbg::Setup();

    // check required extensions
    if (!Render::Supports(RenderFeature::TextureFloat)) {
        o_error("ERROR: float_texture extension required!\n");
    }
    
    // create an offscreen float render target, same size as display,
    // configure texture sampler with point-filtering
    auto rtSetup = TextureSetup::AsRelSizeRenderTarget("rt", 1.0f, 1.0f);
    rtSetup.ColorFormat = PixelFormat::RGBA32F;
    rtSetup.MagFilter = TextureFilterMode::Nearest;
    rtSetup.MinFilter = TextureFilterMode::Nearest;
    this->renderTarget = Render::CreateResource(rtSetup);
    
    // fullscreen mesh, we'll reuse this several times
    Id fullscreenMesh = Render::CreateResource(MeshSetup::CreateFullScreenQuad("fsMesh"));

    // setup draw state for offscreen rendering to float render target
    Id offscreenProg = Render::CreateResource(Shaders::Offscreen::CreateSetup());
    DrawStateSetup offscreenSetup("dsOffscreen", fullscreenMesh, offscreenProg, 0);
    this->offscreenDrawState = Render::CreateResource(offscreenSetup);
    Render::ReleaseResource(offscreenProg);
    
    // fullscreen-copy mesh, shader and draw state
    Id copyProg = Render::CreateResource(Shaders::Copy::CreateSetup());
    this->copyDrawState = Render::CreateResource(DrawStateSetup("dsCopy", fullscreenMesh, copyProg, 0));
    Render::ReleaseResource(copyProg);
    Render::ReleaseResource(fullscreenMesh);
    
    // setup static transform matrices
    const float32 fbWidth = Render::GetDisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Render::GetDisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 5.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnCleanup() {
    // cleanup everything
    Render::ReleaseResource(this->offscreenDrawState);
    Render::ReleaseResource(this->copyDrawState);
    Render::ReleaseResource(this->renderTarget);
    Dbg::Discard();
    Render::Discard();
    return App::OnCleanup();
}
