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
    DrawState initParticles;
    DrawState updParticles;
    DrawState drawParticles;

    PrimitiveGroup shapePrimGroup;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
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
    Gfx::Setup(GfxDesc::Window(800, 500, "Oryol GPU Particles Sample"));
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
        this->particleBuffer[i].texture = Gfx::Texture()
            .RenderTarget(true)
            .Width(ParticleBufferWidth)
            .Height(ParticleBufferHeight)
            .Format(PixelFormat::RGBA32F)
            .MinFilter(TextureFilterMode::Nearest)
            .MagFilter(TextureFilterMode::Nearest)
            .Create();
        this->particleBuffer[i].pass = Gfx::Pass()
            .ColorAttachment(0, this->particleBuffer[i].texture)
            .Create();
    }

    // a fullscreen mesh for the particle init- and update-shaders
    const float quadVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    Id quadVbuf = Gfx::Buffer()
        .Size(sizeof(quadVertices))
        .Content(quadVertices)
        .Create();
    this->initParticles.VertexBuffers[0] = quadVbuf;
    this->updParticles.VertexBuffers[0] = quadVbuf;

    // particle initialization and update resources
    auto particlePipDesc = Gfx::Pipeline()
        .Layout(0, { { "in_pos", VertexFormat::Float2 } })
        .PrimitiveType(PrimitiveType::TriangleStrip)
        .ColorFormat(PixelFormat::RGBA32F)
        .DepthFormat(PixelFormat::None)
        .Desc;
    this->initParticles.Pipeline = Gfx::Pipeline()
        .From(particlePipDesc)
        .Shader(Gfx::CreateShader(InitShader::Desc()))
        .Create();
    this->updParticles.Pipeline = Gfx::Pipeline()
        .From(particlePipDesc)
        .Shader(Gfx::CreateShader(UpdateShader::Desc()))
        .Create();

    // the static geometry of a single particle is at mesh slot 0
    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    auto shape = ShapeBuilder::New()
        .RandomColors(true)
        .Positions("in_pos", VertexFormat::Float3)
        .Colors("in_color", VertexFormat::Float4)
        .Transform(rot90)
        .Sphere(0.05f, 3, 2)
        .Build();
    this->shapePrimGroup = shape.PrimitiveGroups[0];
    this->drawParticles.VertexBuffers[0] = Gfx::Buffer()
        .From(shape.VertexBufferDesc)
        .Content(shape.Data)
        .Create();
    this->drawParticles.IndexBuffer = Gfx::Buffer()
        .From(shape.IndexBufferDesc)
        .Content(shape.Data)
        .Create();

    // a instancing vertex buffer with the particleIds at vertex buffer slot 1
    {
        const int particleIdSize = MaxNumParticles * sizeof(float);
        float* particleIdData = (float*) Memory::Alloc(particleIdSize);
        for (int i = 0; i < MaxNumParticles; i++) {
            particleIdData[i] = (float) i;
        }
        this->drawParticles.VertexBuffers[1] = Gfx::Buffer()
            .Size(particleIdSize)
            .Content(particleIdData)
            .Create();
        Memory::Free(particleIdData);
    }

    // ...and the pipeline object for instanced particle rendering
    this->drawParticles.Pipeline = Gfx::Pipeline()
        .From(shape.PipelineDesc)
        .Shader(Gfx::CreateShader(DrawShader::Desc()))
        .Layout(1, VertexLayout::New()
            .EnableInstancing()
            .Add("in_particleId", VertexFormat::Float))
        .CullFaceEnabled(true)
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .Create();

    // the static projection matrix
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 50.0f);

    // setup initial shader params
    const glm::vec2 bufferDims(ParticleBufferWidth, ParticleBufferHeight);
    this->initFSParams.bufDims = bufferDims;
    this->updFSParams.bufDims = bufferDims;
    this->drawVSParams.bufDims = bufferDims;

    // 'draw' the initial particle state (positions at origin, pseudo-random velocity)
    for (int i = 0; i < 2; i++) {
        Gfx::BeginPass(this->particleBuffer[0].pass, PassAction::DontCare());
        Gfx::ApplyDrawState(this->initParticles);
        Gfx::ApplyUniformBlock(this->initFSParams);
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
    this->updParticles.FSTexture[UpdateShader::prevState] = this->particleBuffer[readIndex].texture;
    this->updFSParams.numParticles = (float) this->curNumParticles;
    Gfx::BeginPass(this->particleBuffer[drawIndex].pass, PassAction::DontCare());
    Gfx::ApplyScissorRect(0, 0, ParticleBufferWidth, scissorHeight, Gfx::QueryFeature(GfxFeature::OriginTopLeft));
    Gfx::ApplyDrawState(this->updParticles);
    Gfx::ApplyUniformBlock(this->updFSParams);
    Gfx::Draw(0, 4);
    Gfx::EndPass();
    
    // now the actual particle shape rendering:
    // - the new particle state texture is sampled in the vertex shader to obtain particle positions
    // - draw 'curNumParticles' instances of the basic particle shape through hardware-instancing
    this->drawParticles.VSTexture[DrawShader::particleTex] = this->particleBuffer[drawIndex].texture;
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawParticles);
    Gfx::ApplyUniformBlock(this->drawVSParams);
    Gfx::Draw(this->shapePrimGroup, this->curNumParticles);
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("\n %d instances\n\r frame=%.3fms", this->curNumParticles, frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
void
GPUParticlesApp::updateCamera() {
    float angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.5f, glm::cos(angle) * 10.0f);
    this->view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->drawVSParams.mvp = this->proj * this->view * this->model;
}

//------------------------------------------------------------------------------
AppState::Code
GPUParticlesApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
