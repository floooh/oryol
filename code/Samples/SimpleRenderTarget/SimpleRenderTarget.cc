//------------------------------------------------------------------------------
//  SimpleRenderTarget.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

// derived application class
class SimpleRenderTargetApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    glm::mat4 computeMVP(const glm::mat4& proj, float rotX, float rotY, const glm::vec3& pos);

    PrimitiveGroup donutPrimGroup;
    PrimitiveGroup spherePrimGroup;
    Id renderPass;
    DrawState offscreenDrawState;
    DrawState displayDrawState;
    OffscreenShader::vsParams offscreenParams;
    DisplayShader::vsParams displayVSParams;
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(SimpleRenderTargetApp);

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnInit() {
    Gfx::Setup(GfxDesc().Width(800).Height(600).SampleCount(4).Title("Oryol Simple Render Target Sample"));

    // create a color render target texture and compatible depth render target
    // texture for offscreen rendering
    const PixelFormat::Code rtColorFormat = PixelFormat::RGBA8;
    const PixelFormat::Code rtDepthFormat = PixelFormat::DEPTH;
    const int rtSampleCount = Gfx::QueryFeature(GfxFeature::MSAARenderTargets) ? 4 : 1;
    auto rtCommon = TextureDesc()
        .Type(TextureType::Texture2D)
        .RenderTarget(true)
        .Width(128)
        .Height(128)
        .WrapU(TextureWrapMode::Repeat)
        .WrapV(TextureWrapMode::Repeat)
        .MagFilter(TextureFilterMode::Linear)
        .MinFilter(TextureFilterMode::Linear)
        .SampleCount(rtSampleCount);
    Id rtColorTexture = Gfx::CreateTexture(TextureDesc(rtCommon).Format(rtColorFormat));
    Id rtDepthTexture = Gfx::CreateTexture(TextureDesc(rtCommon).Format(rtDepthFormat));
    this->renderPass = Gfx::CreatePass(PassDesc()
        .ColorAttachment(0, rtColorTexture)
        .DepthStencilAttachment(rtDepthTexture));

    // create a donut mesh, shader and pipeline object
    // (this will be rendered into the offscreen render target)
    auto donut = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .Normals("in_normal", VertexFormat::Byte4N)
        .Torus(0.3f, 0.5f, 20, 36)
        .Build();
    this->donutPrimGroup = donut.PrimitiveGroups[0];
    this->offscreenDrawState.VertexBuffers[0] = Gfx::CreateBuffer(donut.VertexBufferDesc);
    this->offscreenDrawState.IndexBuffer = Gfx::CreateBuffer(donut.IndexBufferDesc);
    this->offscreenDrawState.Pipeline = Gfx::CreatePipeline(PipelineDesc(donut.PipelineDesc)
        .Shader(Gfx::CreateShader(OffscreenShader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .ColorFormat(rtColorFormat)
        .DepthFormat(rtDepthFormat)
        .SampleCount(rtSampleCount));

    // create a sphere mesh, shader and pipeline object for rendering to display
    auto sphere = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .Normals("in_normal", VertexFormat::Byte4N)
        .TexCoords("in_uv", VertexFormat::Float2)
        .Sphere(0.5f, 72, 40)
        .Build();
    this->spherePrimGroup = sphere.PrimitiveGroups[0];
    this->displayDrawState.VertexBuffers[0] = Gfx::CreateBuffer(sphere.VertexBufferDesc);
    this->displayDrawState.IndexBuffer = Gfx::CreateBuffer(sphere.IndexBufferDesc);
    this->displayDrawState.Pipeline = Gfx::CreatePipeline(PipelineDesc(sphere.PipelineDesc)
        .Shader(Gfx::CreateShader(DisplayShader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .SampleCount(Gfx::Desc().SampleCount()));
    this->displayDrawState.FSTexture[DisplayShader::tex] = rtColorTexture;

    // setup static transform matrices
    float fbWidth = (const float) Gfx::DisplayAttrs().Width;
    float fbHeight = (const float) Gfx::DisplayAttrs().Height;
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnRunning() {
    
    // update angles
    this->angleY += 0.01f;
    this->angleX += 0.02f;
    
    // render donut to offscreen render target
    Gfx::BeginPass(this->renderPass, PassAction::New().Clear(0.25f, 0.25f, 0.25f, 1.0f));
    Gfx::ApplyDrawState(this->offscreenDrawState);
    this->offscreenParams.mvp = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
    Gfx::ApplyUniformBlock(this->offscreenParams);
    Gfx::Draw(this->donutPrimGroup);
    Gfx::EndPass();
    
    // render sphere to display, with offscreen render target as texture
    Gfx::BeginPass(PassAction::New().Clear(0.25f, 0.45f, 0.65f, 1.0f));
    Gfx::ApplyDrawState(this->displayDrawState);
    this->displayVSParams.mvp = this->computeMVP(this->displayProj, -this->angleX * 0.25f, this->angleY * 0.25f, glm::vec3(0.0f, 0.0f, -1.5f));
    Gfx::ApplyUniformBlock(this->displayVSParams);
    Gfx::Draw(this->spherePrimGroup);
    Gfx::EndPass();
    
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
SimpleRenderTargetApp::computeMVP(const glm::mat4& proj, float rotX, float rotY, const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    return proj * this->view * modelTform;
}
