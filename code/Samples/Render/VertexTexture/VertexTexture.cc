//------------------------------------------------------------------------------
//  VertexTexture.cc
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

class VertexTextureApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::vec2& angles);

    RenderFacade* render = nullptr;
    DebugFacade* debug = nullptr;
    Resource::Id renderTarget;
    Resource::Id plasmaDrawState;
    Resource::Id planeDrawState;
    
    glm::mat4 view;
    glm::mat4 proj;
    float32 time = 0.0f;
    Time::TimePoint lastFrameTimePoint;
};
OryolMain(VertexTextureApp);

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(800, 600, "Oryol Vertex Texture Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    this->debug = DebugFacade::CreateSingle();
    
    // FIXME: need a way to check number of vertex texture units
    
    // create RGBA offscreen render target which holds the plasma
    auto rtSetup = TextureSetup::AsRenderTarget("rt", 256, 256, PixelFormat::RGBA8, PixelFormat::None);
    rtSetup.MinFilter = TextureFilterMode::Nearest;
    rtSetup.MagFilter = TextureFilterMode::Nearest;
    this->renderTarget = this->render->CreateResource(rtSetup);

    // setup draw state for offscreen rendering to float render target
    Id fsQuadMesh = this->render->CreateResource(MeshSetup::CreateFullScreenQuad("fsMesh"));
    Id plasmaProg = this->render->CreateResource(Shaders::Plasma::CreateSetup());
    DrawStateSetup plasmaSetup("dsPlasma", fsQuadMesh, plasmaProg, 0);
    this->plasmaDrawState = this->render->CreateResource(plasmaSetup);
    this->render->ReleaseResource(fsQuadMesh);
    this->render->ReleaseResource(plasmaProg);
    
    // draw state for a 256x256 plane
    ShapeBuilder shapeBuilder;
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.AddPlane(3.0f, 3.0f, 255);
    shapeBuilder.Build();
    Id planeMesh = this->render->CreateResource(MeshSetup::FromData("planeMesh"), shapeBuilder.GetStream());
    Id planeProg = this->render->CreateResource(Shaders::Plane::CreateSetup());
    DrawStateSetup dsPlane = DrawStateSetup("dsPlane", planeMesh, planeProg, 0);
    dsPlane.DepthStencilState.DepthWriteEnabled = true;
    dsPlane.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->planeDrawState = this->render->CreateResource(dsPlane);
    this->render->ReleaseResource(planeMesh);
    this->render->ReleaseResource(planeProg);
    
    // setup static transform matrices
    const float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    const float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 10.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
VertexTextureApp::computeMVP(const glm::vec2& angles) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
    modelTform = glm::rotate(modelTform, angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        this->time += 1.0f / 60.0f;
        
        // render plasma to offscreen render target
        this->render->ApplyOffscreenRenderTarget(this->renderTarget);
        this->render->ApplyDrawState(this->plasmaDrawState);
        this->render->ApplyVariable(Shaders::Plasma::Time, this->time);
        this->render->Draw(0);
        
        // render displacement mapped plane shape
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::All, glm::vec4(0.0f), 1.0f, 0);
        this->render->ApplyDrawState(this->planeDrawState);        
        const glm::mat4 mvp = this->computeMVP(glm::vec2(0.0f, 0.0f));
        this->render->ApplyVariable(Shaders::Plane::ModelViewProjection, mvp);
        this->render->ApplyVariable(Shaders::Plane::Texture, this->renderTarget);
        this->render->Draw(0);
        
        this->debug->DrawTextBuffer();
        this->render->EndFrame();
    }
    
    TimePoint curTime = Clock::Now();
    Duration frameTime = curTime - this->lastFrameTimePoint;
    this->lastFrameTimePoint = curTime;
    
    this->debug->PrintF("%.3fms", frameTime.AsMilliSeconds());

    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->planeDrawState);
    this->render->ReleaseResource(this->plasmaDrawState);
    this->render->ReleaseResource(this->renderTarget);
    this->render = nullptr;
    this->debug = nullptr;
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
