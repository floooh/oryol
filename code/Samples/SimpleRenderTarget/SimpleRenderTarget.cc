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
    Id offscreenPipeline;
    Id displayPipeline;
    Bindings offscreenBindings;
    Bindings displayBindings;
    OffscreenShader::vsParams offscreenParams;
    DisplayShader::vsParams displayVSParams;
    glm::mat4 offscreenProj;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(SimpleRenderTargetApp);

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnInit() {
    Gfx::Setup(GfxDesc()
        .SetWidth(800)
        .SetHeight(600)
        .SetSampleCount(4)
        .SetTitle("Oryol Simple Render Target Sample")
        .SetHtmlTrackElementSize(true));

    // create a color render target texture and compatible depth render target
    // texture for offscreen rendering
    const PixelFormat::Code rtColorFormat = PixelFormat::RGBA8;
    const PixelFormat::Code rtDepthFormat = PixelFormat::DEPTH;
    const int rtSampleCount = Gfx::QueryFeature(GfxFeature::MSAARenderTargets) ? 4 : 1;
    auto rtCommon = TextureDesc()
        .SetType(TextureType::Texture2D)
        .SetRenderTarget(true)
        .SetWidth(128)
        .SetHeight(128)
        .SetWrapU(TextureWrapMode::Repeat)
        .SetWrapV(TextureWrapMode::Repeat)
        .SetMagFilter(TextureFilterMode::Linear)
        .SetMinFilter(TextureFilterMode::Linear)
        .SetSampleCount(rtSampleCount);
    Id rtColorTexture = Gfx::CreateTexture(TextureDesc(rtCommon).SetFormat(rtColorFormat));
    Id rtDepthTexture = Gfx::CreateTexture(TextureDesc(rtCommon).SetFormat(rtDepthFormat));
    this->renderPass = Gfx::CreatePass(PassDesc()
        .SetColorAttachment(0, rtColorTexture)
        .SetDepthStencilAttachment(rtDepthTexture));

    // create a donut mesh, shader and pipeline object
    // (this will be rendered into the offscreen render target)
    auto donut = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .Normals("in_normal", VertexFormat::Byte4N)
        .Torus(0.3f, 0.5f, 20, 36)
        .Build();
    this->donutPrimGroup = donut.PrimitiveGroups[0];
    this->offscreenBindings.VertexBuffers[0] = Gfx::CreateBuffer(donut.VertexBufferDesc);
    this->offscreenBindings.IndexBuffer = Gfx::CreateBuffer(donut.IndexBufferDesc);
    this->offscreenPipeline = Gfx::CreatePipeline(PipelineDesc(donut.PipelineDesc)
        .SetShader(Gfx::CreateShader(OffscreenShader::Desc()))
        .SetDepthWriteEnabled(true)
        .SetDepthCmpFunc(CompareFunc::LessEqual)
        .SetColorFormat(rtColorFormat)
        .SetDepthFormat(rtDepthFormat)
        .SetSampleCount(rtSampleCount));

    // create a sphere mesh, shader and pipeline object for rendering to display
    auto sphere = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .Normals("in_normal", VertexFormat::Byte4N)
        .TexCoords("in_uv", VertexFormat::Float2)
        .Sphere(0.5f, 72, 40)
        .Build();
    this->spherePrimGroup = sphere.PrimitiveGroups[0];
    this->displayBindings.VertexBuffers[0] = Gfx::CreateBuffer(sphere.VertexBufferDesc);
    this->displayBindings.IndexBuffer = Gfx::CreateBuffer(sphere.IndexBufferDesc);
    this->displayPipeline = Gfx::CreatePipeline(PipelineDesc(sphere.PipelineDesc)
        .SetShader(Gfx::CreateShader(DisplayShader::Desc()))
        .SetDepthWriteEnabled(true)
        .SetDepthCmpFunc(CompareFunc::LessEqual)
        .SetSampleCount(Gfx::Desc().SampleCount));
    this->displayBindings.FSTexture[DisplayShader::tex] = rtColorTexture;

    // setup static transform matrices
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnRunning() {
    
    // update angles
    this->angleY += 0.01f;
    this->angleX += 0.02f;
    
    // render donut to offscreen render target
    Gfx::BeginPass(this->renderPass, PassAction().Clear(0.25f, 0.25f, 0.25f, 1.0f));
    Gfx::ApplyPipeline(this->offscreenPipeline);
    Gfx::ApplyBindings(this->offscreenBindings);
    this->offscreenParams.mvp = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
    Gfx::ApplyUniforms(this->offscreenParams);
    Gfx::Draw(this->donutPrimGroup);
    Gfx::EndPass();
    
    // render sphere to display, with offscreen render target as texture
    Gfx::BeginPass(PassAction().Clear(0.25f, 0.45f, 0.65f, 1.0f));
    Gfx::ApplyPipeline(this->displayPipeline);
    Gfx::ApplyBindings(this->displayBindings);
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), float(Gfx::Width()), float(Gfx::Height()), 0.01f, 100.0f);
    this->displayVSParams.mvp = this->computeMVP(proj, -this->angleX * 0.25f, this->angleY * 0.25f, glm::vec3(0.0f, 0.0f, -1.5f));
    Gfx::ApplyUniforms(this->displayVSParams);
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
    return proj * modelTform;
}
