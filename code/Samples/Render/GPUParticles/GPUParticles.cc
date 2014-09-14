//------------------------------------------------------------------------------
//  GPUParticles.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/Render.h"
#include "Dbg/Dbg.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class GPUParticlesApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    void updateCamera();

    const int32 NumParticleBuffers = 2;
    const int32 NumParticlesEmittedPerFrame = 100;
    const int32 NumParticlesX = 1024;
    const int32 NumParticlesY = 1024;
    const int32 MaxNumParticles = NumParticlesX * NumParticlesY;
    const int32 ParticleBufferWidth = 2 * NumParticlesX;
    const int32 ParticleBufferHeight = NumParticlesY;
    
    Id particleBuffer[NumParticleBuffers];
    Id particleIdMesh;
    Id shapeMesh;
    Id initParticles;
    Id updateParticles;
    Id drawParticles;
    
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
    if (Render::BeginFrame()) {
        
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
        Render::ApplyOffscreenRenderTarget(this->particleBuffer[drawIndex]);
        const int32 scissorHeight = (this->curNumParticles / NumParticlesX) + 1;
        Render::ApplyScissorRect(0, 0, ParticleBufferWidth, scissorHeight);
        Render::ApplyDrawState(this->updateParticles);
        Render::ApplyVariable(Shaders::UpdateParticles::NumParticles, (float32) this->curNumParticles);
        Render::ApplyVariable(Shaders::UpdateParticles::BufferDims, this->particleBufferDims);
        Render::ApplyVariable(Shaders::UpdateParticles::PrevState, this->particleBuffer[readIndex]);
        Render::Draw(0);
        
        // now the actual particle shape rendering:
        // - the new particle state texture is sampled in the vertex shader to obtain particle positions
        // - draw 'curNumParticles' instances of the basic particle shape through hardware-instancing
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::All, glm::vec4(0.0f), 1.0f, 0);
        Render::ApplyDrawState(this->drawParticles);
        Render::ApplyVariable(Shaders::DrawParticles::ModelViewProjection, this->modelViewProj);
        Render::ApplyVariable(Shaders::DrawParticles::BufferDims, this->particleBufferDims);
        Render::ApplyVariable(Shaders::DrawParticles::ParticleState, this->particleBuffer[drawIndex]);
        Render::DrawInstanced(0, this->curNumParticles);
        
        Dbg::DrawTextBuffer();
        Render::EndFrame();
    }
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("\n %d instances\n\r frame=%.3fms", this->curNumParticles, frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
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
    auto renderSetup = RenderSetup::Window(800, 500, false, "Oryol GPU Particles Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    Render::Setup(renderSetup);
    Dbg::Setup();

    // check required extensions
    if (!Render::Supports(RenderFeature::TextureFloat)) {
        o_error("ERROR: float_texture extension required!\n");
    }
    if (!Render::Supports(RenderFeature::Instancing)) {
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
    this->particleBuffer[0] = Render::CreateResource(particleBufferSetup);
    particleBufferSetup.Locator = "pong";
    this->particleBuffer[1] = Render::CreateResource(particleBufferSetup);
    
    // a fullscreen mesh for the particle init- and update-shaders
    Id fullscreenMesh = Render::CreateResource(MeshSetup::FullScreenQuad());
    
    // particle initialization and update draw states
    Id initProg = Render::CreateResource(Shaders::InitParticles::CreateSetup());
    this->initParticles = Render::CreateResource(DrawStateSetup::FromMeshAndProg(fullscreenMesh, initProg));
    Id updateProg = Render::CreateResource(Shaders::UpdateParticles::CreateSetup());
    auto updateSetup = DrawStateSetup::FromMeshAndProg(fullscreenMesh, updateProg);
    updateSetup.RasterizerState.ScissorTestEnabled = true;
    this->updateParticles = Render::CreateResource(updateSetup);
    Render::ReleaseResource(fullscreenMesh);
    Render::ReleaseResource(initProg);
    Render::ReleaseResource(updateProg);
    
    // a vertex buffer with the particleIds, this would not be needed if
    // ANGLE_instanced_arrays would support gl_InstanceID
    const int32 particleIdSize = MaxNumParticles * sizeof(float32);
    float32* particleIdData = (float32*) Memory::Alloc(particleIdSize);
    for (int32 i = 0; i < MaxNumParticles; i++) {
        particleIdData[i] = (float32) i;
    }
    auto particleIdSetup = MeshSetup::Empty(MaxNumParticles, Usage::Static);
    particleIdSetup.Layout.Add(VertexAttr::Instance0, VertexFormat::Float);
    this->particleIdMesh = Render::CreateResource(particleIdSetup);
    Render::UpdateVertices(this->particleIdMesh, particleIdSize, particleIdData);
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
    this->shapeMesh = Render::CreateResource(meshSetup, shapeBuilder.Result());
    
    // particle rendering draw state
    Id drawProg = Render::CreateResource(Shaders::DrawParticles::CreateSetup());
    auto drawSetup = DrawStateSetup::FromMeshAndProg(this->shapeMesh, drawProg);
    drawSetup.RasterizerState.CullFaceEnabled = true;
    drawSetup.DepthStencilState.DepthWriteEnabled = true;
    drawSetup.DepthStencilState.DepthCmpFunc = CompareFunc::Less;
    this->drawParticles = Render::CreateResource(drawSetup);
    Render::ReleaseResource(drawProg);
    
    // the static projection matrix
    const float32 fbWidth = Render::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Render::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 50.0f);
    
    // 'draw' the initial particle state (positions at origin, pseudo-random velocity)
    Render::ApplyOffscreenRenderTarget(this->particleBuffer[0]);
    Render::ApplyDrawState(this->initParticles);
    Render::ApplyVariable(Shaders::InitParticles::BufferDims, this->particleBufferDims);
    Render::Draw(0);
    Render::ApplyOffscreenRenderTarget(this->particleBuffer[1]);
    Render::ApplyDrawState(this->initParticles);
    Render::ApplyVariable(Shaders::InitParticles::BufferDims, this->particleBufferDims);
    Render::Draw(0);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnCleanup() {
    // cleanup everything
    Render::ReleaseResource(this->particleBuffer[0]);
    Render::ReleaseResource(this->particleBuffer[1]);
    Render::ReleaseResource(this->particleIdMesh);
    Render::ReleaseResource(this->shapeMesh);
    Render::ReleaseResource(this->initParticles);
    Render::ReleaseResource(this->updateParticles);
    Render::ReleaseResource(this->drawParticles);
    Dbg::Discard();
    Render::Discard();
    return App::OnCleanup();
}
