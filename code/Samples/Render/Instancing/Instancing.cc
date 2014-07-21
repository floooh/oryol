//------------------------------------------------------------------------------
//  Instancing.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Debug/DebugFacade.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;
using namespace Oryol::Time;
using namespace Oryol::Debug;

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
    Resource::Id instanceMesh;
    Resource::Id drawState;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 modelViewProj;
    int32 frameCount = 0;
    int32 curNumParticles = 0;
    TimePoint lastFrameTimePoint;
    static const int32 MaxNumParticles = 1<<20;
    glm::vec4 positions[MaxNumParticles];
    glm::vec4 vectors[MaxNumParticles];
};
OryolMain(InstancingApp);

//------------------------------------------------------------------------------
AppState::Code
InstancingApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(800, 500, "Oryol Instancing Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    this->debug = DebugFacade::CreateSingle();
    
    // check instancing extension
    if (!this->render->Supports(Feature::Instancing)) {
        o_error("ERROR: instanced_arrays extension required!\n");
    }

    // create dynamic instance data mesh
    MeshSetup instanceMeshSetup = MeshSetup::CreateEmpty("inst", MaxNumParticles, Usage::Dynamic);
    instanceMeshSetup.Layout.Add(VertexAttr::Instance0, VertexFormat::Float4);
    this->instanceMesh = render->CreateResource(instanceMeshSetup);
    
    // setup static draw state
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetRandomColorsFlag(true);
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.AddBox(0.05f, 0.05f, 0.05f, 1);
    shapeBuilder.Build();
    MeshSetup staticMeshSetup = MeshSetup::FromData("box");
    staticMeshSetup.InstanceMesh = this->instanceMesh;
    Id mesh = this->render->CreateResource(staticMeshSetup, shapeBuilder.GetStream());
    Id prog = this->render->CreateResource(Shaders::Main::CreateSetup());
    DrawStateSetup dsSetup("ds", mesh, prog, 0);
    dsSetup.DepthStencilState.SetDepthWriteEnabled(true);
    dsSetup.DepthStencilState.SetDepthCompareFunc(CompareFunc::LessEqual);
    this->drawState = this->render->CreateResource(dsSetup);
    
    this->render->ReleaseResource(mesh);
    this->render->ReleaseResource(prog);
    
    // setup projection and view matrices
    const float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    const float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 2.5f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->model = glm::mat4();
    this->modelViewProj = this->proj * this->view * this->model;
    
    return App::OnInit();
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
    for (int32 i = 0; i < 10; i++) {
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
InstancingApp::OnRunning() {
    
    Duration updTime, drawTime;
    this->frameCount++;
    if (this->render->BeginFrame()) {
        
        // update block
        TimePoint updStart = Clock::Now();
        this->updateCamera();
        this->emitParticles();
        this->updateParticles();
        updTime = Clock::Since(updStart);
        
        // render block
        TimePoint drawStart = Clock::Now();
        this->render->UpdateVertices(this->instanceMesh, this->curNumParticles * sizeof(glm::vec4), this->positions);
        this->render->ApplyDefaultRenderTarget();
        this->render->ApplyDrawState(this->drawState);
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyState(Render::State::ClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
        this->render->Clear(true, true, true);
        this->render->ApplyVariable(Shaders::Main::ModelViewProjection, this->modelViewProj);
        this->render->DrawInstanced(0, this->curNumParticles);
        drawTime = Clock::Since(drawStart);
        
        this->debug->DrawTextBuffer();
        this->render->EndFrame();
    }
    
    TimePoint curTime = Clock::Now();
    Duration frameTime = curTime - this->lastFrameTimePoint;
    this->lastFrameTimePoint = curTime;
    
    this->debug->PrintF("\n %d instances\n\r upd=%.3fms\n\r draw=%.3fms\n\r frame=%.3fms",
                        this->curNumParticles,
                        updTime.AsMilliSeconds(),
                        drawTime.AsMilliSeconds(),
                        frameTime.AsMilliSeconds());
    
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
InstancingApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->drawState);
    this->render->ReleaseResource(this->instanceMesh);
    this->render = nullptr;
    this->debug = nullptr;
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
