//------------------------------------------------------------------------------
//  Sensors.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Gfx/Util/RawMeshLoader.h"
#include "Gfx/Util/ShapeBuilder.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "shaders.h"

using namespace Oryol;

class SensorsApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const Sensors& sensor);

    GfxId drawState;
    glm::mat4 proj;
    TimePoint lastFrameTimePoint;
};
OryolMain(SensorsApp);

//------------------------------------------------------------------------------
AppState::Code
SensorsApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(800, 400, "Oryol Device Sensor Sample");
    gfxSetup.Loaders.Add(RawMeshLoader::Creator());
    Gfx::Setup(gfxSetup);
    Dbg::Setup();
    Input::Setup();
    
    // create a 3D cube
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.Box(2.0, 2.0, 2.0, 1).Build();
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
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.1f, 100.0f);
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
SensorsApp::computeMVP(const Sensors& sensors) {
    glm::mat4 model = glm::mat4();
    glm::mat4 att = glm::yawPitchRoll(sensors.Pitch, -sensors.Roll, 0.0f);
    glm::vec3 eye = glm::vec3(att[2]) * 6.0f;
    glm::vec3 up = glm::vec3(att[1]);
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f), up);
    return this->proj * view * model;
}

//------------------------------------------------------------------------------
AppState::Code
SensorsApp::OnRunning() {
    
    const Sensors& sensors = Input::Sensors();
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
    Gfx::ApplyDrawState(this->drawState);
    glm::mat4 mvp = this->computeMVP(sensors);
    Gfx::ApplyVariable(Shaders::Main::ModelViewProjection, mvp);
    Gfx::Draw(0);
    if (!Input::Sensors().Attached) {
        Dbg::Print("\n Please run on mobile device!\n\r");
    }
    else {
        Dbg::Print("\n Rotate device to see effect\n\r");
    }
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF(" yaw: %.3f, pitch: %.3f, roll: %.3f\n\r", sensors.Yaw, sensors.Pitch, sensors.Roll);
    Dbg::PrintF(" frame time: %.3fms", frameTime.AsMilliSeconds());
    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SensorsApp::OnCleanup() {
    this->drawState.Release();
    Input::Discard();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
