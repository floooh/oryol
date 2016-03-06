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
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();    
private:
    glm::mat4 computeMVP(const glm::mat4& proj, float32 rotX, float32 rotY, const glm::vec3& pos);

    Id renderTarget;
    Id offscreenPipeline;
    Id displayPipeline;
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
    Shaders::RenderTarget::VSParams offscreenParams;
    Shaders::Main::VSParams displayVSParams;
    Shaders::Main::FSTextures displayFSTextures;
    ClearState offscreenClearState = ClearState::ClearAll(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    ClearState displayClearState = ClearState::ClearAll(glm::vec4(0.25f, 0.45f, 0.65f, 1.0f));
    MeshBlock donutMesh;
    MeshBlock sphereMesh;
};
OryolMain(SimpleRenderTargetApp);

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnRunning() {
    
    // update angles
    this->angleY += 0.01f;
    this->angleX += 0.02f;
    
    // render donut to offscreen render target
    Gfx::ApplyRenderTarget(this->renderTarget, this->offscreenClearState);
    Gfx::ApplyDrawState(this->offscreenPipeline, this->donutMesh);
    this->offscreenParams.ModelViewProjection = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
    Gfx::ApplyUniformBlock(this->offscreenParams);
    Gfx::Draw(0);
    
    // render sphere to display, with offscreen render target as texture
    this->displayVSParams.ModelViewProjection = this->computeMVP(this->displayProj, -this->angleX * 0.25f, this->angleY * 0.25f, glm::vec3(0.0f, 0.0f, -1.5f));
    Gfx::ApplyDefaultRenderTarget(this->displayClearState);
    Gfx::ApplyDrawState(this->displayPipeline, this->sphereMesh, this->displayFSTextures);
    Gfx::ApplyUniformBlock(this->displayVSParams);
    Gfx::Draw(0);
    
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnInit() {
    // setup rendering system
    auto gfxSetup = GfxSetup::WindowMSAA4(800, 600, "Oryol Simple Render Target Sample");
    gfxSetup.ClearHint = this->displayClearState;
    Gfx::Setup(gfxSetup);

    // create an offscreen render target, we explicitly want repeat texture wrap mode
    // and linear blending...
    auto rtSetup = TextureSetup::RenderTarget(128, 128);
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.DepthFormat = PixelFormat::DEPTH;
    rtSetup.Sampler.WrapU = TextureWrapMode::Repeat;
    rtSetup.Sampler.WrapV = TextureWrapMode::Repeat;
    rtSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    rtSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    rtSetup.ClearHint = this->offscreenClearState;
    this->renderTarget = Gfx::CreateResource(rtSetup);

    // create a donut mesh, shader and pipeline object
    // (this will be rendered into the offscreen render target)
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.Torus(0.3f, 0.5f, 20, 36);
    this->donutMesh[0] = Gfx::CreateResource(shapeBuilder.Build());

    Id offScreenShader = Gfx::CreateResource(Shaders::RenderTarget::Setup());

    auto offpsSetup = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, offScreenShader);
    offpsSetup.DepthStencilState.DepthWriteEnabled = true;
    offpsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    offpsSetup.BlendState.ColorFormat = PixelFormat::RGBA8;
    offpsSetup.BlendState.DepthFormat = PixelFormat::DEPTH;
    this->offscreenPipeline = Gfx::CreateResource(offpsSetup);

    // create a sphere mesh, shader, pipeline object and texture with normals and uv coords
    shapeBuilder.Layout
        .Clear()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Sphere(0.5f, 72, 40);
    this->sphereMesh[0] = Gfx::CreateResource(shapeBuilder.Build());

    Id dispShader = Gfx::CreateResource(Shaders::Main::Setup());

    auto disppsSetup = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, dispShader);
    disppsSetup.DepthStencilState.DepthWriteEnabled = true;
    disppsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    disppsSetup.RasterizerState.SampleCount = 4;
    this->displayPipeline = Gfx::CreateResource(disppsSetup);

    this->displayFSTextures.Texture = this->renderTarget;

    // setup static transform matrices
    float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
SimpleRenderTargetApp::computeMVP(const glm::mat4& proj, float32 rotX, float32 rotY, const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    return proj * this->view * modelTform;
}
