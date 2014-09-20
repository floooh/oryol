//------------------------------------------------------------------------------
//  DrawCallPerf.cc
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
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;

class DrawCallPerfApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    void updateCamera();
    void emitParticles();
    void updateParticles();

    GfxId drawState;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 modelViewProj;
    bool updateEnabled = true;    
    int32 frameCount = 0;
    int32 curNumParticles = 0;
    TimePoint lastFrameTimePoint;
    static const int32 NumParticlesEmittedPerFrame = 100;
    static const int32 MaxNumParticles = 1024 * 1024;
    struct {
        glm::vec4 pos;
        glm::vec4 vec;
    } particles[MaxNumParticles];
};
OryolMain(DrawCallPerfApp);

//------------------------------------------------------------------------------
AppState::Code
DrawCallPerfApp::OnRunning() {
    
    Duration updTime, drawTime;
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
    TimePoint drawStart = Clock::Now();
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.0f));
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyVariable(Shaders::Main::ModelViewProjection, this->modelViewProj);
    for (int32 i = 0; i < this->curNumParticles; i++) {
        Gfx::ApplyVariable(Shaders::Main::ParticleTranslate, this->particles[i].pos);
        Gfx::Draw(0);
    }
    drawTime = Clock::Since(drawStart);
    
    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();

    // toggle particle update
    const Mouse& mouse = Input::Mouse();
    if (mouse.Attached() && mouse.ButtonDown(Mouse::Button::LMB)) {
        this->updateEnabled = !this->updateEnabled;
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    Dbg::PrintF("\n %d draws\n\r upd=%.3fms\n\r draw=%.3fms\n\r frame=%.3fms\n\r"
                " LMB/tap: toggle particle update",
                this->curNumParticles,
                updTime.AsMilliSeconds(),
                drawTime.AsMilliSeconds(),
                frameTime.AsMilliSeconds());
    Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    Dbg::PrintF("\n\n\r NOTE: this demo will bring down GL fairly quickly!\n");
    
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
void
DrawCallPerfApp::updateCamera() {
    float32 angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.5f, glm::cos(angle) * 10.0f);
    this->view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelViewProj = this->proj * this->view * this->model;
}

//------------------------------------------------------------------------------
void
DrawCallPerfApp::emitParticles() {
    for (int32 i = 0; i < NumParticlesEmittedPerFrame; i++) {
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
    const float32 frameTime = 1.0f / 60.0f;
    for (int32 i = 0; i < this->curNumParticles; i++) {
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
DrawCallPerfApp::OnInit() {
    // setup rendering system
    auto gfxSetup = GfxSetup::Window(800, 500, "Oryol DrawCallPerf Sample");
    gfxSetup.Loaders.Add(RawMeshLoader::Creator());
    Gfx::Setup(gfxSetup);
    Dbg::Setup();
    Input::Setup();

    // create resources
    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.RandomColors = true;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.Transform(rot90).Sphere(0.05f, 3, 2).Build();
    GfxId mesh = Gfx::CreateResource(MeshSetup::FromStream(), shapeBuilder.Result());
    GfxId prog = Gfx::CreateResource(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(mesh, prog);
    dss.RasterizerState.CullFaceEnabled = true;
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = Gfx::CreateResource(dss);
    
    // setup projection and view matrices
    const float32 fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 2.5f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->model = glm::mat4();
    this->modelViewProj = this->proj * this->view * this->model;
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DrawCallPerfApp::OnCleanup() {
    // cleanup everything
    this->drawState.Release();
    Dbg::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
