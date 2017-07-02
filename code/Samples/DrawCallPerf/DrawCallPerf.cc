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

    DrawState drawState;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
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
    GfxSetup gfxSetup = GfxSetup::Window(800, 500, "Oryol DrawCallPerf Sample");
    gfxSetup.GlobalUniformBufferSize = 1024 * 1024 * 32;
    Gfx::Setup(gfxSetup);
    Dbg::Setup();
    Input::Setup();

    // create resources
    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.RandomColors = true;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Color0, VertexFormat::Float4 }
    };
    shapeBuilder.Transform(rot90).Sphere(0.05f, 3, 2);
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    ps.RasterizerState.CullFaceEnabled = true;
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState.Pipeline = Gfx::CreateResource(ps);
    
    // setup projection and view matrices
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 2.5f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->model = glm::mat4();
    
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
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->perFrameParams);
    for (int i = 0; i < this->curNumParticles; i++) {
        this->perParticleParams.translate = this->particles[i].pos;
        Gfx::ApplyUniformBlock(this->perParticleParams);
        Gfx::Draw();
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
    Dbg::PrintF("\n %d draws\n\r upd=%.3fms\n\r applyRt=%.3fms\n\r draw=%.3fms\n\r frame=%.3fms\n\r"
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
    this->view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->perFrameParams.mvp = this->proj * this->view * this->model;
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
