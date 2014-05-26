//------------------------------------------------------------------------------
//  PBRendering
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/ShapeBuilder.h"
#include "Render/Util/RawMeshLoader.h"
#include "shaders.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;

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
    Resource::Id meshId;
    Resource::Id stateId;
    Resource::Id progId;
    glm::mat4 proj;
    glm::mat4 view;
};
OryolMain(PBRenderingApp);

//------------------------------------------------------------------------------
AppState::Code
PBRenderingApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(1024, 600, "Oryol PBR Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    
    // create shapes in separate primitive groups
    ShapeBuilder shapeBuilder;
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Normal, VertexFormat::Float3);
    shapeBuilder.AddSphere(0.5f, 36, 20, true);
    shapeBuilder.AddPlane(5.0f, 5.0f, 1, true);
    shapeBuilder.Build();
    this->meshId = this->render->CreateResource(MeshSetup::FromData("shapes"), shapeBuilder.GetStream());
    
    // build a state block with the static depth state
    StateBlockSetup stateSetup("state");
    stateSetup.AddState(Render::State::DepthMask, true);
    stateSetup.AddState(Render::State::DepthTestEnabled, true);
    stateSetup.AddState(Render::State::DepthFunc, Render::State::LessEqual);
    stateSetup.AddState(Render::State::ClearDepth, 1.0f);
    stateSetup.AddState(Render::State::ClearColor, 0.3f, 0.3f, 0.3f, 0.0f);
    this->stateId = this->render->CreateResource(stateSetup);
    
    // setup shaders
    this->progId = this->render->CreateResource(Shaders::Main::CreateSetup());
    
    // setup projection and view matrices
    float32 fbWidth = this->render->GetDisplayAttrs().GetFramebufferWidth();
    float32 fbHeight = this->render->GetDisplayAttrs().GetFramebufferHeight();
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
    // render one frame
    if (this->render->BeginFrame()) {
    
        this->render->ApplyStateBlock(this->stateId);
        this->render->Clear(true, true, true);
        this->render->ApplyMesh(this->meshId);
        
        this->render->ApplyProgram(this->progId, 0);
        this->applyDirLight();
        this->applyTransforms(glm::vec3(0.0f, 2.0f, 0.0f));
        this->render->Draw(0);
        this->applyTransforms(glm::vec3(0.0f, 0.0f, 0.0f));
        this->render->Draw(1);
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PBRenderingApp::OnCleanup() {
    this->render->DiscardResource(this->stateId);
    this->render->DiscardResource(this->meshId);
    this->render->DiscardResource(this->progId);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
