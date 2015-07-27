//------------------------------------------------------------------------------
//  GPUParticles.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Assets/Gfx/ShapeBuilder.h"
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

    Id particleBuffer[NumParticleBuffers];
    Id particleIdMesh;
    Id shapeMesh;
    Id initParticles;
    Id updateParticles;
    Id drawParticles;
    
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    int32 frameCount = 0;
    TimePoint lastFrameTimePoint;
    int32 curNumParticles = 0;

    Shaders::InitParticles::FSParams initFSParams;
    Shaders::UpdateParticles::FSParams updFSParams;
    Shaders::DrawParticles::VSParams drawVSParams;
    ClearState noClearState;
};
OryolMain(GPUParticlesApp);

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnRunning() {
    
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
    Gfx::ApplyRenderTarget(this->particleBuffer[drawIndex], this->noClearState);
    const int32 scissorHeight = (this->curNumParticles / NumParticlesX) + 1;
    Gfx::ApplyScissorRect(0, 0, ParticleBufferWidth, scissorHeight, Gfx::QueryFeature(GfxFeature::OriginTopLeft));
    Gfx::ApplyDrawState(this->updateParticles);
    this->updFSParams.NumParticles = (float32) this->curNumParticles;
    this->updFSParams.PrevState = this->particleBuffer[readIndex];
    Gfx::ApplyUniformBlock(this->updFSParams);
    Gfx::Draw(0);
    
    // now the actual particle shape rendering:
    // - the new particle state texture is sampled in the vertex shader to obtain particle positions
    // - draw 'curNumParticles' instances of the basic particle shape through hardware-instancing
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->drawParticles);
    this->drawVSParams.ParticleState = this->particleBuffer[drawIndex];
    Gfx::ApplyUniformBlock(this->drawVSParams);
    Gfx::DrawInstanced(0, this->curNumParticles);
    
    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();
    
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
    this->drawVSParams.ModelViewProjection = this->proj * this->view * this->model;
}

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxSetup::Window(800, 500, "Oryol GPU Particles Sample"));
    Dbg::Setup();

    // check required extensions
    if (!Gfx::QueryFeature(GfxFeature::TextureFloat)) {
        o_error("ERROR: float_texture extension required!\n");
    }
    if (!Gfx::QueryFeature(GfxFeature::Instancing)) {
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
    Id fullscreenMesh = Gfx::CreateResource(MeshSetup::FullScreenQuad(Gfx::QueryFeature(GfxFeature::OriginTopLeft)));
    
    // particle initialization and update draw states
    Id initShader = Gfx::CreateResource(Shaders::InitParticles::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndShader(fullscreenMesh, initShader);
    dss.BlendState.ColorFormat = particleBufferSetup.ColorFormat;
    dss.BlendState.DepthFormat = particleBufferSetup.DepthFormat;
    this->initParticles = Gfx::CreateResource(dss);
    Id updateShader = Gfx::CreateResource(Shaders::UpdateParticles::CreateSetup());
    dss.Shader = updateShader;
    dss.RasterizerState.ScissorTestEnabled = true;
    this->updateParticles = Gfx::CreateResource(dss);

    // a vertex buffer with the particleIds, this would not be needed if
    // ANGLE_instanced_arrays would support gl_InstanceID
    const int32 particleIdSize = MaxNumParticles * sizeof(float32);
    float32* particleIdData = (float32*) Memory::Alloc(particleIdSize);
    for (int32 i = 0; i < MaxNumParticles; i++) {
        particleIdData[i] = (float32) i;
    }
    auto particleIdSetup = MeshSetup::FromData(Usage::Immutable);
    particleIdSetup.NumVertices = MaxNumParticles;
    particleIdSetup.StepFunction = VertexStepFunction::PerInstance;
    particleIdSetup.StepRate = 1;
    particleIdSetup.Layout.Add(VertexAttr::Instance0, VertexFormat::Float);
    this->particleIdMesh = Gfx::CreateResource(particleIdSetup, particleIdData, particleIdSize);
    Memory::Free(particleIdData);
    
    // the geometry of a single particle
    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.RandomColors = true;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.Transform(rot90).Sphere(0.05f, 3, 2).Build();
    this->shapeMesh = Gfx::CreateResource(shapeBuilder.Result());
    
    // particle rendering draw state
    Id drawShader = Gfx::CreateResource(Shaders::DrawParticles::CreateSetup());
    dss = DrawStateSetup::FromMeshAndShader(this->shapeMesh, drawShader);
    dss.Meshes[1] = this->particleIdMesh;
    dss.RasterizerState.CullFaceEnabled = true;
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Less;
    this->drawParticles = Gfx::CreateResource(dss);

    // a ClearState which does not perform clear
    this->noClearState.Actions = ClearState::ClearNone;

    // the static projection matrix
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 50.0f);

    // setup initial shader params
    const glm::vec2 bufferDims(ParticleBufferWidth, ParticleBufferHeight);
    this->initFSParams.BufferDims = bufferDims;
    this->updFSParams.BufferDims = bufferDims;
    this->drawVSParams.BufferDims = bufferDims;

    // 'draw' the initial particle state (positions at origin, pseudo-random velocity)
    Gfx::ApplyRenderTarget(this->particleBuffer[0], this->noClearState);
    Gfx::ApplyDrawState(this->initParticles);
    Gfx::ApplyUniformBlock(this->initFSParams);
    Gfx::Draw(0);
    Gfx::ApplyRenderTarget(this->particleBuffer[1], this->noClearState);
    Gfx::ApplyDrawState(this->initParticles);
    Gfx::ApplyUniformBlock(this->initFSParams);
    Gfx::Draw(0);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
