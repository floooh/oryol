//------------------------------------------------------------------------------
//  TextureFloat.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Debug/DebugFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;
using namespace Oryol::Debug;
using namespace Oryol::Time;

class TextureFloatApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render = nullptr;
    DebugFacade* debug = nullptr;
    Resource::Id renderTarget;
    Resource::Id offscreenDrawState;
    Resource::Id copyDrawState;
    
    glm::mat4 view;
    glm::mat4 proj;
    float32 time = 0.0f;
    Time::TimePoint lastFrameTimePoint;
};
OryolMain(TextureFloatApp);

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnInit() {
    // setup rendering system
    auto renderSetup = RenderSetup::AsWindow(512, 512, false, "Oryol Float Texture Sample");
    renderSetup.Loaders.AddBack(RawMeshLoader::Creator());
    this->render = RenderFacade::CreateSingle(renderSetup);
    this->debug = DebugFacade::CreateSingle();

    // check required extensions
    if (!this->render->Supports(Feature::TextureFloat)) {
        o_error("ERROR: float_texture extension required!\n");
    }
    
    // create an offscreen float render target, same size as display,
    // configure texture sampler with point-filtering
    auto rtSetup = TextureSetup::AsRelSizeRenderTarget("rt", 1.0f, 1.0f);
    rtSetup.ColorFormat = PixelFormat::RGBA32F;
    rtSetup.MagFilter = TextureFilterMode::Nearest;
    rtSetup.MinFilter = TextureFilterMode::Nearest;
    this->renderTarget = this->render->CreateResource(rtSetup);
    
    // fullscreen mesh, we'll reuse this several times
    Id fullscreenMesh = this->render->CreateResource(MeshSetup::CreateFullScreenQuad("fsMesh"));

    // setup draw state for offscreen rendering to float render target
    Id offscreenProg = this->render->CreateResource(Shaders::Offscreen::CreateSetup());
    DrawStateSetup offscreenSetup("dsOffscreen", fullscreenMesh, offscreenProg, 0);
    this->offscreenDrawState = this->render->CreateResource(offscreenSetup);
    this->render->ReleaseResource(offscreenProg);
    
    // fullscreen-copy mesh, shader and draw state
    Id copyProg = this->render->CreateResource(Shaders::Copy::CreateSetup());
    this->copyDrawState = this->render->CreateResource(DrawStateSetup("dsCopy", fullscreenMesh, copyProg, 0));
    this->render->ReleaseResource(copyProg);
    this->render->ReleaseResource(fullscreenMesh);
    
    // setup static transform matrices
    const float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    const float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 5.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        this->time += 1.0f / 60.0f;
        
        // render plasma to offscreen render target
        this->render->ApplyOffscreenRenderTarget(this->renderTarget);
        this->render->ApplyDrawState(this->offscreenDrawState);
        this->render->ApplyVariable(Shaders::Offscreen::Time, this->time);
        this->render->Draw(0);
        
        // copy fullscreen quad
        this->render->ApplyDefaultRenderTarget();
        this->render->ApplyDrawState(this->copyDrawState);
        this->render->ApplyVariable(Shaders::Copy::Texture, this->renderTarget);
        this->render->Draw(0);
        
        this->debug->DrawTextBuffer();
        this->render->EndFrame();
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    this->debug->PrintF("%.3fms", frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->offscreenDrawState);
    this->render->ReleaseResource(this->copyDrawState);
    this->render->ReleaseResource(this->renderTarget);
    this->render = nullptr;
    this->debug = nullptr;
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
