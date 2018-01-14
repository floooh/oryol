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

    ShapeBuilder::Result donutShape;
    ShapeBuilder::Result sphereShape;
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

    auto gfxSetup = GfxSetup::WindowMSAA4(800, 600, "Oryol Simple Render Target Sample");
    gfxSetup.DefaultPassAction = PassAction::Clear(glm::vec4(0.25f, 0.45f, 0.65f, 1.0f));
    Gfx::Setup(gfxSetup);

    // create an offscreen render pass object with a color-attachment and depth-attachment
    auto colorDesc = MakeTextureDesc()
        .Type(TextureType::Texture2D)
        .RenderTarget(true)
        .Width(128)
        .Height(128)
        .Format(PixelFormat::RGBA8)
        .WrapU(TextureWrapMode::Repeat)
        .WrapV(TextureWrapMode::Repeat)
        .MagFilter(TextureFilterMode::Linear)
        .MinFilter(TextureFilterMode::Linear);
    // if supported, use an anti-aliased offscreen render target
    if (Gfx::QueryFeature(GfxFeature::MSAARenderTargets)) {
        colorDesc.SampleCount(4);
        Log::Info("Using MSAA4 render target\n");
    }
    Id colorTexture = Gfx::CreateTexture(colorDesc);
    auto depthDesc = MakeTextureDesc(colorDesc).Format(PixelFormat::DEPTH);
    Id depthTexture = Gfx::CreateTexture(depthDesc);

    auto rpSetup = PassSetup::From(colorTexture, depthTexture);
    rpSetup.DefaultAction = PassAction::Clear(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    this->renderPass = Gfx::CreateResource(rpSetup);

    // create a donut mesh, shader and pipeline object
    // (this will be rendered into the offscreen render target)
    ShapeBuilder shapeBuilder;
    this->donutShape = shapeBuilder
        .AddPositions("in_pos", VertexFormat::Float3)
        .AddNormals("in_normal", VertexFormat::Byte4N)
        .Torus(0.3f, 0.5f, 20, 36)
        .Build();
    this->offscreenDrawState.VertexBuffers[0] = Gfx::CreateBuffer(
        this->donutShape.VertexBufferDesc, this->donutShape.Data);
    this->offscreenDrawState.IndexBuffer = Gfx::CreateBuffer(
        this->donutShape.IndexBufferDesc, this->donutShape.Data);

    // create shader and pipeline-state-object for offscreen rendering
    this->offscreenDrawState.Pipeline = Gfx::CreatePipeline(MakePipelineDesc()
        .Shader(Gfx::CreateShader(OffscreenShader::Desc()))
        .Layout(0, this->donutShape.Layout)
        .IndexType(this->donutShape.IndexType)
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .ColorFormat(colorDesc.Desc.Format)
        .DepthFormat(depthDesc.Desc.Format)
        .SampleCount(colorDesc.Desc.SampleCount)


    Id offScreenShader = Gfx::CreateResource(OffscreenShader::Setup());
    auto offpsSetup = PipelineSetup::FromShaderAndLayout(offScreenShader, this->donutShape.Layout);
    offpsSetup.IndexType = this->donutShape.IndexType;
    offpsSetup.DepthStencilState.DepthWriteEnabled = true;
    offpsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    offpsSetup.BlendState.ColorFormat = rtSetup.ColorFormat;
    offpsSetup.BlendState.DepthFormat = rtSetup.DepthFormat;
    offpsSetup.RasterizerState.SampleCount = rtSetup.SampleCount;
    this->offscreenDrawState.Pipeline = Gfx::CreateResource(offpsSetup);

    // create a sphere mesh, shader and pipeline object for rendering to display
    this->sphereShape = shapeBuilder
        .AddPositions("in_pos", VertexFormat::Float3)
        .AddNormals("in_normal", VertexFormat::Byte4N)
        .AddTexCoords("in_uv", VertexFormat::Float2)
        .Sphere(0.5f, 72, 40)
        .Build();
    this->displayDrawState.VertexBuffers[0] = Gfx::CreateResource(
        this->sphereShape.VertexBufferSetup, this->sphereShape.Data);
    this->displayDrawState.IndexBuffer = Gfx::CreateResource(
        this->sphereShape.IndexBufferSetup, this->sphereShape.Data);

    Id dispShader = Gfx::CreateResource(DisplayShader::Setup());
    auto disppsSetup = PipelineSetup::FromShaderAndLayout(dispShader, this->sphereShape.Layout);
    disppsSetup.IndexType = IndexType::UInt16;
    disppsSetup.DepthStencilState.DepthWriteEnabled = true;
    disppsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    disppsSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->displayDrawState.Pipeline = Gfx::CreateResource(disppsSetup);
    this->displayDrawState.FSTexture[DisplayShader::tex] = rtTexture;

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
    Gfx::BeginPass(this->renderPass);
    Gfx::ApplyDrawState(this->offscreenDrawState);
    this->offscreenParams.mvp = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
    Gfx::ApplyUniformBlock(this->offscreenParams);
    Gfx::Draw();
    Gfx::EndPass();
    
    // render sphere to display, with offscreen render target as texture
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->displayDrawState);
    this->displayVSParams.mvp = this->computeMVP(this->displayProj, -this->angleX * 0.25f, this->angleY * 0.25f, glm::vec3(0.0f, 0.0f, -1.5f));
    Gfx::ApplyUniformBlock(this->displayVSParams);
    Gfx::Draw();
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
