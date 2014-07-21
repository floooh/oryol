//------------------------------------------------------------------------------
//  Shapes.cc
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
class ShapeApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::vec3& pos);

    RenderFacade* render = nullptr;
    Resource::Id drawState;
    glm::mat4 view;
    glm::mat4 proj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
};
OryolMain(ShapeApp);

//------------------------------------------------------------------------------
AppState::Code
ShapeApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(600, 400, "Oryol Shapes Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;

    // create resources
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetRandomColorsFlag(true);
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.AddBox(1.0f, 1.0f, 1.0f, 4);
    shapeBuilder.AddSphere(0.75f, 36, 20);
    shapeBuilder.AddCylinder(0.5f, 1.5f, 36, 10);
    shapeBuilder.AddTorus(0.3f, 0.5f, 20, 36);
    shapeBuilder.AddPlane(1.5f, 1.5f, 10);
    shapeBuilder.Build();
    Id mesh = this->render->CreateResource(MeshSetup::FromData("shapes"), shapeBuilder.GetStream());
    Id prog = this->render->CreateResource(Shaders::Shapes::CreateSetup());
    
    DrawStateSetup dsSetup("ds", mesh, prog, 0);
    dsSetup.DepthStencilState.SetDepthWriteEnabled(true);
    dsSetup.DepthStencilState.SetDepthCompareFunc(CompareFunc::LessEqual);
    this->drawState = this->render->CreateResource(dsSetup);

    this->render->ReleaseResource(mesh);
    this->render->ReleaseResource(prog);
    
    // setup projection and view matrices
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
ShapeApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
ShapeApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // update rotation angles
        this->angleY += 0.01f;
        this->angleX += 0.02f;

        // apply state and render
        this->render->ApplyDefaultRenderTarget();
        this->render->ApplyDrawState(this->drawState);
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyState(Render::State::ClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
        this->render->Clear(true, true, true);
        
        // render shape primitive groups
        this->render->ApplyVariable(Shaders::Shapes::ModelViewProjection, this->computeMVP(glm::vec3(-1.0, 1.0f, -6.0f)));
        this->render->Draw(0);
        this->render->ApplyVariable(Shaders::Shapes::ModelViewProjection, this->computeMVP(glm::vec3(1.0f, 1.0f, -6.0f)));
        this->render->Draw(1);
        this->render->ApplyVariable(Shaders::Shapes::ModelViewProjection, this->computeMVP(glm::vec3(-2.0f, -1.0f, -6.0f)));
        this->render->Draw(2);
        this->render->ApplyVariable(Shaders::Shapes::ModelViewProjection, this->computeMVP(glm::vec3(+2.0f, -1.0f, -6.0f)));
        this->render->Draw(3);
        this->render->ApplyVariable(Shaders::Shapes::ModelViewProjection, this->computeMVP(glm::vec3(0.0f, -1.0f, -6.0f)));
        this->render->Draw(4);
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ShapeApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->drawState);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
