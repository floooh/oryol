//------------------------------------------------------------------------------
//  VertexTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Assets/Gfx/ShapeBuilder.h"
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

    Id plasmaRenderTarget;
    Id plasmaDrawState;
    Id planeDrawState;
    Id planeTextureBlock;
    
    glm::mat4 view;
    glm::mat4 proj;
    Shaders::Plane::VSParams planeVSParams;
    Shaders::Plasma::FSParams plasmaFSParams;
    TimePoint lastFrameTimePoint;
    ClearState noClearState = ClearState::ClearNone();
};
OryolMain(VertexTextureApp);

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnRunning() {
    
    this->plasmaFSParams.Time += 1.0f / 60.0f;
    this->planeVSParams.ModelViewProjection = this->computeMVP(glm::vec2(0.0f, 0.0f));

    // render plasma to offscreen render target
    Gfx::ApplyRenderTarget(this->plasmaRenderTarget, this->noClearState);
    Gfx::ApplyDrawState(this->plasmaDrawState);
    Gfx::ApplyUniformBlock(this->plasmaFSParams);
    Gfx::Draw(0);

    // render displacement mapped plane shape
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->planeDrawState);
    Gfx::ApplyUniformBlock(this->planeVSParams);
    Gfx::ApplyTextureBlock(this->planeTextureBlock);
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
VertexTextureApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxSetup::WindowMSAA4(800, 600, "Oryol Vertex Texture Sample"));
    Dbg::Setup();
    
    // FIXME: need a way to check number of vertex texture units
    
    // create RGBA offscreen render target which holds the plasma
    auto rtSetup = TextureSetup::RenderTarget(256, 256);
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.MinFilter = TextureFilterMode::Nearest;
    rtSetup.MagFilter = TextureFilterMode::Nearest;
    this->plasmaRenderTarget = Gfx::CreateResource(rtSetup);

    // setup draw state for offscreen rendering to float render target
    Id fsQuadMesh = Gfx::CreateResource(MeshSetup::FullScreenQuad());
    Id plasmaShader = Gfx::CreateResource(Shaders::Plasma::Setup());
    auto dss = DrawStateSetup::FromMeshAndShader(fsQuadMesh, plasmaShader);
    dss.BlendState.ColorFormat = rtSetup.ColorFormat;
    dss.BlendState.DepthFormat = rtSetup.DepthFormat;
    this->plasmaDrawState = Gfx::CreateResource(dss);
    
    // draw state for a 256x256 plane
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Plane(3.0f, 3.0f, 255).Build();
    Id planeMesh = Gfx::CreateResource(shapeBuilder.Result());
    Id planeShader = Gfx::CreateResource(Shaders::Plane::Setup());
    auto dsPlane = DrawStateSetup::FromMeshAndShader(planeMesh, planeShader);
    dsPlane.DepthStencilState.DepthWriteEnabled = true;
    dsPlane.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    dsPlane.RasterizerState.SampleCount = 4;
    this->planeDrawState = Gfx::CreateResource(dsPlane);

    // texture block for the plane vertex texture
    auto tbSetup = Shaders::Plane::VSTextures::Setup(planeShader);
    tbSetup.Slot[Shaders::Plane::VSTextures::Texture] = this->plasmaRenderTarget;
    this->planeTextureBlock = Gfx::CreateResource(tbSetup);
    
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 10.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->plasmaFSParams.Time = 0.0f;

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnCleanup() {
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

