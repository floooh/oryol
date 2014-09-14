//------------------------------------------------------------------------------
//  VertexTexture.cc
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

class VertexTextureApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::vec2& angles);

    Id renderTarget;
    Id plasmaDrawState;
    Id planeDrawState;
    
    glm::mat4 view;
    glm::mat4 proj;
    float32 time = 0.0f;
    TimePoint lastFrameTimePoint;
};
OryolMain(VertexTextureApp);

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnRunning() {
    // render one frame
    if (Render::BeginFrame()) {
        
        this->time += 1.0f / 60.0f;
        
        // render plasma to offscreen render target
        Render::ApplyOffscreenRenderTarget(this->renderTarget);
        Render::ApplyDrawState(this->plasmaDrawState);
        Render::ApplyVariable(Shaders::Plasma::Time, this->time);
        Render::Draw(0);
        
        // render displacement mapped plane shape
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::All, glm::vec4(0.0f), 1.0f, 0);
        Render::ApplyDrawState(this->planeDrawState);
        const glm::mat4 mvp = this->computeMVP(glm::vec2(0.0f, 0.0f));
        Render::ApplyVariable(Shaders::Plane::ModelViewProjection, mvp);
        Render::ApplyVariable(Shaders::Plane::Texture, this->renderTarget);
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
VertexTextureApp::OnInit() {
    // setup rendering system
    auto renderSetup = RenderSetup::Window(800, 600, true, "Oryol Vertex Texture Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    Render::Setup(renderSetup);
    Dbg::Setup();
    
    // FIXME: need a way to check number of vertex texture units
    
    // create RGBA offscreen render target which holds the plasma
    auto rtSetup = TextureSetup::RenderTarget(256, 256);
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.MinFilter = TextureFilterMode::Nearest;
    rtSetup.MagFilter = TextureFilterMode::Nearest;
    this->renderTarget = Render::CreateResource(rtSetup);

    // setup draw state for offscreen rendering to float render target
    Id fsQuadMesh = Render::CreateResource(MeshSetup::FullScreenQuad());
    Id plasmaProg = Render::CreateResource(Shaders::Plasma::CreateSetup());
    this->plasmaDrawState = Render::CreateResource(DrawStateSetup::FromMeshAndProg(fsQuadMesh, plasmaProg));
    Render::ReleaseResource(fsQuadMesh);
    Render::ReleaseResource(plasmaProg);
    
    // draw state for a 256x256 plane
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Plane(3.0f, 3.0f, 255).Build();
    Id planeMesh = Render::CreateResource(MeshSetup::FromStream(), shapeBuilder.Result());
    Id planeProg = Render::CreateResource(Shaders::Plane::CreateSetup());
    auto dsPlane = DrawStateSetup::FromMeshAndProg(planeMesh, planeProg);
    dsPlane.DepthStencilState.DepthWriteEnabled = true;
    dsPlane.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->planeDrawState = Render::CreateResource(dsPlane);
    Render::ReleaseResource(planeMesh);
    Render::ReleaseResource(planeProg);
    
    // setup static transform matrices
    const float32 fbWidth = Render::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Render::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 10.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnCleanup() {
    // cleanup everything
    Render::ReleaseResource(this->planeDrawState);
    Render::ReleaseResource(this->plasmaDrawState);
    Render::ReleaseResource(this->renderTarget);
    Dbg::Discard();
    Render::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
VertexTextureApp::computeMVP(const glm::vec2& angles) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
    modelTform = glm::rotate(modelTform, angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

