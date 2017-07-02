//------------------------------------------------------------------------------
//  Instancing.cc
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

class InstancingApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
    
    void updateCamera();
    void emitParticles();
    void updateParticles();

    // the static geometry is at mesh slot 0, and the instance data at slot 1
    static const int geomMeshSlot = 0;
    static const int instMeshSlot = 1;

    DrawState drawState;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    Shader::vsParams vsParams;
    bool updateEnabled = true;
    int frameCount = 0;
    int curNumParticles = 0;
    TimePoint lastFrameTimePoint;
    static const int MaxNumParticles = 1024 * 1024;
    const int NumParticlesEmittedPerFrame = 100;
    glm::vec4 positions[MaxNumParticles];
    glm::vec4 vectors[MaxNumParticles];
};
OryolMain(InstancingApp);

//------------------------------------------------------------------------------
AppState::Code
InstancingApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxSetup::Window(800, 500, "Oryol Instancing Sample"));
    Dbg::Setup();
    Input::Setup();
    
    // check instancing extension
    if (!Gfx::QueryFeature(GfxFeature::Instancing)) {
        o_error("ERROR: instanced_arrays extension required!\n");
    }

    // create static mesh at mesh slot 0
    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.RandomColors = true;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Color0, VertexFormat::Float4 }
    };
    shapeBuilder.Transform(rot90).Sphere(0.05f, 3, 2);
    auto shapeBuilderResult = shapeBuilder.Build();
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilderResult);

    // create dynamic instance data mesh at mesh slot 1
    auto instMeshSetup = MeshSetup::Empty(MaxNumParticles, Usage::Stream);
    instMeshSetup.Layout
        .EnableInstancing()
        .Add(VertexAttr::Instance0, VertexFormat::Float4);
    this->drawState.Mesh[1] = Gfx::CreateResource(instMeshSetup);

    // setup draw state for instanced rendering
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto ps = PipelineSetup::FromShader(shd);
    ps.Layouts[0] = shapeBuilder.Layout;
    ps.Layouts[1] = instMeshSetup.Layout;
    ps.RasterizerState.CullFaceEnabled = true;
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState.Pipeline = Gfx::CreateResource(ps);
    
    // setup projection and view matrices
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
InstancingApp::OnRunning() {
    
    Duration updTime, bufTime, drawTime;
    this->frameCount++;
    
    // update block
    this->updateCamera();
    if (this->updateEnabled) {
        TimePoint updStart = Clock::Now();
        this->emitParticles();
        this->updateParticles();
        updTime = Clock::Since(updStart);

        TimePoint bufStart = Clock::Now();
        Gfx::UpdateVertices(this->drawState.Mesh[instMeshSlot], this->positions, this->curNumParticles * sizeof(glm::vec4));
        bufTime = Clock::Since(bufStart);
    }
    
    // render block        
    TimePoint drawStart = Clock::Now();
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::Draw(0, this->curNumParticles);
    drawTime = Clock::Since(drawStart);
    
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // toggle particle update
    if ((Input::MouseAttached() && Input::MouseButtonDown(MouseButton::Left)) ||
        (Input::TouchpadAttached() && Input::TouchTapped())) {
        this->updateEnabled = !this->updateEnabled;
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("\n %d instances\n\r upd=%.3fms\n\r bufUpd=%.3fms\n\r draw=%.3fms\n\r frame=%.3fms\n\r"
                " LMB/Tap: toggle particle updates",
                this->curNumParticles,
                updTime.AsMilliSeconds(),
                bufTime.AsMilliSeconds(),
                drawTime.AsMilliSeconds(),
                frameTime.AsMilliSeconds());
    
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
void
InstancingApp::updateCamera() {
    float angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.5f, glm::cos(angle) * 10.0f);
    this->view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->vsParams.mvp = this->proj * this->view * this->model;
}

//------------------------------------------------------------------------------
void
InstancingApp::emitParticles() {
    for (int i = 0; i < NumParticlesEmittedPerFrame; i++) {
        if (this->curNumParticles < MaxNumParticles) {
            this->positions[this->curNumParticles] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 rnd = glm::ballRand(0.5f);
            rnd.y += 2.0f;
            this->vectors[this->curNumParticles] = glm::vec4(rnd, 0.0f);
            this->curNumParticles++;
        }
    }
}

//------------------------------------------------------------------------------
void
InstancingApp::updateParticles() {
    const float frameTime = 1.0f / 60.0f;
    for (int i = 0; i < this->curNumParticles; i++) {
        auto& pos = this->positions[i];
        auto& vec = this->vectors[i];
        vec.y -= 1.0f * frameTime;
        pos += vec * frameTime;
        if (pos.y < -2.0f) {
            pos.y = -1.8f;
            vec.y = -vec.y;
            vec *= 0.8f;
        }
    }
}

//------------------------------------------------------------------------------
AppState::Code
InstancingApp::OnCleanup() {
    Input::Discard();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
