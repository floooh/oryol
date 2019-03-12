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

    PrimitiveGroup primGroup;
    Id pip;
    Bindings bind;
    Shader::vsParams vsParams;
    TimePoint lastFrameTimePoint;
};
OryolMain(SensorsApp);

//------------------------------------------------------------------------------
AppState::Code
SensorsApp::OnInit() {
    Gfx::Setup(GfxDesc()
        .SetWidth(800)
        .SetHeight(400)
        .SetTitle("Oryol Device Sensor Sample")
        .SetHtmlTrackElementSize(true));
    Dbg::Setup();
    Input::Setup();
    
    // create a 3D cube
    auto shape = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .Normals("in_normal", VertexFormat::Byte4N)
        .Box(2.0, 2.0, 2.0, 1)
        .Build();
    this->primGroup = shape.PrimitiveGroups[0];
    this->bind.VertexBuffers[0] = Gfx::CreateBuffer(shape.VertexBufferDesc);
    this->bind.IndexBuffer = Gfx::CreateBuffer(shape.IndexBufferDesc);
    this->pip = Gfx::CreatePipeline(PipelineDesc(shape.PipelineDesc)
        .SetShader(Gfx::CreateShader(Shader::Desc()))
        .SetDepthWriteEnabled(true)
        .SetDepthCmpFunc(CompareFunc::LessEqual)
        .SetCullFaceEnabled(true));
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
SensorsApp::computeMVP() {
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), float(Gfx::Width()), float(Gfx::Height()), 0.1f, 100.0f);
    const glm::vec3& ypr = Input::SensorYawPitchRoll();
    glm::mat4 att = glm::yawPitchRoll(ypr.y, -ypr.z, 0.0f);
    glm::vec3 eye = glm::vec3(att[2]) * 6.0f;
    glm::vec3 up = glm::vec3(att[1]);
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f), up);
    return proj * view;
}

//------------------------------------------------------------------------------
AppState::Code
SensorsApp::OnRunning() {
    
    Gfx::BeginPass();
    Gfx::ApplyPipeline(this->pip);
    Gfx::ApplyBindings(this->bind);
    this->vsParams.mvp = this->computeMVP();
    Gfx::ApplyUniforms(this->vsParams);
    Gfx::Draw(this->primGroup);
    Dbg::Print("\n\n\n\n");
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
