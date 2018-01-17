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

    auto gfxDesc = GfxDesc::WindowMSAA4(800, 600, "Oryol Simple Render Target Sample");
    Gfx::Setup(gfxDesc);

    // create a color render target texture and compatible depth render target
    // texture for offscreen rendering
    const PixelFormat::Code rtColorFormat = PixelFormat::RGBA8;
    const PixelFormat::Code rtDepthFormat = PixelFormat::DEPTH;
    const int rtSampleCount = Gfx::QueryFeature(GfxFeature::MSAARenderTargets) ? 4 : 1;
    auto rtCommon = Gfx::Texture()
        .Type(TextureType::Texture2D)
        .RenderTarget(true)
        .Width(128)
        .Height(128)
        .WrapU(TextureWrapMode::Repeat)
        .WrapV(TextureWrapMode::Repeat)
        .MagFilter(TextureFilterMode::Linear)
        .MinFilter(TextureFilterMode::Linear)
        .SampleCount(rtSampleCount)
        .Desc;
    Id rtColorTexture = Gfx::Texture().From(rtCommon).Format(rtColorFormat).Create();
    Id rtDepthTexture = Gfx::Texture().From(rtCommon).Format(rtDepthFormat).Create();
    this->renderPass = Gfx::Pass()
        .ColorAttachment(0, rtColorTexture)
        .DepthStencilAttachment(rtDepthTexture)
        .Create();

    // create a donut mesh, shader and pipeline object
    // (this will be rendered into the offscreen render target)
    auto donut = ShapeBuilder::New()
        .Positions("in_pos", VertexFormat::Float3)
        .Normals("in_normal", VertexFormat::Byte4N)
        .Torus(0.3f, 0.5f, 20, 36)
        .Build();
    this->donutPrimGroup = donut.PrimitiveGroups[0];
    this->offscreenDrawState.VertexBuffers[0] = Gfx::Buffer()
        .From(donut.VertexBufferDesc)
        .Content(donut.Data)
        .Create();
    this->offscreenDrawState.IndexBuffer = Gfx::Buffer()
        .From(donut.IndexBufferDesc)
        .Content(donut.Data)
        .Create();
    this->offscreenDrawState.Pipeline = Gfx::Pipeline()
        .From(donut.PipelineDesc)
        .Shader(Gfx::CreateShader(OffscreenShader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .ColorFormat(rtColorFormat)
        .DepthFormat(rtDepthFormat)
        .SampleCount(rtSampleCount)
        .Create();

    // create a sphere mesh, shader and pipeline object for rendering to display
    auto sphere = ShapeBuilder::New()
        .Positions("in_pos", VertexFormat::Float3)
        .Normals("in_normal", VertexFormat::Byte4N)
        .TexCoords("in_uv", VertexFormat::Float2)
        .Sphere(0.5f, 72, 40)
        .Build();
    this->spherePrimGroup = sphere.PrimitiveGroups[0];
    this->displayDrawState.VertexBuffers[0] = Gfx::Buffer()
        .From(sphere.VertexBufferDesc)
        .Content(sphere.Data)
        .Create();
    this->displayDrawState.IndexBuffer = Gfx::Buffer()
        .From(sphere.IndexBufferDesc)
        .Content(sphere.Data)
        .Create();
    this->displayDrawState.Pipeline = Gfx::Pipeline()
        .From(sphere.PipelineDesc)
        .Shader(Gfx::CreateShader(DisplayShader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .SampleCount(gfxDesc.SampleCount)
        .Create();
    this->displayDrawState.FSTexture[DisplayShader::tex] = rtColorTexture;

    // setup static transform matrices
    float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
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
    Gfx::BeginPass(this->renderPass, PassAction::Clear(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)));
    Gfx::ApplyDrawState(this->offscreenDrawState);
    this->offscreenParams.mvp = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
    Gfx::ApplyUniformBlock(this->offscreenParams);
    Gfx::Draw(this->donutPrimGroup);
    Gfx::EndPass();
    
    // render sphere to display, with offscreen render target as texture
    Gfx::BeginPass(PassAction::Clear(glm::vec4(0.25f, 0.45f, 0.65f, 1.0f)));
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
