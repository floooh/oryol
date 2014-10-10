//------------------------------------------------------------------------------
//  Accelerometer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Gfx/Util/RawMeshLoader.h"
#include "Gfx/Util/ShapeBuilder.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class AccelApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP();

    GfxId drawState;
    glm::mat4 proj;
    glm::mat4 view;
};
OryolMain(AccelApp);

//------------------------------------------------------------------------------
AppState::Code
AccelApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(800, 400, "Oryol Accelerometer Sample");
    gfxSetup.Loaders.Add(RawMeshLoader::Creator());
    Gfx::Setup(gfxSetup);
    Dbg::Setup();
    Input::Setup();
    
    // create a 3D cube
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.Cylinder(0.25f, 2.0f, 36, 1, false)
        .Transform(glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f)))
        .Sphere(0.5f, 36, 8)
        .Build();
    GfxId mesh = Gfx::CreateResource(MeshSetup::FromStream(), shapeBuilder.Result());
    GfxId prog = Gfx::CreateResource(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    dss.RasterizerState.CullFaceEnabled = true;
    this->drawState = Gfx::CreateResource(dss);

    // setup transform matrices
    const float32 fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.1f, 10.0f);
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
AccelApp::computeMVP() {
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -6.0f));
    return this->proj * this->view * model;
}

//------------------------------------------------------------------------------
AppState::Code
AccelApp::OnRunning() {

    // draw frame
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyVariable(Shaders::Main::ModelViewProjection, this->computeMVP());
    Gfx::Draw(0);
    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
AccelApp::OnCleanup() {
    this->drawState.Release();
    Input::Discard();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
