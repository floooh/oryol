//------------------------------------------------------------------------------
//  Instancing.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Debug/DebugFacade.h"
#include "Input/InputFacade.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Input;

// derived application class
class InstancingApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    void updateCamera();
    void emitParticles();
    void updateParticles();

    RenderFacade* render = nullptr;
    DebugFacade* debug = nullptr;
    InputFacade* input = nullptr;
    Id instanceMesh;
    Id drawState;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 modelViewProj;
    bool updateEnabled = true;
    int32 frameCount = 0;
    int32 curNumParticles = 0;
    TimePoint lastFrameTimePoint;
    static const int32 MaxNumParticles = 1024 * 1024;
    const int32 NumParticlesEmittedPerFrame = 100;
    glm::vec4 positions[MaxNumParticles];
    glm::vec4 vectors[MaxNumParticles];
};
OryolMain(InstancingApp);

//------------------------------------------------------------------------------
AppState::Code
InstancingApp::OnRunning() {
    
    Duration updTime, bufTime, drawTime;
    this->frameCount++;
    if (this->render->BeginFrame()) {
        
        // update block
        this->updateCamera();
        if (this->updateEnabled) {
            TimePoint updStart = Clock::Now();
            this->emitParticles();
            this->updateParticles();
            updTime = Clock::Since(updStart);

            TimePoint bufStart = Clock::Now();
            this->render->UpdateVertices(this->instanceMesh, this->curNumParticles * sizeof(glm::vec4), this->positions);
            bufTime = Clock::Since(bufStart);
        }
        
        // render block        
        TimePoint drawStart = Clock::Now();
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(PixelChannel::All, glm::vec4(0.0f), 1.0f, 0);
        this->render->ApplyDrawState(this->drawState);
        this->render->ApplyVariable(Shaders::Main::ModelViewProjection, this->modelViewProj);
        this->render->DrawInstanced(0, this->curNumParticles);
        drawTime = Clock::Since(drawStart);
        
        this->debug->DrawTextBuffer();
        this->render->EndFrame();
        
        // toggle particle update
        const Mouse& mouse = this->input->Mouse();
        if (mouse.Attached() && mouse.ButtonDown(Mouse::Button::LMB)) {
            this->updateEnabled = !this->updateEnabled;
        }
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    this->debug->PrintF("\n %d instances\n\r upd=%.3fms\n\r bufUpd=%.3fms\n\r draw=%.3fms\n\r frame=%.3fms\n\r"
                        " LMB/Tap: toggle particle updates",
                        this->curNumParticles,
                        updTime.AsMilliSeconds(),
                        bufTime.AsMilliSeconds(),
                        drawTime.AsMilliSeconds(),
                        frameTime.AsMilliSeconds());
    
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
void
InstancingApp::updateCamera() {
    float32 angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.5f, glm::cos(angle) * 10.0f);
    this->view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelViewProj = this->proj * this->view * this->model;
}

//------------------------------------------------------------------------------
void
InstancingApp::emitParticles() {
    for (int32 i = 0; i < NumParticlesEmittedPerFrame; i++) {
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
    const float32 frameTime = 1.0f / 60.0f;
    for (int32 i = 0; i < this->curNumParticles; i++) {
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
InstancingApp::OnInit() {
    // setup rendering system
    auto renderSetup = RenderSetup::AsWindow(800, 500, false, "Oryol Instancing Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    this->render = RenderFacade::CreateSingle(renderSetup);
    this->debug = DebugFacade::CreateSingle();
    this->input = InputFacade::CreateSingle();
    
    // check instancing extension
    if (!this->render->Supports(RenderFeature::Instancing)) {
        o_error("ERROR: instanced_arrays extension required!\n");
    }

    // create dynamic instance data mesh
    MeshSetup instanceMeshSetup = MeshSetup::CreateEmpty("inst", MaxNumParticles, Usage::Stream);
    instanceMeshSetup.Layout.Add(VertexAttr::Instance0, VertexFormat::Float4);
    this->instanceMesh = render->CreateResource(instanceMeshSetup);
    
    // setup static draw state
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetRandomColorsFlag(true);
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.SetTransform(glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    shapeBuilder.AddSphere(0.05f, 3, 2);
    shapeBuilder.Build();
    MeshSetup staticMeshSetup = MeshSetup::FromData("box");
    staticMeshSetup.InstanceMesh = this->instanceMesh;
    Id mesh = this->render->CreateResource(staticMeshSetup, shapeBuilder.GetStream());
    Id prog = this->render->CreateResource(Shaders::Main::CreateSetup());
    DrawStateSetup dsSetup("ds", mesh, prog, 0);
    dsSetup.RasterizerState.CullFaceEnabled = true;
    dsSetup.DepthStencilState.DepthWriteEnabled = true;
    dsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = this->render->CreateResource(dsSetup);
    
    this->render->ReleaseResource(mesh);
    this->render->ReleaseResource(prog);
    
    // setup projection and view matrices
    const float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    const float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->modelViewProj = this->proj * this->view * this->model;
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
InstancingApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->drawState);
    this->render->ReleaseResource(this->instanceMesh);
    this->input = nullptr;
    this->render = nullptr;
    this->debug = nullptr;
    InputFacade::DestroySingle();
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
