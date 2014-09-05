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
const int32 NumParticlesEmittedPerFrame = 100;
const int32 NumParticlesX = 1024;
const int32 NumParticlesY = 1024;
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
    Resource::Id initParticles;
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
GPUParticlesApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // increment frame count, update camera position
        this->frameCount++;
        this->updateCamera();
        
        // bump number of active particles
        this->curNumParticles += NumParticlesEmittedPerFrame;
        if (this->curNumParticles > MaxNumParticles) {
            this->curNumParticles = MaxNumParticles;
        }
        
        // ping and pong particle state buffer indices
        const int32 readIndex = (this->frameCount + 1) % NumParticleBuffers;
        const int32 drawIndex = this->frameCount % NumParticleBuffers;
        
        // update particle state texture by rendering a fullscreen-quad:
        // - the previous and next particle state are stored in separate float textures
        // - the particle update shader reads the previous state and draws the next state
        // - we use a scissor rect around the currently active particles to make this update
        //   a bit more efficient
        this->render->ApplyOffscreenRenderTarget(this->particleBuffer[drawIndex]);
        const int32 scissorHeight = (this->curNumParticles / NumParticlesX) + 1;
        this->render->ApplyScissorRect(0, 0, ParticleBufferWidth, scissorHeight);
        this->render->ApplyDrawState(this->updateParticles);
        this->render->ApplyVariable(Shaders::UpdateParticles::NumParticles, (float32) this->curNumParticles);
        this->render->ApplyVariable(Shaders::UpdateParticles::BufferDims, this->particleBufferDims);
        this->render->ApplyVariable(Shaders::UpdateParticles::PrevState, this->particleBuffer[readIndex]);
        this->render->Draw(0);
        
        // now the actual particle shape rendering:
        // - the new particle state texture is sampled in the vertex shader to obtain particle positions
        // - draw 'curNumParticles' instances of the basic particle shape through hardware-instancing
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::All, glm::vec4(0.0f), 1.0f, 0);
        this->render->ApplyDrawState(this->drawParticles);
        this->render->ApplyVariable(Shaders::DrawParticles::ModelViewProjection, this->modelViewProj);
        this->render->ApplyVariable(Shaders::DrawParticles::BufferDims, this->particleBufferDims);
        this->render->ApplyVariable(Shaders::DrawParticles::ParticleState, this->particleBuffer[drawIndex]);
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
void
GPUParticlesApp::updateCamera() {
    float32 angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.5f, glm::cos(angle) * 10.0f);
    this->view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelViewProj = this->proj * this->view * this->model;
}

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
    // - an particle-init shader which renders initial particle positions and velocity
    // - an update shader which reads previous particle state from ping texture and
    //   render new particle state to pong texture
    // - a static vertex buffer with per-instance particleIDs
    // - a static vertex buffer with the particle shape
    // - a particle-rendering draw state which uses instanced rendering
    // - 2 fullscreen-quad draw-states for emitting and updating particles
    // - 1 particle-rendering draw state
    
    // the 2 ping/pong particle state textures
    auto particleBufferSetup = TextureSetup::AsRenderTarget("ping", ParticleBufferWidth, ParticleBufferHeight);
    particleBufferSetup.ColorFormat = PixelFormat::RGBA32F;
    particleBufferSetup.MinFilter = TextureFilterMode::Nearest;
    particleBufferSetup.MagFilter = TextureFilterMode::Nearest;
    this->particleBuffer[0] = this->render->CreateResource(particleBufferSetup);
    particleBufferSetup.Locator = "pong";
    this->particleBuffer[1] = this->render->CreateResource(particleBufferSetup);
    
    // a fullscreen mesh for the particle init- and update-shaders
    Id fullscreenMesh = this->render->CreateResource(MeshSetup::CreateFullScreenQuad("fsMesh"));
    
    // particle initialization and update draw states
    Id initProg = this->render->CreateResource(Shaders::InitParticles::CreateSetup());
    DrawStateSetup initSetup("init", fullscreenMesh, initProg, 0);
    this->initParticles = this->render->CreateResource(initSetup);
    Id updateProg = this->render->CreateResource(Shaders::UpdateParticles::CreateSetup());
    DrawStateSetup updateSetup("update", fullscreenMesh, updateProg, 0);
    updateSetup.RasterizerState.ScissorTestEnabled = true;
    this->updateParticles = this->render->CreateResource(updateSetup);
    this->render->ReleaseResource(fullscreenMesh);
    this->render->ReleaseResource(initProg);
    this->render->ReleaseResource(updateProg);
    
    // a vertex buffer with the particleIds, this would not be needed if
    // ANGLE_instanced_arrays would support gl_InstanceID
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
    
    // the geometry of a single particle
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetRandomColorsFlag(true);
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.SetTransform(glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    shapeBuilder.AddSphere(0.05f, 3, 2);
    shapeBuilder.Build();
    MeshSetup meshSetup = MeshSetup::FromData("box");
    meshSetup.InstanceMesh = this->particleIdMesh;
    this->shapeMesh = this->render->CreateResource(meshSetup, shapeBuilder.GetStream());
    
    // particle rendering draw state
    Id drawProg = this->render->CreateResource(Shaders::DrawParticles::CreateSetup());
    DrawStateSetup drawSetup("draw", this->shapeMesh, drawProg, 0);
    drawSetup.RasterizerState.CullFaceEnabled = true;
    drawSetup.DepthStencilState.DepthWriteEnabled = true;
    drawSetup.DepthStencilState.DepthCmpFunc = CompareFunc::Less;
    this->drawParticles = this->render->CreateResource(drawSetup);
    this->render->ReleaseResource(drawProg);
    
    // the static projection matrix
    const float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    const float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 50.0f);
    
    // 'draw' the initial particle state (positions at origin, pseudo-random velocity)
    this->render->ApplyOffscreenRenderTarget(this->particleBuffer[0]);
    this->render->ApplyDrawState(this->initParticles);
    this->render->ApplyVariable(Shaders::InitParticles::BufferDims, this->particleBufferDims);
    this->render->Draw(0);
    this->render->ApplyOffscreenRenderTarget(this->particleBuffer[1]);
    this->render->ApplyDrawState(this->initParticles);
    this->render->ApplyVariable(Shaders::InitParticles::BufferDims, this->particleBufferDims);
    this->render->Draw(0);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->particleBuffer[0]);
    this->render->ReleaseResource(this->particleBuffer[1]);
    this->render->ReleaseResource(this->particleIdMesh);
    this->render->ReleaseResource(this->shapeMesh);
    this->render->ReleaseResource(this->initParticles);
    this->render->ReleaseResource(this->updateParticles);
    this->render->ReleaseResource(this->drawParticles);
    this->render = nullptr;
    this->debug = nullptr;
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
