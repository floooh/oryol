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
    PrimitiveGroup planePrimGroup;
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
    Gfx::Setup(GfxDesc::WindowMSAA4(800, 600, "Oryol Vertex Texture Sample"));
    Dbg::Setup(DbgSetup::MSAA4());
    
    // FIXME: need a way to check number of vertex texture units
    
    // create RGBA offscreen render pass which holds the plasma
    Id plasmaTex = Gfx::Texture()
        .RenderTarget(true)
        .Width(256)
        .Height(256)
        .Format(PixelFormat::RGBA8)
        .MinFilter(TextureFilterMode::Nearest)
        .MagFilter(TextureFilterMode::Nearest)
        .Create();
    this->plasmaRenderPass = Gfx::Pass()
        .ColorAttachment(0, plasmaTex)
        .Create();

    // setup draw state for offscreen rendering to float render target
    const float quadVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    this->plasmaDrawState.VertexBuffers[0] = Gfx::Buffer()
        .Size(sizeof(quadVertices))
        .Content(quadVertices)
        .Create();
    this->plasmaDrawState.Pipeline = Gfx::Pipeline()
        .Shader(Gfx::CreateShader(PlasmaShader::Desc()))
        .Layout(0, { { "in_pos", VertexFormat::Float2 } })
        .PrimitiveType(PrimitiveType::TriangleStrip)
        .ColorFormat(PixelFormat::RGBA8)
        .DepthFormat(PixelFormat::None)
        .Create();
    
    // draw state for a 256x256 plane
    auto shape = ShapeBuilder::New()
        .Positions("in_pos", VertexFormat::Float3)
        .TexCoords("in_uv", VertexFormat::Float2)
        .Plane(3.0f, 3.0f, 255)
        .Build();
    this->planePrimGroup = shape.PrimitiveGroups[0];
    this->planeDrawState.VertexBuffers[0] = Gfx::Buffer()
        .From(shape.VertexBufferDesc)
        .Content(shape.Data)
        .Create();
    this->planeDrawState.IndexBuffer = Gfx::Buffer()
        .From(shape.IndexBufferDesc)
        .Content(shape.Data)
        .Create();
    this->planeDrawState.Pipeline = Gfx::Pipeline()
        .From(shape.PipelineDesc)
        .Shader(Gfx::CreateShader(PlaneShader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .SampleCount(4)
        .Create();
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
    Gfx::BeginPass(this->plasmaRenderPass, PassAction::New().DontCare());
    Gfx::ApplyDrawState(this->plasmaDrawState);
    Gfx::ApplyUniformBlock(this->plasmaFSParams);
    Gfx::Draw(0, 4);
    Gfx::EndPass();

    // render displacement mapped plane shape
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->planeDrawState);
    Gfx::ApplyUniformBlock(this->planeVSParams);
    Gfx::Draw(this->planePrimGroup);
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

