//------------------------------------------------------------------------------
//  VertexTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Gfx/Util/RawMeshLoader.h"
#include "Gfx/Util/ShapeBuilder.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class VertexTextureApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
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
    if (Gfx::BeginFrame()) {
        
        this->time += 1.0f / 60.0f;
        
        // render plasma to offscreen render target
        Gfx::ApplyOffscreenRenderTarget(this->renderTarget);
        Gfx::ApplyDrawState(this->plasmaDrawState);
        Gfx::ApplyVariable(Shaders::Plasma::Time, this->time);
        Gfx::Draw(0);
        
        // render displacement mapped plane shape
        Gfx::ApplyDefaultRenderTarget();
        Gfx::Clear(PixelChannel::All, glm::vec4(0.0f), 1.0f, 0);
        Gfx::ApplyDrawState(this->planeDrawState);
        const glm::mat4 mvp = this->computeMVP(glm::vec2(0.0f, 0.0f));
        Gfx::ApplyVariable(Shaders::Plane::ModelViewProjection, mvp);
        Gfx::ApplyVariable(Shaders::Plane::Texture, this->renderTarget);
        Gfx::Draw(0);
        
        Dbg::DrawTextBuffer();
        Gfx::EndFrame();
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("%.3fms", frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnInit() {
    // setup rendering system
    auto gfxSetup = GfxSetup::Window(800, 600, true, "Oryol Vertex Texture Sample");
    gfxSetup.Loaders.Add(RawMeshLoader::Creator());
    Gfx::Setup(gfxSetup);
    Dbg::Setup();
    
    // FIXME: need a way to check number of vertex texture units
    
    // create RGBA offscreen render target which holds the plasma
    auto rtSetup = TextureSetup::RenderTarget(256, 256);
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.MinFilter = TextureFilterMode::Nearest;
    rtSetup.MagFilter = TextureFilterMode::Nearest;
    this->renderTarget = Gfx::CreateResource(rtSetup);

    // setup draw state for offscreen rendering to float render target
    Id fsQuadMesh = Gfx::CreateResource(MeshSetup::FullScreenQuad());
    Id plasmaProg = Gfx::CreateResource(Shaders::Plasma::CreateSetup());
    this->plasmaDrawState = Gfx::CreateResource(DrawStateSetup::FromMeshAndProg(fsQuadMesh, plasmaProg));
    Gfx::ReleaseResource(fsQuadMesh);
    Gfx::ReleaseResource(plasmaProg);
    
    // draw state for a 256x256 plane
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Plane(3.0f, 3.0f, 255).Build();
    Id planeMesh = Gfx::CreateResource(MeshSetup::FromStream(), shapeBuilder.Result());
    Id planeProg = Gfx::CreateResource(Shaders::Plane::CreateSetup());
    auto dsPlane = DrawStateSetup::FromMeshAndProg(planeMesh, planeProg);
    dsPlane.DepthStencilState.DepthWriteEnabled = true;
    dsPlane.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->planeDrawState = Gfx::CreateResource(dsPlane);
    Gfx::ReleaseResource(planeMesh);
    Gfx::ReleaseResource(planeProg);
    
    // setup static transform matrices
    const float32 fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 10.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnCleanup() {
    // cleanup everything
    Gfx::ReleaseResource(this->planeDrawState);
    Gfx::ReleaseResource(this->plasmaDrawState);
    Gfx::ReleaseResource(this->renderTarget);
    Dbg::Discard();
    Gfx::Discard();
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

