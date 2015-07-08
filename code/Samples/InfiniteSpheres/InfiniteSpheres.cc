//------------------------------------------------------------------------------
//  InfiniteSpheres.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;

class InfiniteSpheresApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    glm::mat4 computeModel(float32 rotX, float32 rotY, const glm::vec3& pos);
    glm::mat4 computeMVP(const glm::mat4& proj, const glm::mat4& model);

    Id renderTargets[2];
    Id offscreenDrawState;
    Id displayDrawState;
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
    int32 frameIndex = 0;
    Shaders::Main::VSParams vsParams;
    Shaders::Main::FSParams fsParams;
    ClearState clearState;
};
OryolMain(InfiniteSpheresApp);

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnRunning() {
    
    // update angles
    this->angleY += 0.01f;
    this->angleX += 0.02f;
    this->frameIndex++;
    const int32 index0 = this->frameIndex % 2;
    const int32 index1 = (this->frameIndex + 1) % 2;
    
    // render sphere to offscreen render target, using the other render target as
    // source texture
    Gfx::ApplyRenderTarget(this->renderTargets[index0]);
    Gfx::ApplyDrawState(this->offscreenDrawState);
    glm::mat4 model = this->computeModel(this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -2.0f));
    this->vsParams.ModelViewProjection = this->computeMVP(this->offscreenProj, model);
    this->fsParams.Texture = this->renderTargets[index1];
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::ApplyUniformBlock(this->fsParams);
    Gfx::Draw(0);
    
    // ...and again to display
    Gfx::ApplyDefaultRenderTarget(this->clearState);
    Gfx::ApplyDrawState(this->displayDrawState);
    model = this->computeModel(-this->angleX, -this->angleY, glm::vec3(0.0f, 0.0f, -2.0f));
    this->vsParams.ModelViewProjection = this->computeMVP(this->displayProj, model);
    this->fsParams.Texture = this->renderTargets[index0];
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::ApplyUniformBlock(this->fsParams);
    Gfx::Draw(0);
    
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxSetup::WindowMSAA4(800, 600, "Oryol Infinite Spheres Sample"));

    // create resources
    auto rtSetup = TextureSetup::RenderTarget(512, 512);
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.DepthFormat = PixelFormat::D16;
    rtSetup.MinFilter = TextureFilterMode::Linear;
    rtSetup.MagFilter = TextureFilterMode::Linear;
    rtSetup.WrapU = TextureWrapMode::Repeat;
    rtSetup.WrapV = TextureWrapMode::Repeat;
    for (int32 i = 0; i < 2; i++) {
        this->renderTargets[i] = Gfx::CreateResource(rtSetup);
    }
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Sphere(0.75f, 72, 40).Build();
    Id sphere = Gfx::CreateResource(shapeBuilder.Result());
    Id prog = Gfx::CreateResource(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(sphere, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    dss.RasterizerState.SampleCount = 4;
    this->displayDrawState = Gfx::CreateResource(dss);
    dss.BlendState.ColorFormat = rtSetup.ColorFormat;
    dss.BlendState.DepthFormat = rtSetup.DepthFormat;
    dss.RasterizerState.SampleCount = 1;
    this->offscreenDrawState = Gfx::CreateResource(dss);
    this->clearState.Color = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    
    // setup static transform matrices
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 20.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
InfiniteSpheresApp::computeModel(float32 rotX, float32 rotY, const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    return modelTform;
}

//------------------------------------------------------------------------------
glm::mat4
InfiniteSpheresApp::computeMVP(const glm::mat4& proj, const glm::mat4& modelTform) {
    return proj * this->view * modelTform;
}

