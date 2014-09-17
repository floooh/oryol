//------------------------------------------------------------------------------
//  GPUParticles.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Gfx/Util/RawMeshLoader.h"
#include "Gfx/Util/ShapeBuilder.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

const int32 NumParticleBuffers = 2;
const int32 NumParticlesEmittedPerFrame = 100;
const int32 NumParticlesX = 1024;
const int32 NumParticlesY = 1024;
const int32 MaxNumParticles = NumParticlesX * NumParticlesY;
const int32 ParticleBufferWidth = 2 * NumParticlesX;
const int32 ParticleBufferHeight = NumParticlesY;

class GPUParticlesApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    void updateCamera();

    GfxId particleBuffer[NumParticleBuffers];
    GfxId particleIdMesh;
    GfxId shapeMesh;
    GfxId initParticles;
    GfxId updateParticles;
    GfxId drawParticles;
    
    glm::vec2 particleBufferDims{ ParticleBufferWidth, ParticleBufferHeight };
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 modelViewProj;
    int32 frameCount = 0;
    TimePoint lastFrameTimePoint;
    int32 curNumParticles = 0;
};
OryolMain(GPUParticlesApp);

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnRunning() {
    // render one frame
    if (Gfx::BeginFrame()) {
        
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
        Gfx::ApplyOffscreenRenderTarget(this->particleBuffer[drawIndex]);
        const int32 scissorHeight = (this->curNumParticles / NumParticlesX) + 1;
        Gfx::ApplyScissorRect(0, 0, ParticleBufferWidth, scissorHeight);
        Gfx::ApplyDrawState(this->updateParticles);
        Gfx::ApplyVariable(Shaders::UpdateParticles::NumParticles, (float32) this->curNumParticles);
        Gfx::ApplyVariable(Shaders::UpdateParticles::BufferDims, this->particleBufferDims);
        Gfx::ApplyVariable(Shaders::UpdateParticles::PrevState, this->particleBuffer[readIndex]);
        Gfx::Draw(0);
        
        // now the actual particle shape rendering:
        // - the new particle state texture is sampled in the vertex shader to obtain particle positions
        // - draw 'curNumParticles' instances of the basic particle shape through hardware-instancing
        Gfx::ApplyDefaultRenderTarget();
        Gfx::Clear(PixelChannel::All, glm::vec4(0.0f), 1.0f, 0);
        Gfx::ApplyDrawState(this->drawParticles);
        Gfx::ApplyVariable(Shaders::DrawParticles::ModelViewProjection, this->modelViewProj);
        Gfx::ApplyVariable(Shaders::DrawParticles::BufferDims, this->particleBufferDims);
        Gfx::ApplyVariable(Shaders::DrawParticles::ParticleState, this->particleBuffer[drawIndex]);
        Gfx::DrawInstanced(0, this->curNumParticles);
        
        Dbg::DrawTextBuffer();
        Gfx::EndFrame();
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("\n %d instances\n\r frame=%.3fms", this->curNumParticles, frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
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
    auto gfxSetup = GfxSetup::Window(800, 500, false, "Oryol GPU Particles Sample");
    gfxSetup.Loaders.Add(RawMeshLoader::Creator());
    Gfx::Setup(gfxSetup);
    Dbg::Setup();

    // check required extensions
    if (!Gfx::Supports(GfxFeature::TextureFloat)) {
        o_error("ERROR: float_texture extension required!\n");
    }
    if (!Gfx::Supports(GfxFeature::Instancing)) {
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
    auto particleBufferSetup = TextureSetup::RenderTarget(ParticleBufferWidth, ParticleBufferHeight);
    particleBufferSetup.ColorFormat = PixelFormat::RGBA32F;
    particleBufferSetup.MinFilter = TextureFilterMode::Nearest;
    particleBufferSetup.MagFilter = TextureFilterMode::Nearest;
    this->particleBuffer[0] = Gfx::CreateResource(particleBufferSetup);
    particleBufferSetup.Locator = "pong";
    this->particleBuffer[1] = Gfx::CreateResource(particleBufferSetup);
    
    // a fullscreen mesh for the particle init- and update-shaders
    GfxId fullscreenMesh = Gfx::CreateResource(MeshSetup::FullScreenQuad());
    
    // particle initialization and update draw states
    GfxId initProg = Gfx::CreateResource(Shaders::InitParticles::CreateSetup());
    this->initParticles = Gfx::CreateResource(DrawStateSetup::FromMeshAndProg(fullscreenMesh, initProg));
    GfxId updateProg = Gfx::CreateResource(Shaders::UpdateParticles::CreateSetup());
    auto updateSetup = DrawStateSetup::FromMeshAndProg(fullscreenMesh, updateProg);
    updateSetup.RasterizerState.ScissorTestEnabled = true;
    this->updateParticles = Gfx::CreateResource(updateSetup);
    
    // a vertex buffer with the particleIds, this would not be needed if
    // ANGLE_instanced_arrays would support gl_InstanceID
    const int32 particleIdSize = MaxNumParticles * sizeof(float32);
    float32* particleIdData = (float32*) Memory::Alloc(particleIdSize);
    for (int32 i = 0; i < MaxNumParticles; i++) {
        particleIdData[i] = (float32) i;
    }
    auto particleIdSetup = MeshSetup::Empty(MaxNumParticles, Usage::Static);
    particleIdSetup.Layout.Add(VertexAttr::Instance0, VertexFormat::Float);
    this->particleIdMesh = Gfx::CreateResource(particleIdSetup);
    Gfx::UpdateVertices(this->particleIdMesh, particleIdSize, particleIdData);
    Memory::Free(particleIdData);
    
    // the geometry of a single particle
    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.RandomColors = true;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.Transform(rot90).Sphere(0.05f, 3, 2).Build();
    auto meshSetup = MeshSetup::FromStream();
    meshSetup.InstanceMesh = this->particleIdMesh;
    this->shapeMesh = Gfx::CreateResource(meshSetup, shapeBuilder.Result());
    
    // particle rendering draw state
    GfxId drawProg = Gfx::CreateResource(Shaders::DrawParticles::CreateSetup());
    auto drawSetup = DrawStateSetup::FromMeshAndProg(this->shapeMesh, drawProg);
    drawSetup.RasterizerState.CullFaceEnabled = true;
    drawSetup.DepthStencilState.DepthWriteEnabled = true;
    drawSetup.DepthStencilState.DepthCmpFunc = CompareFunc::Less;
    this->drawParticles = Gfx::CreateResource(drawSetup);
    
    // the static projection matrix
    const float32 fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 50.0f);
    
    // 'draw' the initial particle state (positions at origin, pseudo-random velocity)
    Gfx::ApplyOffscreenRenderTarget(this->particleBuffer[0]);
    Gfx::ApplyDrawState(this->initParticles);
    Gfx::ApplyVariable(Shaders::InitParticles::BufferDims, this->particleBufferDims);
    Gfx::Draw(0);
    Gfx::ApplyOffscreenRenderTarget(this->particleBuffer[1]);
    Gfx::ApplyDrawState(this->initParticles);
    Gfx::ApplyVariable(Shaders::InitParticles::BufferDims, this->particleBufferDims);
    Gfx::Draw(0);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnCleanup() {
    // cleanup everything
    this->particleBuffer[0].Release();
    this->particleBuffer[1].Release();
    this->particleIdMesh.Release();
    this->shapeMesh.Release();
    this->initParticles.Release();
    this->updateParticles.Release();
    this->drawParticles.Release();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
