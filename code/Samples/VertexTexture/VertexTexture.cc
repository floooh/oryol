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
    Id plasmaPipeline;
    Bindings plasmaBind;
    PrimitiveGroup planePrimGroup;
    Id planePipeline;
    Bindings planeBind;
    
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
        .SetWidth(800)
        .SetHeight(600)
        .SetSampleCount(4)
        .SetTitle("Oryol Vertex Texture Sample")
        .SetHtmlTrackElementSize(true));
    Dbg::Setup(DbgDesc().SetSampleCount(4));
    
    // FIXME: need a way to check number of vertex texture units
    
    // create RGBA offscreen render pass which holds the plasma
    Id plasmaTex = Gfx::CreateTexture(TextureDesc()
        .SetRenderTarget(true)
        .SetWidth(256)
        .SetHeight(256)
        .SetFormat(PixelFormat::RGBA8)
        .SetMinFilter(TextureFilterMode::Nearest)
        .SetMagFilter(TextureFilterMode::Nearest));
    this->plasmaRenderPass = Gfx::CreatePass(PassDesc().SetColorAttachment(0, plasmaTex));

    // setup draw state for offscreen rendering to float render target
    const float quadVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    this->plasmaBind.VertexBuffers[0] = Gfx::CreateBuffer(BufferDesc()
        .SetSize(sizeof(quadVertices))
        .SetContent(quadVertices));
    this->plasmaPipeline = Gfx::CreatePipeline(PipelineDesc()
        .SetShader(Gfx::CreateShader(PlasmaShader::Desc()))
        .SetLayout(0, { { "in_pos", VertexFormat::Float2 } })
        .SetPrimitiveType(PrimitiveType::TriangleStrip)
        .SetColorFormat(PixelFormat::RGBA8)
        .SetDepthFormat(PixelFormat::None));
    
    // draw state for a 256x256 plane
    auto shape = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .TexCoords("in_uv", VertexFormat::Float2)
        .Plane(3.0f, 3.0f, 255)
        .Build();
    this->planePrimGroup = shape.PrimitiveGroups[0];
    this->planeBind.VertexBuffers[0] = Gfx::CreateBuffer(shape.VertexBufferDesc);
    this->planeBind.IndexBuffer = Gfx::CreateBuffer(shape.IndexBufferDesc);
    this->planeBind.VSTexture[PlaneShader::tex] = plasmaTex;
    this->planePipeline = Gfx::CreatePipeline(PipelineDesc(shape.PipelineDesc)
        .SetShader(Gfx::CreateShader(PlaneShader::Desc()))
        .SetDepthWriteEnabled(true)
        .SetDepthCmpFunc(CompareFunc::LessEqual)
        .SetSampleCount(4));
    
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
    Gfx::ApplyPipeline(this->plasmaPipeline);
    Gfx::ApplyBindings(this->plasmaBind);
    Gfx::ApplyUniforms(this->plasmaFSParams);
    Gfx::Draw(0, 4);
    Gfx::EndPass();

    // render displacement mapped plane shape
    Gfx::BeginPass();
    Gfx::ApplyPipeline(this->planePipeline);
    Gfx::ApplyBindings(this->planeBind);
    Gfx::ApplyUniforms(this->planeVSParams);
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

