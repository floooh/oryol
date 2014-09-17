//------------------------------------------------------------------------------
//  PBRendering
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Dbg/Dbg.h"
#include "Gfx/Gfx.h"
#include "Gfx/Util/ShapeBuilder.h"
#include "Gfx/Util/RawMeshLoader.h"
#include "shaders.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace Oryol;

class PBRenderingApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    void applyTransforms(const glm::vec3& pos) const;
    void applyDirLight() const;

    Id drawState;
    glm::mat4 proj;
    glm::mat4 view;
};
OryolMain(PBRenderingApp);

//------------------------------------------------------------------------------
AppState::Code
PBRenderingApp::OnRunning() {
    
    Dbg::Print("\n Work in progress!");
    
    // render one frame
    if (Gfx::BeginFrame()) {
        
        Gfx::ApplyDefaultRenderTarget();
        Gfx::ApplyDrawState(this->drawState);
        Gfx::Clear(PixelChannel::All, glm::vec4(0.3f, 0.3f, 0.3f, 0.0f), 1.0f, 0);
        
        this->applyDirLight();
        this->applyTransforms(glm::vec3(0.0f, 2.0f, 0.0f));
        Gfx::Draw(0);
        this->applyTransforms(glm::vec3(0.0f, 0.0f, 0.0f));
        Gfx::Draw(1);
        
        Dbg::DrawTextBuffer();
        Gfx::EndFrame();
    }
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PBRenderingApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(1024, 600, true, "Oryol PBR Sample");
    gfxSetup.Loaders.Add(RawMeshLoader::Creator());
    Gfx::Setup(gfxSetup);
    Dbg::Setup();
    
    // create resources
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Float3);
    shapeBuilder.Sphere(0.5f, 36, 20, true)
        .Plane(5.0f, 5.0f, 1, true)
        .Build();
    Id mesh = Gfx::CreateResource(MeshSetup::FromStream(), shapeBuilder.Result());
    Id prog = Gfx::CreateResource(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = Gfx::CreateResource(dss);
    
    Gfx::ReleaseResource(mesh);
    Gfx::ReleaseResource(prog);
    
    // setup projection and view matrices
    float32 fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    float32 fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 2.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PBRenderingApp::OnCleanup() {
    Gfx::ReleaseResource(this->drawState);
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
void
PBRenderingApp::applyDirLight() const {
    // compute directional light vector in view space (vector TOWARDS light)
    glm::vec4 worldLightDir(glm::normalize(glm::vec4(1.0, 1.0, 1.0, 0.0)));
    glm::vec3 viewSpaceLightDir = glm::vec3(this->view * worldLightDir);
    Gfx::ApplyVariable(Shaders::Main::LightVec, viewSpaceLightDir);
}

//------------------------------------------------------------------------------
void
PBRenderingApp::applyTransforms(const glm::vec3& pos) const {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    glm::mat4 modelView = this->view * modelTform;
    glm::mat4 modelViewProj = this->proj * this->view * modelTform;
    Gfx::ApplyVariable(Shaders::Main::ModelViewProj, modelViewProj);
    Gfx::ApplyVariable(Shaders::Main::ModelView, modelView);
}
