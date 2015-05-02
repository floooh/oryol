//-----------------------------------------------------------------------------
//  MeshLoading.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
#include "Assets/Gfx/MeshLoader.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class MeshLoadingApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

private:
    void updateCamera();

    int32 frameCount = 0;
    Id mesh;
    Id drawState;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 modelViewProj;
};
OryolMain(MeshLoadingApp);

//-----------------------------------------------------------------------------
AppState::Code
MeshLoadingApp::OnInit() {

    // setup IO system
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("msh:", "http://localhost:8000/");
    IO::Setup(ioSetup);

    // setup rendering system
    Gfx::Setup(GfxSetup::WindowMSAA4(800, 600, "Oryol Mesh Loading Sample"));

    // load mesh
    this->mesh = Gfx::LoadResource(MeshLoader::Create(MeshSetup::FromFile("msh:tiger.omsh"), 0));
    Id prog = Gfx::CreateResource(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(this->mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = Gfx::CreateResource(dss);

    // setup projection and view matrices
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(60.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    
    return App::OnInit();
}

//-----------------------------------------------------------------------------
AppState::Code
MeshLoadingApp::OnRunning() {
    this->frameCount++;
    this->updateCamera();
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    if (ResourceState::Valid == Gfx::QueryResourceInfo(this->mesh).State) {
        Gfx::ApplyDrawState(this->drawState);
        Gfx::ApplyVariable(Shaders::Main::ModelViewProjection, this->modelViewProj);
        Gfx::Draw(0);
    }
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//-----------------------------------------------------------------------------
AppState::Code
MeshLoadingApp::OnCleanup() {
    Gfx::Discard();
    IO::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
void
MeshLoadingApp::updateCamera() {
    float32 angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.0f, glm::cos(angle) * 10.0f);
    this->view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelViewProj = this->proj * this->view * this->model;
}

