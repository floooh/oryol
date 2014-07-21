//------------------------------------------------------------------------------
//  InfiniteSpheres.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;

class InfiniteSpheresApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeModel(float32 rotX, float32 rotY, const glm::vec3& pos);
    glm::mat4 computeMVP(const glm::mat4& proj, const glm::mat4& model);

    RenderFacade* render = nullptr;
    Resource::Id renderTargets[2];
    Resource::Id drawState;
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
    int32 frameIndex = 0;
};
OryolMain(InfiniteSpheresApp);

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(800, 600, "Oryol Infinite Spheres Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;

    // create resources
    for (int32 i = 0; i < 2; i++) {
        auto rtSetup = TextureSetup::AsRenderTarget(Locator::NonShared(), 512, 512, PixelFormat::R8G8B8, PixelFormat::D16);
        rtSetup.MinFilter = TextureFilterMode::Linear;
        rtSetup.MagFilter = TextureFilterMode::Linear;
        rtSetup.WrapU = TextureWrapMode::Repeat;
        rtSetup.WrapV = TextureWrapMode::Repeat;
        this->renderTargets[i] = this->render->CreateResource(rtSetup);
    }
    ShapeBuilder shapeBuilder;
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.VertexLayout().Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.AddSphere(0.75f, 72.0f, 40.0f);
    shapeBuilder.Build();
    Id sphere = this->render->CreateResource(MeshSetup::FromData("sphere"), shapeBuilder.GetStream());
    Id prog = this->render->CreateResource(Shaders::Main::CreateSetup());
    DrawStateSetup dsSetup("ds", sphere, prog, 0);
    dsSetup.DepthStencilState.SetDepthWriteEnabled(true);
    dsSetup.DepthStencilState.SetDepthCompareFunc(CompareFunc::LessEqual);
    this->drawState = this->render->CreateResource(dsSetup);
    
    this->render->ReleaseResource(sphere);
    this->render->ReleaseResource(prog);
    
    // setup static transform matrices
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 20.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
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

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // update angles
        this->angleY += 0.01f;
        this->angleX += 0.02f;
        this->frameIndex++;
        const int32 index0 = this->frameIndex % 2;
        const int32 index1 = (this->frameIndex + 1) % 2;
        
        // general render states
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyDrawState(this->drawState);
        
        // render sphere to offscreen render target, using the other render target as
        // source texture
        this->render->ApplyOffscreenRenderTarget(this->renderTargets[index0]);
        this->render->ApplyState(Render::State::ClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
        this->render->Clear(true, true, true);
        glm::mat4 model = this->computeModel(this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -2.0f));
        glm::mat4 mvp = this->computeMVP(this->offscreenProj, model);
        this->render->ApplyVariable(Shaders::Main::ModelViewProjection, mvp);
        this->render->ApplyVariable(Shaders::Main::Texture, this->renderTargets[index1]);
        this->render->Draw(0);
        
        // ...and again to display
        this->render->ApplyDefaultRenderTarget();
        this->render->ApplyState(Render::State::ClearColor, 0.25f, 0.25f, 0.25f, 0.0f);
        this->render->Clear(true, true, true);
        model = this->computeModel(-this->angleX, -this->angleY, glm::vec3(0.0f, 0.0f, -2.0f));
        mvp = this->computeMVP(this->displayProj, model);
        this->render->ApplyVariable(Shaders::Main::ModelViewProjection, mvp);
        this->render->ApplyVariable(Shaders::Main::Texture, this->renderTargets[index0]);
        this->render->Draw(0);
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->drawState);
    for (int32 i = 0; i < 2; i++) {
        this->render->ReleaseResource(this->renderTargets[i]);
    }
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
