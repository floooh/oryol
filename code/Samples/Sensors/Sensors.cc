//------------------------------------------------------------------------------
//  Sensors.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/Time/Clock.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
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
    
    glm::mat4 computeMVP();

    DrawState drawState;
    Shader::vsParams vsParams;
    glm::mat4 proj;
    TimePoint lastFrameTimePoint;
};
OryolMain(SensorsApp);

//------------------------------------------------------------------------------
AppState::Code
SensorsApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(800, 400, "Oryol Device Sensor Sample"));
    Dbg::Setup();
    Input::Setup();
    
    // create a 3D cube
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Normal, VertexFormat::Byte4N }
    };
    shapeBuilder.Box(2.0, 2.0, 2.0, 1);
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    ps.RasterizerState.CullFaceEnabled = true;
    this->drawState.Pipeline = Gfx::CreateResource(ps);

    // setup transform matrices
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.1f, 100.0f);
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
SensorsApp::computeMVP() {
    glm::mat4 model = glm::mat4();
    const glm::vec3& ypr = Input::SensorYawPitchRoll();
    glm::mat4 att = glm::yawPitchRoll(ypr.y, -ypr.z, 0.0f);
    glm::vec3 eye = glm::vec3(att[2]) * 6.0f;
    glm::vec3 up = glm::vec3(att[1]);
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f), up);
    return this->proj * view * model;
}

//------------------------------------------------------------------------------
AppState::Code
SensorsApp::OnRunning() {
    
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    this->vsParams.mvp = this->computeMVP();
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::Draw();
    if (!Input::SensorsAttached()) {
        Dbg::Print("\n Please run on mobile device!\n\r");
    }
    else {
        Dbg::Print("\n Rotate device to see effect\n\r");
    }
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    const glm::vec3& ypr = Input::SensorYawPitchRoll();
    Dbg::PrintF(" yaw: %.3f, pitch: %.3f, roll: %.3f\n\r", ypr.x, ypr.y, ypr.z);
    Dbg::PrintF(" frame time: %.3fms", frameTime.AsMilliSeconds());
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SensorsApp::OnCleanup() {
    Input::Discard();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
