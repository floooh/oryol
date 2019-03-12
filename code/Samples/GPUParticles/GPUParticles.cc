//------------------------------------------------------------------------------
//  GPUParticles.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/Time/Clock.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

const int NumParticleBuffers = 2;
const int NumParticlesEmittedPerFrame = 100;
const int NumParticlesX = 1024;
const int NumParticlesY = 1024;
const int MaxNumParticles = NumParticlesX * NumParticlesY;
const int ParticleBufferWidth = 2 * NumParticlesX;
const int ParticleBufferHeight = NumParticlesY;

class GPUParticlesApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
    
    void updateCamera();

    struct {
        Id texture;
        Id pass;
    } particleBuffer[NumParticleBuffers];
    Id initPipeline;
    Id updPipeline;
    Id drawPipeline;
    Bindings initBind;
    Bindings updBind;
    Bindings drawBind;

    PrimitiveGroup shapePrimGroup;
    int frameCount = 0;
    TimePoint lastFrameTimePoint;
    int curNumParticles = 0;

    InitShader::fsParams initFSParams;
    UpdateShader::fsParams updFSParams;
    DrawShader::vsParams drawVSParams;
};
OryolMain(GPUParticlesApp);

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxDesc()
        .SetWidth(800)
        .SetHeight(500)
        .SetTitle("Oryol GPU Particles Sample")
        .SetHtmlTrackElementSize(true));
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
    
    // the 2 ping/pong particle state textures and render passes
    for (int i = 0; i < 2; i++) {
        this->particleBuffer[i].texture = Gfx::CreateTexture(TextureDesc()
            .SetRenderTarget(true)
            .SetWidth(ParticleBufferWidth)
            .SetHeight(ParticleBufferHeight)
            .SetFormat(PixelFormat::RGBA32F)
            .SetMinFilter(TextureFilterMode::Nearest)
            .SetMagFilter(TextureFilterMode::Nearest));
        this->particleBuffer[i].pass = Gfx::CreatePass(PassDesc()
            .SetColorAttachment(0, this->particleBuffer[i].texture));
    }

    // a fullscreen mesh for the particle init- and update-shaders
    const float quadVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    Id quadVbuf = Gfx::CreateBuffer(BufferDesc()
        .SetSize(sizeof(quadVertices))
        .SetContent(quadVertices));
    this->initBind.VertexBuffers[0] = quadVbuf;
    this->updBind.VertexBuffers[0] = quadVbuf;

    // particle initialization and update resources
    PipelineDesc particlePipDesc = PipelineDesc()
        .SetLayout(0, { { "in_pos", VertexFormat::Float2 } })
        .SetPrimitiveType(PrimitiveType::TriangleStrip)
        .SetColorFormat(PixelFormat::RGBA32F)
        .SetDepthFormat(PixelFormat::None);
    this->initPipeline = Gfx::CreatePipeline(PipelineDesc(particlePipDesc)
        .SetShader(Gfx::CreateShader(InitShader::Desc())));
    this->updPipeline = Gfx::CreatePipeline(PipelineDesc(particlePipDesc)
        .SetShader(Gfx::CreateShader(UpdateShader::Desc())));

    // the static geometry of a single particle is at mesh slot 0
    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    auto shape = ShapeBuilder()
        .RandomColors(true)
        .Positions("in_pos", VertexFormat::Float3)
        .Colors("in_color", VertexFormat::Float4)
        .Transform(rot90)
        .Sphere(0.05f, 3, 2)
        .Build();
    this->shapePrimGroup = shape.PrimitiveGroups[0];
    this->drawBind.VertexBuffers[0] = Gfx::CreateBuffer(shape.VertexBufferDesc);
    this->drawBind.IndexBuffer = Gfx::CreateBuffer(shape.IndexBufferDesc);

    // a instancing vertex buffer with the particleIds at vertex buffer slot 1
    {
        const int particleIdSize = MaxNumParticles * sizeof(float);
        float* particleIdData = (float*) Memory::Alloc(particleIdSize);
        for (int i = 0; i < MaxNumParticles; i++) {
            particleIdData[i] = (float) i;
        }
        this->drawBind.VertexBuffers[1] = Gfx::CreateBuffer(BufferDesc()
            .SetSize(particleIdSize)
            .SetContent(particleIdData));
        Memory::Free(particleIdData);
    }

    // ...and the pipeline object for instanced particle rendering
    this->drawPipeline = Gfx::CreatePipeline(PipelineDesc(shape.PipelineDesc)
        .SetShader(Gfx::CreateShader(DrawShader::Desc()))
        .SetLayout(1, VertexLayout()
            .EnableInstancing()
            .Add("in_particleId", VertexFormat::Float))
        .SetCullFaceEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetDepthCmpFunc(CompareFunc::LessEqual));

    // setup initial shader params
    const glm::vec2 bufferDims(ParticleBufferWidth, ParticleBufferHeight);
    this->initFSParams.bufDims = bufferDims;
    this->updFSParams.bufDims = bufferDims;
    this->drawVSParams.bufDims = bufferDims;

    // 'draw' the initial particle state (positions at origin, pseudo-random velocity)
    for (int i = 0; i < 2; i++) {
        Gfx::BeginPass(this->particleBuffer[0].pass, PassAction().DontCare());
        Gfx::ApplyPipeline(this->initPipeline);
        Gfx::ApplyBindings(this->initBind);
        Gfx::ApplyUniforms(this->initFSParams);
        Gfx::Draw(0, 4);
        Gfx::EndPass();
    }

    return App::OnInit();
}

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
    const int readIndex = (this->frameCount + 1) % NumParticleBuffers;
    const int drawIndex = this->frameCount % NumParticleBuffers;
    
    // update particle state texture by rendering a fullscreen-quad:
    // - the previous and next particle state are stored in separate float textures
    // - the particle update shader reads the previous state and draws the next state
    // - we use a scissor rect around the currently active particles to make this update
    //   a bit more efficient
    const int scissorHeight = (this->curNumParticles / NumParticlesX) + 1;
    this->updBind.FSTexture[UpdateShader::prevState] = this->particleBuffer[readIndex].texture;
    this->updFSParams.numParticles = (float) this->curNumParticles;
    Gfx::BeginPass(this->particleBuffer[drawIndex].pass, PassAction().DontCare());
    Gfx::ApplyScissorRect(0, 0, ParticleBufferWidth, scissorHeight, Gfx::QueryFeature(GfxFeature::OriginTopLeft));
    Gfx::ApplyPipeline(this->updPipeline);
    Gfx::ApplyBindings(this->updBind);
    Gfx::ApplyUniforms(this->updFSParams);
    Gfx::Draw(0, 4);
    Gfx::EndPass();
    
    // now the actual particle shape rendering:
    // - the new particle state texture is sampled in the vertex shader to obtain particle positions
    // - draw 'curNumParticles' instances of the basic particle shape through hardware-instancing
    this->drawBind.VSTexture[DrawShader::particleTex] = this->particleBuffer[drawIndex].texture;
    Gfx::BeginPass();
    Gfx::ApplyPipeline(this->drawPipeline);
    Gfx::ApplyBindings(this->drawBind);
    Gfx::ApplyUniforms(this->drawVSParams);
    Gfx::Draw(this->shapePrimGroup, this->curNumParticles);
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("\n\n\n\n\n %d instances\n\r frame=%.3fms", this->curNumParticles, frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
void
GPUParticlesApp::updateCamera() {
    float angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.5f, glm::cos(angle) * 10.0f);
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), float(Gfx::Width()), float(Gfx::Height()), 0.01f, 50.0f);
    glm::mat4 view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->drawVSParams.mvp = proj * view;
}

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
