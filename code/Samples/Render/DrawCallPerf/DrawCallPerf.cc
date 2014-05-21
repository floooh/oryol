//------------------------------------------------------------------------------
//  DrawCallPerf.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include <chrono>
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;

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

    RenderFacade* render = nullptr;
    Resource::Id meshId;
    Resource::Id progId;
    Resource::Id stateId;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 modelViewProj;
    int32 frameCount = 0;
    int32 curNumParticles = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTimePoint;
    static const int32 MaxNumParticles = 1<<16;
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
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(800, 500, "Oryol DrawCallPerf Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());

    // create a small cube shape
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetRandomColorsFlag(true);
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.AddBox(0.05f, 0.05f, 0.05f, 1);
    shapeBuilder.Build();
    this->meshId = this->render->CreateResource(MeshSetup::FromData("box"), shapeBuilder.GetStream());

    // build a shader program from vs/fs sources
    this->progId = this->render->CreateResource(Shaders::Main::CreateSetup());
    
    // setup state block object
    StateBlockSetup stateSetup("state");
    stateSetup.AddState(Render::State::DepthMask, true);
    stateSetup.AddState(Render::State::DepthTestEnabled, true);
    stateSetup.AddState(Render::State::DepthFunc, Render::State::LessEqual);
    stateSetup.AddState(Render::State::ClearDepth, 1.0f);
    stateSetup.AddState(Render::State::ClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
    this->stateId = this->render->CreateResource(stateSetup);
    
    // setup projection and view matrices
    const float32 fbWidth = this->render->GetDisplayAttrs().GetFramebufferWidth();
    const float32 fbHeight = this->render->GetDisplayAttrs().GetFramebufferHeight();
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 2.5f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->model = glm::mat4();
    this->modelViewProj = this->proj * this->view * this->model;
    
    return App::OnInit();
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
    if (this->curNumParticles < MaxNumParticles) {
        this->particles[this->curNumParticles].pos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 rnd = glm::ballRand(0.5f);
        rnd.y += 2.0f;
        this->particles[this->curNumParticles].vec = glm::vec4(rnd, 0.0f);
        this->curNumParticles++;
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
DrawCallPerfApp::OnRunning() {
    
    // FIXME: that chrono code is ugly, create our own wrapper Time module?
    
    using namespace std;
    
    chrono::microseconds updTime;
    chrono::microseconds drawTime;
    this->frameCount++;
    if (this->render->BeginFrame()) {
        
        // update block
        auto updStart = chrono::high_resolution_clock::now();
        this->updateCamera();
        this->emitParticles();
        this->updateParticles();
        updTime = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - updStart);
        
        // render block
        this->render->ApplyStateBlock(this->stateId);
        this->render->Clear(true, true, true);
        this->render->ApplyMesh(this->meshId);
        this->render->ApplyProgram(this->progId, 0);
        this->render->ApplyVariable(Shaders::Main::ModelViewProjection, this->modelViewProj);

        auto drawStart = chrono::high_resolution_clock::now();
        for (int32 i = 0; i < this->curNumParticles; i++) {
            this->render->ApplyVariable(Shaders::Main::ParticleTranslate, this->particles[i].pos);
            this->render->Draw(0);
        }
        drawTime = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - drawStart);
        this->render->EndFrame();
    }
    
    auto curTime = chrono::high_resolution_clock::now();
    auto frameTime = chrono::duration_cast<chrono::microseconds>(curTime - this->lastFrameTimePoint);
    this->lastFrameTimePoint = curTime;
    
    if (0 == (this->curNumParticles % 200)) {
        Log::Info("%d draws: upd=%lld draw=%lld, frame=%lld microseconds\n",
                  this->curNumParticles,
                  updTime.count(),
                  drawTime.count(),
                  frameTime.count());
    }
    
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DrawCallPerfApp::OnCleanup() {
    // cleanup everything
    this->render->DiscardResource(this->stateId);
    this->render->DiscardResource(this->progId);
    this->render->DiscardResource(this->meshId);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
