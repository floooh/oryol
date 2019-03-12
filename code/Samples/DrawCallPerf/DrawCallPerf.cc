//------------------------------------------------------------------------------
//  DrawCallPerf.cc
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
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;

class DrawCallPerfApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
    void updateCamera();
    void emitParticles();
    void updateParticles();

    PrimitiveGroup primGroup;
    Id pip;
    Bindings bind;
    Shader::perFrameParams perFrameParams;
    Shader::perParticleParams perParticleParams;
    bool updateEnabled = true;
    int frameCount = 0;
    int curNumParticles = 0;
    TimePoint lastFrameTimePoint;
    static const int NumParticlesEmittedPerFrame = 100;
    static const int MaxNumParticles = 1024 * 1024;
    struct {
        glm::vec4 pos;
        glm::vec4 vec;
    } particles[MaxNumParticles];
};
OryolMain(DrawCallPerfApp);

//------------------------------------------------------------------------------
AppState::Code
DrawCallPerfApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxDesc()
        .SetWidth(800)
        .SetHeight(500)
        .SetTitle("Oryol DrawCallPerf Sample")
        .SetGlobalUniformBufferSize(1024 * 1024 * 32)
        .SetHtmlTrackElementSize(true));
    Dbg::Setup();
    Input::Setup();

    // create resources
    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    auto shape = ShapeBuilder()
        .RandomColors(true)
        .Positions("in_pos", VertexFormat::Float3)
        .Colors("in_color", VertexFormat::Float4)
        .Transform(rot90)
        .Sphere(0.05f, 3, 2)
        .Build();
    this->primGroup = shape.PrimitiveGroups[0];
    this->bind.VertexBuffers[0] = Gfx::CreateBuffer(shape.VertexBufferDesc);
    this->bind.IndexBuffer = Gfx::CreateBuffer(shape.IndexBufferDesc);
    this->pip = Gfx::CreatePipeline(PipelineDesc(shape.PipelineDesc)
        .SetShader(Gfx::CreateShader(Shader::Desc()))
        .SetCullFaceEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetDepthCmpFunc(CompareFunc::LessEqual));
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DrawCallPerfApp::OnRunning() {
    
    Duration updTime, drawTime, applyRtTime;
    this->frameCount++;
    
    // update block
    this->updateCamera();
    if (this->updateEnabled) {
        TimePoint updStart = Clock::Now();
        this->emitParticles();
        this->updateParticles();
        updTime = Clock::Since(updStart);
    }
    
    // render block
    TimePoint applyRtStart = Clock::Now();
    Gfx::BeginPass();
    applyRtTime = Clock::Since(applyRtStart);
    TimePoint drawStart = Clock::Now();
    Gfx::ApplyPipeline(this->pip);
    Gfx::ApplyBindings(this->bind);
    Gfx::ApplyUniforms(this->perFrameParams);
    for (int i = 0; i < this->curNumParticles; i++) {
        this->perParticleParams.translate = this->particles[i].pos;
        Gfx::ApplyUniforms(this->perParticleParams);
        Gfx::Draw(this->primGroup);
    }
    drawTime = Clock::Since(drawStart);
    
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();

    // toggle particle update
    if (Input::MouseAttached() && Input::MouseButtonDown(MouseButton::Left)) {
        this->updateEnabled = !this->updateEnabled;
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::TextColor(1.0f, 1.0f, 0.0f, 1.0f);
    Dbg::PrintF("\n\n\n\n\n %d draws\n\r upd=%.3fms\n\r applyRt=%.3fms\n\r draw=%.3fms\n\r frame=%.3fms\n\r"
                " LMB/tap: toggle particle update",
                this->curNumParticles,
                updTime.AsMilliSeconds(),
                applyRtTime.AsMilliSeconds(),
                drawTime.AsMilliSeconds(),
                frameTime.AsMilliSeconds());
    Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
    Dbg::PrintF("\n\n\r NOTE: this demo will bring down GL fairly quickly!\n");
    
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
void
DrawCallPerfApp::updateCamera() {
    float angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.5f, glm::cos(angle) * 10.0f);
    glm::mat4 view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), float(Gfx::Width()), float(Gfx::Height()), 0.01f, 100.0f);
    this->perFrameParams.mvp = proj * view;
}

//------------------------------------------------------------------------------
void
DrawCallPerfApp::emitParticles() {
    for (int i = 0; i < NumParticlesEmittedPerFrame; i++) {
        if (this->curNumParticles < MaxNumParticles) {
            this->particles[this->curNumParticles].pos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 rnd = glm::ballRand(0.5f);
            rnd.y += 2.0f;
            this->particles[this->curNumParticles].vec = glm::vec4(rnd, 0.0f);
            this->curNumParticles++;
        }
    }
}

//------------------------------------------------------------------------------
void
DrawCallPerfApp::updateParticles() {
    const float frameTime = 1.0f / 60.0f;
    for (int i = 0; i < this->curNumParticles; i++) {
        auto& curParticle = this->particles[i];
        curParticle.vec.y -= 1.0f * frameTime;
        curParticle.pos += curParticle.vec * frameTime;
        if (curParticle.pos.y < -2.0f) {
            curParticle.pos.y = -1.8f;
            curParticle.vec.y = -curParticle.vec.y;
            curParticle.vec *= 0.8f;
        }
    }
}

//------------------------------------------------------------------------------
AppState::Code
DrawCallPerfApp::OnCleanup() {
    Dbg::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
