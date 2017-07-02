//------------------------------------------------------------------------------
//  VertexTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/Time/Clock.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class VertexTextureApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
    glm::mat4 computeMVP(const glm::vec2& angles);

    Id plasmaRenderPass;
    DrawState plasmaDrawState;
    DrawState planeDrawState;
    
    glm::mat4 view;
    glm::mat4 proj;
    PlaneShader::vsParams planeVSParams;
    PlasmaShader::fsParams plasmaFSParams;
    TimePoint lastFrameTimePoint;
};
OryolMain(VertexTextureApp);

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxSetup::WindowMSAA4(800, 600, "Oryol Vertex Texture Sample"));
    Dbg::Setup();
    
    // FIXME: need a way to check number of vertex texture units
    
    // create RGBA offscreen render pass which holds the plasma
    auto rtSetup = TextureSetup::RenderTarget2D(256, 256, PixelFormat::RGBA8);
    rtSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
    rtSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
    Id plasmaTex = Gfx::CreateResource(rtSetup);
    auto passSetup = PassSetup::From(plasmaTex);
    passSetup.DefaultAction.DontCareColor(0);
    this->plasmaRenderPass = Gfx::CreateResource(passSetup);

    // setup draw state for offscreen rendering to float render target
    auto quadSetup = MeshSetup::FullScreenQuad();
    this->plasmaDrawState.Mesh[0] = Gfx::CreateResource(quadSetup);
    Id plasmaShader = Gfx::CreateResource(PlasmaShader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(quadSetup.Layout, plasmaShader);
    ps.BlendState.ColorFormat = rtSetup.ColorFormat;
    ps.BlendState.DepthFormat = rtSetup.DepthFormat;
    this->plasmaDrawState.Pipeline = Gfx::CreateResource(ps);
    
    // draw state for a 256x256 plane
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::TexCoord0, VertexFormat::Float2 }
    };
    shapeBuilder.Plane(3.0f, 3.0f, 255);
    this->planeDrawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    Id planeShader = Gfx::CreateResource(PlaneShader::Setup());
    auto psPlane = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, planeShader);
    psPlane.DepthStencilState.DepthWriteEnabled = true;
    psPlane.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    psPlane.RasterizerState.SampleCount = 4;
    this->planeDrawState.Pipeline = Gfx::CreateResource(psPlane);
    this->planeDrawState.VSTexture[PlaneShader::tex] = plasmaTex;
    
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 10.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->plasmaFSParams.time = 0.0f;

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnRunning() {
    
    this->plasmaFSParams.time += 1.0f / 60.0f;
    this->planeVSParams.mvp = this->computeMVP(glm::vec2(0.0f, 0.0f));

    // render plasma to offscreen render target
    Gfx::BeginPass(this->plasmaRenderPass);
    Gfx::ApplyDrawState(this->plasmaDrawState);
    Gfx::ApplyUniformBlock(this->plasmaFSParams);
    Gfx::Draw();
    Gfx::EndPass();

    // render displacement mapped plane shape
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->planeDrawState);
    Gfx::ApplyUniformBlock(this->planeVSParams);
    Gfx::Draw();
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("%.3fms", frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
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

