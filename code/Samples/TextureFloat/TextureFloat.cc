//------------------------------------------------------------------------------
//  TextureFloat.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Core/Time/Clock.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class TextureFloatApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
    Id renderPass;
    PassAction renderPassAction;
    Id offscreenPipeline;
    Bindings offscreenBind;
    Id copyPipeline;
    Bindings copyBind;
    OffscreenShader::fsParams offscreenFSParams;
    TimePoint lastFrameTimePoint;
};
OryolMain(TextureFloatApp);

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnInit() {
    // setup rendering system
    Gfx::Setup(GfxDesc()
        .SetWidth(512)
        .SetHeight(512)
        .SetTitle("Oryol Float Texture Sample")
        .SetHtmlTrackElementSize(true));
    Dbg::Setup();

    // check required extensions
    if (!Gfx::QueryFeature(GfxFeature::TextureFloat)) {
        o_error("ERROR: float_texture extension required!\n");
    }
    
    // create an offscreen float render target, same size as display,
    // configure texture sampler with point-filtering
    const PixelFormat::Code rtColorFormat = PixelFormat::RGBA32F;
    Id rt = Gfx::CreateTexture(TextureDesc()
        .SetRenderTarget(true)
        .SetWidth(Gfx::Desc().Width)
        .SetHeight(Gfx::Desc().Height)
        .SetFormat(rtColorFormat)
        .SetMinFilter(TextureFilterMode::Nearest)
        .SetMagFilter(TextureFilterMode::Nearest));
    this->renderPass = Gfx::CreatePass(PassDesc()
        .SetColorAttachment(0, rt));
    this->renderPassAction.DontCareColor(0);

    // fullscreen mesh, we'll reuse this several times
    const float quadVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    this->offscreenBind.VertexBuffers[0] = Gfx::CreateBuffer(BufferDesc()
        .SetSize(sizeof(quadVertices))
        .SetContent(quadVertices));
    this->copyBind.VertexBuffers[0] = this->offscreenBind.VertexBuffers[0];

    // setup draw state for offscreen rendering to float render target
    this->offscreenPipeline = Gfx::CreatePipeline(PipelineDesc()
        .SetShader(Gfx::CreateShader(OffscreenShader::Desc()))
        .SetLayout(0, {{"in_pos", VertexFormat::Float2}})
        .SetPrimitiveType(PrimitiveType::TriangleStrip)
        .SetColorFormat(rtColorFormat)
        .SetDepthFormat(PixelFormat::None));
    this->offscreenFSParams.time = 0.0f;

    // fullscreen-copy resources
    this->copyPipeline = Gfx::CreatePipeline(PipelineDesc()
        .SetShader(Gfx::CreateShader(CopyShader::Desc()))
        .SetLayout(0, {{"in_pos", VertexFormat::Float2}})
        .SetPrimitiveType(PrimitiveType::TriangleStrip));
    this->copyBind.FSTexture[CopyShader::tex] = rt;

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnRunning() {

    this->offscreenFSParams.time += 1.0f / 60.0f;
    
    // render plasma to offscreen render target, do not clear
    Gfx::BeginPass(this->renderPass, this->renderPassAction);
    Gfx::ApplyPipeline(this->offscreenPipeline);
    Gfx::ApplyBindings(this->offscreenBind);
    Gfx::ApplyUniforms(this->offscreenFSParams);
    Gfx::Draw(0, 4);
    Gfx::EndPass();
    
    // copy fullscreen quad
    Gfx::BeginPass();
    Gfx::ApplyPipeline(this->copyPipeline);
    Gfx::ApplyBindings(this->copyBind);
    Gfx::Draw(0, 4);
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    Duration frameTime = Clock::LapTime(this->lastFrameTimePoint);
    Dbg::PrintF("\n\n\n\n\n  %.3fms", frameTime.AsMilliSeconds());
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TextureFloatApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
