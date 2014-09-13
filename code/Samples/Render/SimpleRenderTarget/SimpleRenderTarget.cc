//------------------------------------------------------------------------------
//  SimpleRenderTarget.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/Render.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

// derived application class
class SimpleRenderTargetApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::mat4& proj, float32 rotX, float32 rotY, const glm::vec3& pos);

    Id renderTarget;
    Id offscreenDrawState;
    Id displayDrawState;
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
};
OryolMain(SimpleRenderTargetApp);

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnRunning() {
    // render one frame
    if (Render::BeginFrame()) {
        
        // update angles
        this->angleY += 0.01f;
        this->angleX += 0.02f;
        
        // render donut to offscreen render target
        Render::ApplyOffscreenRenderTarget(this->renderTarget);
        Render::Clear(PixelChannel::All, glm::vec4(0.25f), 1.0f, 0);
        Render::ApplyDrawState(this->offscreenDrawState);
        glm::mat4 donutMVP = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
        Render::ApplyVariable(Shaders::RenderTarget::ModelViewProjection, donutMVP);
        Render::Draw(0);
        
        // render sphere to display, with offscreen render target as texture
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::All, glm::vec4(0.25f), 1.0f, 0);
        Render::ApplyDrawState(this->displayDrawState);
        glm::mat4 sphereMVP = this->computeMVP(this->displayProj, -this->angleX * 0.25f, this->angleY * 0.25f, glm::vec3(0.0f, 0.0f, -1.5f));
        Render::ApplyVariable(Shaders::Main::ModelViewProjection, sphereMVP);
        Render::ApplyVariable(Shaders::Main::Texture, this->renderTarget);
        Render::Draw(0);
        
        Render::EndFrame();
    }
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnInit() {
    // setup rendering system
    auto renderSetup = RenderSetup::AsWindow(800, 600, true, "Oryol Simple Render Target Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    Render::Setup(renderSetup);

    // create an offscreen render target, we explicitely want repeat texture wrap mode
    // and linear blending...
    auto rtSetup = TextureSetup::AsRenderTarget("rt", 128, 128);
    rtSetup.ColorFormat = PixelFormat::RGB8;
    rtSetup.DepthFormat = PixelFormat::D16;
    rtSetup.WrapU = TextureWrapMode::Repeat;
    rtSetup.WrapV = TextureWrapMode::Repeat;
    rtSetup.MagFilter = TextureFilterMode::Linear;
    rtSetup.MinFilter = TextureFilterMode::Linear;
    this->renderTarget = Render::CreateResource(rtSetup);
    
    // create a donut (this will be rendered into the offscreen render target)
    ShapeBuilder shapeBuilder;
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.AddTorus(0.3f, 0.5f, 20, 36);
    shapeBuilder.Build();
    Id torus = Render::CreateResource(MeshSetup::FromData("torus"), shapeBuilder.GetStream());
    
    // create a sphere mesh with normals and uv coords
    shapeBuilder.Clear();
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.VertexLayout().Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.AddSphere(0.5f, 72.0f, 40.0f);
    shapeBuilder.Build();
    Id sphere = Render::CreateResource(MeshSetup::FromData("sphere"), shapeBuilder.GetStream());

    // create shaders
    Id offScreenProg = Render::CreateResource(Shaders::RenderTarget::CreateSetup());
    Id dispProg = Render::CreateResource(Shaders::Main::CreateSetup());
    
    // create one draw state for offscreen rendering, and one draw state for main target rendering
    DrawStateSetup offdsSetup("offds", torus, offScreenProg, 0);
    offdsSetup.DepthStencilState.DepthWriteEnabled = true;
    offdsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->offscreenDrawState = Render::CreateResource(offdsSetup);
    DrawStateSetup dispdsSetup("dispds", sphere, dispProg, 0);
    dispdsSetup.DepthStencilState.DepthWriteEnabled = true;
    dispdsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->displayDrawState = Render::CreateResource(dispdsSetup);
    
    Render::ReleaseResource(torus);
    Render::ReleaseResource(sphere);
    Render::ReleaseResource(offScreenProg);
    Render::ReleaseResource(dispProg);
    
    // setup static transform matrices
    float32 fbWidth = Render::GetDisplayAttrs().FramebufferWidth;
    float32 fbHeight = Render::GetDisplayAttrs().FramebufferHeight;
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnCleanup() {
    // cleanup everything
    Render::ReleaseResource(this->offscreenDrawState);
    Render::ReleaseResource(this->displayDrawState);
    Render::ReleaseResource(this->renderTarget);
    Render::Discard();
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
