//------------------------------------------------------------------------------
//  SimpleRenderTarget.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;

// derived application class
class SimpleRenderTargetApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::mat4& proj, float32 rotX, float32 rotY, const glm::vec3& pos);

    RenderFacade* render = nullptr;
    Resource::Id renderTarget;
    Resource::Id offscreenDrawState;
    Resource::Id displayDrawState;
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
};
OryolMain(SimpleRenderTargetApp);

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(800, 600, "Oryol Simple Render Target Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;

    // create an offscreen render target, we explicitely want repeat texture wrap mode
    // and linear blending...
    auto rtSetup = TextureSetup::AsRenderTarget("rt", 128, 128, PixelFormat::R8G8B8, PixelFormat::D16);
    rtSetup.WrapU = TextureWrapMode::Repeat;
    rtSetup.WrapV = TextureWrapMode::Repeat;
    rtSetup.MagFilter = TextureFilterMode::Linear;
    rtSetup.MinFilter = TextureFilterMode::Linear;
    this->renderTarget = this->render->CreateResource(rtSetup);
    
    // create a donut (this will be rendered into the offscreen render target)
    ShapeBuilder shapeBuilder;
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.AddTorus(0.3f, 0.5f, 20, 36);
    shapeBuilder.Build();
    Id torus = this->render->CreateResource(MeshSetup::FromData("torus"), shapeBuilder.GetStream());
    
    // create a sphere mesh with normals and uv coords
    shapeBuilder.Clear();
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.VertexLayout().Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.AddSphere(0.5f, 72.0f, 40.0f);
    shapeBuilder.Build();
    Id sphere = this->render->CreateResource(MeshSetup::FromData("sphere"), shapeBuilder.GetStream());

    // create shaders
    Id offScreenProg = this->render->CreateResource(Shaders::RenderTarget::CreateSetup());
    Id dispProg = this->render->CreateResource(Shaders::Main::CreateSetup());
    
    // create one draw state for offscreen rendering, and one draw state for main target rendering
    DrawStateSetup offdsSetup("offds", torus, offScreenProg, 0);
    offdsSetup.DepthStencilState.SetDepthWriteEnabled(true);
    offdsSetup.DepthStencilState.SetDepthCompareFunc(CompareFunc::LessEqual);
    this->offscreenDrawState = this->render->CreateResource(offdsSetup);
    DrawStateSetup dispdsSetup("dispds", sphere, dispProg, 0);
    dispdsSetup.DepthStencilState.SetDepthWriteEnabled(true);
    dispdsSetup.DepthStencilState.SetDepthCompareFunc(CompareFunc::LessEqual);
    this->displayDrawState = this->render->CreateResource(dispdsSetup);
    
    this->render->ReleaseResource(torus);
    this->render->ReleaseResource(sphere);
    this->render->ReleaseResource(offScreenProg);
    this->render->ReleaseResource(dispProg);
    
    // setup static transform matrices
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
SimpleRenderTargetApp::computeMVP(const glm::mat4& proj, float32 rotX, float32 rotY, const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    return proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // update angles
        this->angleY += 0.01f;
        this->angleX += 0.02f;
        
        // apply general states
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyState(Render::State::ClearColor, 0.25f, 0.25f, 0.25f, 0.0f);
        
        // render donut to offscreen render target
        this->render->ApplyOffscreenRenderTarget(this->renderTarget);
        this->render->Clear(true, true, true);
        this->render->ApplyDrawState(this->offscreenDrawState);
        glm::mat4 donutMVP = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
        this->render->ApplyVariable(Shaders::RenderTarget::ModelViewProjection, donutMVP);
        this->render->Draw(0);
        
        // render sphere to display, with offscreen render target as texture
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(true, true, true);
        this->render->ApplyDrawState(this->displayDrawState);
        glm::mat4 sphereMVP = this->computeMVP(this->displayProj, -this->angleX * 0.25f, this->angleY * 0.25f, glm::vec3(0.0f, 0.0f, -1.5f));
        this->render->ApplyVariable(Shaders::Main::ModelViewProjection, sphereMVP);
        this->render->ApplyVariable(Shaders::Main::Texture, this->renderTarget);
        this->render->Draw(0);

        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->offscreenDrawState);
    this->render->ReleaseResource(this->displayDrawState);
    this->render->ReleaseResource(this->renderTarget);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
