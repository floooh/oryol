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
    
    PlaneShader::vsParams planeVSParams;
    PlasmaShader::fsParams plasmaFSParams;
    TimePoint lastFrameTimePoint;
};
OryolMain(VertexTextureApp);

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxDesc()
        .Width(800).Height(600)
        .SampleCount(4)
        .Title("Oryol Vertex Texture Sample")
        .HtmlTrackElementSize(true));
    Dbg::Setup(DbgDesc().SampleCount(4));
    
    // FIXME: need a way to check number of vertex texture units
    
    // create RGBA offscreen render pass which holds the plasma
    Id plasmaTex = Gfx::CreateTexture(TextureDesc()
        .RenderTarget(true)
        .Width(256)
        .Height(256)
        .Format(PixelFormat::RGBA8)
        .MinFilter(TextureFilterMode::Nearest)
        .MagFilter(TextureFilterMode::Nearest));
    this->plasmaRenderPass = Gfx::CreatePass(PassDesc().ColorAttachment(0, plasmaTex));

    // setup draw state for offscreen rendering to float render target
    const float quadVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    this->plasmaDrawState.VertexBuffers[0] = Gfx::CreateBuffer(BufferDesc()
        .Size(sizeof(quadVertices))
        .Content(quadVertices));
    this->plasmaDrawState.Pipeline = Gfx::CreatePipeline(PipelineDesc()
        .Shader(Gfx::CreateShader(PlasmaShader::Desc()))
        .Layout(0, { { "in_pos", VertexFormat::Float2 } })
        .PrimitiveType(PrimitiveType::TriangleStrip)
        .ColorFormat(PixelFormat::RGBA8)
        .DepthFormat(PixelFormat::None));
    
    // draw state for a 256x256 plane
    auto shape = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .TexCoords("in_uv", VertexFormat::Float2)
        .Plane(3.0f, 3.0f, 255)
        .Build();
    this->planePrimGroup = shape.PrimitiveGroups[0];
    this->planeDrawState.VertexBuffers[0] = Gfx::CreateBuffer(shape.VertexBufferDesc);
    this->planeDrawState.IndexBuffer = Gfx::CreateBuffer(shape.IndexBufferDesc);
    this->planeDrawState.Pipeline = Gfx::CreatePipeline(PipelineDesc(shape.PipelineDesc)
        .Shader(Gfx::CreateShader(PlaneShader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .SampleCount(4));
    this->planeDrawState.VSTexture[PlaneShader::tex] = plasmaTex;
    
    this->plasmaFSParams.time = 0.0f;

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VertexTextureApp::OnRunning() {
    
    this->plasmaFSParams.time += 1.0f / 60.0f;
    this->planeVSParams.mvp = this->computeMVP(glm::vec2(0.0f, 0.0f));

    // render plasma to offscreen render target
    Gfx::BeginPass(this->plasmaRenderPass, PassAction().DontCare());
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
    Dbg::PrintF("\n\n\n\n\n %.3fms", frameTime.AsMilliSeconds());
    
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
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), float(Gfx::Width()), float(Gfx::Height()), 0.01f, 10.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 modelTform = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
    modelTform = glm::rotate(modelTform, angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
    return proj * view * modelTform;
}

