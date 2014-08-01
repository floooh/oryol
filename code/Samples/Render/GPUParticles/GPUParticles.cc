//------------------------------------------------------------------------------
//  GPUParticles.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Debug/DebugFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;
using namespace Oryol::Debug;
using namespace Oryol::Time;

const int32 NumParticleBuffers = 2;
const int32 NumParticlesEmittedPerFrame = 16;
const int32 NumParticlesX = 512;
const int32 NumParticlesY = 512;
const int32 MaxNumParticles = NumParticlesX * NumParticlesY;
const int32 ParticleBufferWidth = 2 * NumParticlesX;
const int32 ParticleBufferHeight = NumParticlesY;

class GPUParticlesApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    void updateCamera();

    RenderFacade* render = nullptr;
    DebugFacade* debug = nullptr;
    
    Resource::Id particleBuffer[NumParticleBuffers];
    Resource::Id particleIdMesh;
    Resource::Id shapeMesh;
    Resource::Id emitParticles;
    Resource::Id updateParticles;
    Resource::Id drawParticles;
    
    glm::vec2 particleBufferDims{ ParticleBufferWidth, ParticleBufferHeight };
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 modelViewProj;
    int32 frameCount = 0;
    Time::TimePoint lastFrameTimePoint;
    int32 curNumParticles = 0;
};
OryolMain(GPUParticlesApp);

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(800, 500, "Oryol GPU Particles Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    this->debug = DebugFacade::CreateSingle();

    // check required extensions
    if (!this->render->Supports(Feature::TextureFloat)) {
        o_error("ERROR: float_texture extension required!\n");
    }
    if (!this->render->Supports(Feature::Instancing)) {
        o_error("ERROR: instances_arrays extension required!\n");
    }
    
    // what we need:
    // - 2 ping/pong particle state float textures which keep track of the persistent particle state (pos and velocity)
    // - an emitter shader which renders new particle state
    // - an update shader which reads previous particle state from ping texture and
    //   render new particle state to pong texture
    // - a static vertex buffer with per-instance particleIDs
    // - a static vertex buffer with the particle shape
    // - a particle-rendering draw state which uses instanced rendering
    // - 2 fullscreen-quad draw-states for emitting and updating particles
    // - 1 particle-rendering draw state

    // create 2 ping/pong render target textures to hold particle state
    // after creation, fill the state render targets with all zeros
    auto particleBufferSetup = TextureSetup::AsRenderTarget("ping", ParticleBufferWidth, ParticleBufferHeight, PixelFormat::RGBA32F);
    particleBufferSetup.MinFilter = TextureFilterMode::Nearest;
    particleBufferSetup.MagFilter = TextureFilterMode::Nearest;
    this->particleBuffer[0] = this->render->CreateResource(particleBufferSetup);
    particleBufferSetup.Locator = "pong";
    this->particleBuffer[1] = this->render->CreateResource(particleBufferSetup);
    
    // create a particleId buffer which is used as instance data buffer with particleIds from 0..MaxNumParticles
    const int32 particleIdSize = MaxNumParticles * sizeof(float32);
    float32* particleIdData = (float32*) Memory::Alloc(particleIdSize);
    for (int32 i = 0; i < MaxNumParticles; i++) {
        particleIdData[i] = (float32) i;
    }
    auto particleIdSetup = MeshSetup::CreateEmpty("particleId", MaxNumParticles, Usage::Static);
    particleIdSetup.Layout.Add(VertexAttr::Instance0, VertexFormat::Float);
    this->particleIdMesh = this->render->CreateResource(particleIdSetup);
    this->render->UpdateVertices(this->particleIdMesh, particleIdSize, particleIdData);
    Memory::Free(particleIdData);
    
    // create geometry mesh with particleId mesh providing the instancing data
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetRandomColorsFlag(true);
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.AddSphere(0.075f, 3, 2);
    shapeBuilder.Build();
    MeshSetup meshSetup = MeshSetup::FromData("box");
    meshSetup.InstanceMesh = this->particleIdMesh;
    this->shapeMesh = this->render->CreateResource(meshSetup, shapeBuilder.GetStream());
    
    // fullscreen mesh, we'll reuse this several times
    Id fullscreenMesh = this->render->CreateResource(MeshSetup::CreateFullScreenQuad("fsMesh"));
    
    // create particle emission and update draw states
    Id emitProg = this->render->CreateResource(Shaders::EmitParticles::CreateSetup());
    DrawStateSetup emitterSetup("emit", fullscreenMesh, emitProg, 0);
    this->emitParticles = this->render->CreateResource(emitterSetup);
    Id updateProg = this->render->CreateResource(Shaders::UpdateParticles::CreateSetup());
    DrawStateSetup updateSetup("update", fullscreenMesh, updateProg, 0);
    updateSetup.RasterizerState.ScissorTestEnabled = true;
    this->updateParticles = this->render->CreateResource(updateSetup);
    this->render->ReleaseResource(fullscreenMesh);
    this->render->ReleaseResource(emitProg);
    this->render->ReleaseResource(updateProg);
    
    // create particle rendering draw state
    Id drawProg = this->render->CreateResource(Shaders::DrawParticles::CreateSetup());
    DrawStateSetup drawSetup("draw", this->shapeMesh, drawProg, 0);
    drawSetup.DepthStencilState.DepthWriteEnabled = true;
    drawSetup.DepthStencilState.DepthCmpFunc = CompareFunc::Less;
    this->drawParticles = this->render->CreateResource(drawSetup);
    this->render->ReleaseResource(drawProg);
    
    // setup static transform matrices
    const float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    const float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 50.0f);
    this->view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 5.0f));
    this->model = glm::mat4();
    
    // initialize particle state
    this->render->ApplyOffscreenRenderTarget(this->particleBuffer[0]);
    this->render->Clear(Channel::RGBA, glm::vec4(0.0f), 1.0f, 0);
    this->render->ApplyDrawState(this->emitParticles);
    this->render->ApplyVariable(Shaders::EmitParticles::BufferDims, this->particleBufferDims);
    this->render->Draw(0);
    this->render->ApplyOffscreenRenderTarget(this->particleBuffer[1]);
    this->render->Clear(Channel::RGBA, glm::vec4(0.0f), 1.0f, 0);
    this->render->ApplyDrawState(this->emitParticles);
    this->render->ApplyVariable(Shaders::EmitParticles::BufferDims, this->particleBufferDims);
    this->render->Draw(0);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
void
GPUParticlesApp::updateCamera() {
    float32 angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.5f, glm::cos(angle) * 10.0f);
    this->view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelViewProj = this->proj * this->view * this->model;
}

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        this->frameCount++;
        const int32 curStateBufferIndex = (this->frameCount + 1) % NumParticleBuffers;
        const int32 prevStateBufferIndex = this->frameCount % NumParticleBuffers;
        this->curNumParticles += NumParticlesEmittedPerFrame;
        if (this->curNumParticles > MaxNumParticles) {
            this->curNumParticles = MaxNumParticles;
        }
        this->updateCamera();

        // update particle state (fullscreen-quad with ping/pong texture)
        this->render->ApplyOffscreenRenderTarget(this->particleBuffer[curStateBufferIndex]);

        // scissor rect to enclose the number of active particles to save some fillrate
        const int32 scissorWidth = ParticleBufferWidth;
        const int32 scissorHeight = (this->curNumParticles / ParticleBufferWidth) + 1;
        this->render->ApplyScissorRect(0, 0, scissorWidth, scissorHeight);
        
        // update particle state by rendering a (scissored) fullscreen quad
        this->render->ApplyDrawState(this->updateParticles);
        this->render->ApplyVariable(Shaders::UpdateParticles::NumParticles, (float32) this->curNumParticles);
        this->render->ApplyVariable(Shaders::UpdateParticles::BufferDims, this->particleBufferDims);
        this->render->ApplyVariable(Shaders::UpdateParticles::PrevState, this->particleBuffer[prevStateBufferIndex]);
        this->render->Draw(0);
        
        // render particles through instanced rendering
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::All, glm::vec4(0.0f), 1.0f, 0);
        this->render->ApplyDrawState(this->drawParticles);
        this->render->ApplyVariable(Shaders::DrawParticles::ModelViewProjection, this->modelViewProj);
        this->render->ApplyVariable(Shaders::DrawParticles::BufferDims, this->particleBufferDims);
        this->render->ApplyVariable(Shaders::DrawParticles::ParticleState, this->particleBuffer[curStateBufferIndex]);
        this->render->DrawInstanced(0, this->curNumParticles);
        
        this->debug->DrawTextBuffer();
        this->render->EndFrame();
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    this->debug->PrintF("\n %d instances\n\r frame=%.3fms",
                        this->curNumParticles,
                        frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->particleBuffer[0]);
    this->render->ReleaseResource(this->particleBuffer[1]);
    this->render->ReleaseResource(this->particleIdMesh);
    this->render->ReleaseResource(this->shapeMesh);
    this->render->ReleaseResource(this->emitParticles);
    this->render->ReleaseResource(this->updateParticles);
    this->render->ReleaseResource(this->drawParticles);
    this->render = nullptr;
    this->debug = nullptr;
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
