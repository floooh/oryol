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
private:
    glm::mat4 computeMVP(const glm::mat4& proj, float rotX, float rotY, const glm::vec3& pos);

    Id renderPass;
    DrawState offscreenDrawState;
    DrawState displayDrawState;
    OffscreenShader::VSParams offscreenParams;
    DisplayShader::VSParams displayVSParams;
    PassState offscreenPassState = PassState(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    PassState displayPassState = PassState(glm::vec4(0.25f, 0.45f, 0.65f, 1.0f));
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
    Gfx::Setup(gfxSetup);

    // create an offscreen render pass object with a single color attachment
    // texture, we explicitly want repeat texture wrap mode and linear blending...
    auto rtSetup = TextureSetup::RenderTarget(128, 128);
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.DepthFormat = PixelFormat::DEPTH;
    rtSetup.Sampler.WrapU = TextureWrapMode::Repeat;
    rtSetup.Sampler.WrapV = TextureWrapMode::Repeat;
    rtSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    rtSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    // if supported, use an anti-aliased offscreen render target
    if (Gfx::QueryFeature(GfxFeature::MSAARenderTargets)) {
        rtSetup.SampleCount = 4;
        Log::Info("Using MSAA4 render target\n");
    }
    Id rtTexture = Gfx::CreateResource(rtSetup);
    auto rpSetup = RenderPassSetup::From(rtTexture, rtTexture);
    rpSetup.StoreAction = RenderPassStoreAction::Resolve;
    this->renderPass = Gfx::CreateResource(rpSetup);

    // create a donut mesh, shader and pipeline object
    // (this will be rendered into the offscreen render target)
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.Torus(0.3f, 0.5f, 20, 36);
    this->offscreenDrawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());

    // create shader and pipeline-state-object for offscreen rendering
    Id offScreenShader = Gfx::CreateResource(OffscreenShader::Setup());
    auto offpsSetup = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, offScreenShader);
    offpsSetup.DepthStencilState.DepthWriteEnabled = true;
    offpsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    offpsSetup.BlendState.ColorFormat = rtSetup.ColorFormat;
    offpsSetup.BlendState.DepthFormat = rtSetup.DepthFormat;
    offpsSetup.RasterizerState.SampleCount = rtSetup.SampleCount;
    this->offscreenDrawState.Pipeline = Gfx::CreateResource(offpsSetup);

    // create a sphere mesh, shader and pipeline object for rendering to display
    shapeBuilder.Layout
        .Clear()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Sphere(0.5f, 72, 40);
    this->displayDrawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());

    Id dispShader = Gfx::CreateResource(DisplayShader::Setup());
    auto disppsSetup = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, dispShader);
    disppsSetup.DepthStencilState.DepthWriteEnabled = true;
    disppsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    disppsSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->displayDrawState.Pipeline = Gfx::CreateResource(disppsSetup);
    this->displayDrawState.FSTexture[Textures::Texture] = rtTexture;

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
    Gfx::BeginPass(this->renderPass, this->offscreenPassState);
    Gfx::ApplyDrawState(this->offscreenDrawState);
    this->offscreenParams.ModelViewProjection = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
    Gfx::ApplyUniformBlock(this->offscreenParams);
    Gfx::Draw();
    Gfx::EndPass();
    
    // render sphere to display, with offscreen render target as texture
    Gfx::BeginPass(this->displayPassState);
    Gfx::ApplyDrawState(this->displayDrawState);
    this->displayVSParams.ModelViewProjection = this->computeMVP(this->displayProj, -this->angleX * 0.25f, this->angleY * 0.25f, glm::vec3(0.0f, 0.0f, -1.5f));
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
