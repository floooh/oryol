//------------------------------------------------------------------------------
//  PBRendering
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Debug/DebugFacade.h"
#include "Render/RenderFacade.h"
#include "Render/Util/ShapeBuilder.h"
#include "Render/Util/RawMeshLoader.h"
#include "shaders.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Debug;
using namespace Oryol::Render;
using namespace Oryol::Resource;

// derived application class
class PBRenderingApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    void applyTransforms(const glm::vec3& pos) const;
    void applyDirLight() const;

    RenderFacade* render;
    DebugFacade* debug;
    Id drawState;
    glm::mat4 proj;
    glm::mat4 view;
};
OryolMain(PBRenderingApp);

//------------------------------------------------------------------------------
AppState::Code
PBRenderingApp::OnInit() {
    this->debug  = DebugFacade::CreateSingle();
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(1024, 600, "Oryol PBR Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    
    // create resources
    ShapeBuilder shapeBuilder;
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Normal, VertexFormat::Float3);
    shapeBuilder.AddSphere(0.5f, 36, 20, true);
    shapeBuilder.AddPlane(5.0f, 5.0f, 1, true);
    shapeBuilder.Build();
    Id mesh = this->render->CreateResource(MeshSetup::FromData("shapes"), shapeBuilder.GetStream());
    Id prog = this->render->CreateResource(Shaders::Main::CreateSetup());
    DrawStateSetup dsSetup("ds", mesh, prog, 0);
    dsSetup.DepthStencilState.SetDepthWriteEnabled(true);
    dsSetup.DepthStencilState.SetDepthCompareFunc(CompareFunc::LessEqual);
    this->drawState = this->render->CreateResource(dsSetup);
    
    this->render->ReleaseResource(mesh);
    this->render->ReleaseResource(prog);
    
    // setup projection and view matrices
    float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 2.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
void
PBRenderingApp::applyDirLight() const {
    // compute directional light vector in view space (vector TOWARDS light)
    glm::vec4 worldLightDir(glm::normalize(glm::vec4(1.0, 1.0, 1.0, 0.0)));
    glm::vec3 viewSpaceLightDir = glm::vec3(this->view * worldLightDir);
    this->render->ApplyVariable(Shaders::Main::LightVec, viewSpaceLightDir);
}

//------------------------------------------------------------------------------
void
PBRenderingApp::applyTransforms(const glm::vec3& pos) const {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    glm::mat4 modelView = this->view * modelTform;
    glm::mat4 modelViewProj = this->proj * this->view * modelTform;
    this->render->ApplyVariable(Shaders::Main::ModelViewProj, modelViewProj);
    this->render->ApplyVariable(Shaders::Main::ModelView, modelView);
}

//------------------------------------------------------------------------------
AppState::Code
PBRenderingApp::OnRunning() {
    
    this->debug->Print("\n Work in progress!");
    
    // render one frame
    if (this->render->BeginFrame()) {
    
        this->render->ApplyDefaultRenderTarget();
        this->render->ApplyDrawState(this->drawState);
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyState(Render::State::ClearColor, 0.3f, 0.3f, 0.3f, 0.0f);
        this->render->Clear(true, true, true);
        
        this->applyDirLight();
        this->applyTransforms(glm::vec3(0.0f, 2.0f, 0.0f));
        this->render->Draw(0);
        this->applyTransforms(glm::vec3(0.0f, 0.0f, 0.0f));
        this->render->Draw(1);
        
        this->debug->DrawTextBuffer();
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PBRenderingApp::OnCleanup() {
    this->render->ReleaseResource(this->drawState);
    this->render = nullptr;
    this->debug  = nullptr;
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
