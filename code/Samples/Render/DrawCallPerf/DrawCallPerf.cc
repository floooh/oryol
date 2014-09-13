//------------------------------------------------------------------------------
//  DrawCallPerf.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/Render.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Dbg/Dbg.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;

// derived application class
class DrawCallPerfApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    void updateCamera();
    void emitParticles();
    void updateParticles();

    Id drawState;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 modelViewProj;
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
    if (Render::BeginFrame()) {
        
        // update block
        TimePoint updStart = Clock::Now();
        this->updateCamera();
        this->emitParticles();
        this->updateParticles();
        updTime = Clock::Since(updStart);
        
        // render block
        TimePoint drawStart = Clock::Now();
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::All, glm::vec4(0.0f), 1.0f, 0);
        Render::ApplyDrawState(this->drawState);
        Render::ApplyVariable(Shaders::Main::ModelViewProjection, this->modelViewProj);
        for (int32 i = 0; i < this->curNumParticles; i++) {
            Render::ApplyVariable(Shaders::Main::ParticleTranslate, this->particles[i].pos);
            Render::Draw(0);
        }
        drawTime = Clock::Since(drawStart);
        
        Dbg::DrawTextBuffer();
        Render::EndFrame();
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    Dbg::PrintF("\n %d draws\n\r upd=%.3fms\n\r draw=%.3fms\n\r frame=%.3fms\n\r",
                this->curNumParticles,
                updTime.AsMilliSeconds(),
                drawTime.AsMilliSeconds(),
                frameTime.AsMilliSeconds());
    Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    Dbg::PrintF("\n NOTE: this demo will bring down GL fairly quickly!\n");
    
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
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
    auto renderSetup = RenderSetup::AsWindow(800, 500, false, "Oryol DrawCallPerf Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    Render::Setup(renderSetup);
    Dbg::Setup();

    // create resources
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetRandomColorsFlag(true);
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.SetTransform(glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    shapeBuilder.AddSphere(0.05f, 3, 2);
    shapeBuilder.Build();
    Id mesh = Render::CreateResource(MeshSetup::FromData("box"), shapeBuilder.GetStream());
    Id prog = Render::CreateResource(Shaders::Main::CreateSetup());
    DrawStateSetup dsSetup("ds", mesh, prog, 0);
    dsSetup.RasterizerState.CullFaceEnabled = true;
    dsSetup.DepthStencilState.DepthWriteEnabled = true;
    dsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = Render::CreateResource(dsSetup);
    
    Render::ReleaseResource(mesh);
    Render::ReleaseResource(prog);
    
    // setup projection and view matrices
    const float32 fbWidth = Render::GetDisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Render::GetDisplayAttrs().FramebufferHeight;
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
    Render::ReleaseResource(this->drawState);
    Dbg::Discard();
    Render::Discard();
    return App::OnCleanup();
}
