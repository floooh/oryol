//------------------------------------------------------------------------------
//  MultipleRenderTarget.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"
#include <cstring>

using namespace Oryol;

class MultipleRenderTargetApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    glm::mat4 computeMVP(float angleX, float angleY, const glm::vec3& pos);
    AppState::Code notSupported();

    const int DisplayWidth = 640;
    const int DisplayHeight = 480;
    const int OffscreenWidth = 200;
    const int OffscreenHeight = 200;

    PrimitiveGroup cubePrimGroup;
    PrimitiveGroup planePrimGroup;
    Id mrtPass;
    PassAction mrtPassAction;
    DrawState rt0DrawState;
    DrawState rt1DrawState;
    DrawState rt2DrawState;

    DrawState cubeDrawState;
    OffscreenShader::vsParams cubeParams;

    DrawState displayDrawState;
    DisplayShader::vsParams displayParams;

    glm::mat4 proj;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(MultipleRenderTargetApp);

//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::OnInit() {
    Gfx::Setup(NewGfxDesc().WindowedMSAA4(DisplayWidth, DisplayHeight, "Oryol MRT Sample").Done());
    Dbg::Setup();

    // if rendering backend doesn't support MRT, drop out now
    // and show a warning during frame rendering
    if (!Gfx::QueryFeature(GfxFeature::MultipleRenderTarget)) {
        return App::OnInit();
    }

    // create 3 rendertarget textures, the first one with a DepthStencil
    // surface, note that the render target textures use MSAA anti-aliasing
    const PixelFormat::Code rtColorFormat = PixelFormat::RGBA8;
    const PixelFormat::Code rtDepthFormat = PixelFormat::DEPTHSTENCIL;
    const int rtSampleCount = 4;
    auto rtDesc = NewTextureDesc()
        .Type(TextureType::Texture2D)
        .RenderTarget(true)
        .Width(OffscreenWidth)
        .Height(OffscreenHeight)
        .Format(rtColorFormat)
        .MinFilter(TextureFilterMode::Linear)
        .MagFilter(TextureFilterMode::Linear)
        .SampleCount(rtSampleCount)
        .Done();
    Id rtColor0 = Gfx::CreateTexture(rtDesc);
    Id rtColor1 = Gfx::CreateTexture(rtDesc);
    Id rtColor2 = Gfx::CreateTexture(rtDesc);
    Id rtDepth = Gfx::CreateTexture(NewTextureDesc().From(rtDesc).Format(rtDepthFormat).Done());

    // create a render pass with the 3 color- and 1 depth-attachment
    this->mrtPass = Gfx::CreatePass(NewPassDesc()
        .ColorAttachment(0, rtColor0)
        .ColorAttachment(1, rtColor1)
        .ColorAttachment(2, rtColor2)
        .DepthStencilAttachment(rtDepth)
        .Done());

    // a pass-action to clear the multiple-render-target
    this->mrtPassAction
        .ClearColor(0, glm::vec4(0.25f, 0.0f, 0.0f, 1.0f))
        .ClearColor(1, glm::vec4(0.0f, 0.25f, 0.0f, 1.0f))
        .ClearColor(2, glm::vec4(0.0f, 0.0f, 0.25f, 1.0f));

    // create a mesh with 2 shapes, a box and a plane
    auto shapes = ShapeBuilder::New()
        .Positions("in_pos", VertexFormat::Float3)
        .Normals("in_normal", VertexFormat::UByte4N)
        .TexCoords("in_uv", VertexFormat::Float2)
        .Box(1.0f, 1.0f, 1.0f, 1)
        .Plane(1.0f, 1.0f, 1)
        .Build();
    this->cubePrimGroup = shapes.PrimitiveGroups[0];
    this->planePrimGroup = shapes.PrimitiveGroups[1];
    Id shapesVertexBuffer = Gfx::CreateBuffer(shapes.VertexBufferDesc);
    Id shapesIndexBuffer = Gfx::CreateBuffer(shapes.IndexBufferDesc);

    // create a draw state to render a cube into the
    // offscreen render targets (this is where the MRT rendering happens)
    this->cubeDrawState.Pipeline = Gfx::CreatePipeline(NewPipelineDesc()
        .From(shapes.PipelineDesc)
        .Shader(Gfx::CreateShader(OffscreenShader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .CullFaceEnabled(true)
        .ColorFormat(rtColorFormat)
        .DepthFormat(rtDepthFormat)
        .SampleCount(rtSampleCount)
        .MRTCount(3)
        .Done());
    this->cubeDrawState.VertexBuffers[0] = shapesVertexBuffer;
    this->cubeDrawState.IndexBuffer = shapesIndexBuffer;

    // create a quad-mesh for displaying the 3 render target textures on screen
    const float quadVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    Id quadVertexBuffer = Gfx::CreateBuffer(NewBufferDesc()
        .Size(sizeof(quadVertices))
        .Content(quadVertices)
        .Done());
    Id quadPipeline = Gfx::CreatePipeline(NewPipelineDesc()
        .Shader(Gfx::CreateShader(QuadShader::Desc()))
        .Layout(0, { { "in_pos", VertexFormat::Float2 } })
        .PrimitiveType(PrimitiveType::TriangleStrip)
        .DepthWriteEnabled(false)
        .DepthCmpFunc(CompareFunc::Always)
        .CullFaceEnabled(false)
        .SampleCount(Gfx::Desc().SampleCount)
        .Done());
    this->rt0DrawState.Pipeline = quadPipeline;
    this->rt0DrawState.VertexBuffers[0] = quadVertexBuffer;
    this->rt0DrawState.FSTexture[QuadShader::tex] = rtColor0;
    this->rt1DrawState.Pipeline = quadPipeline;
    this->rt1DrawState.VertexBuffers[0] = quadVertexBuffer;
    this->rt1DrawState.FSTexture[QuadShader::tex] = rtColor1;
    this->rt2DrawState.Pipeline = quadPipeline;
    this->rt2DrawState.VertexBuffers[0] = quadVertexBuffer;
    this->rt2DrawState.FSTexture[QuadShader::tex] = rtColor2;

    // and finally create a draw state to render a plane to the
    // main display which samples the 3 offscreen render targets
    this->displayDrawState.Pipeline = Gfx::CreatePipeline(NewPipelineDesc()
        .From(shapes.PipelineDesc)
        .Shader(Gfx::CreateShader(DisplayShader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .CullFaceEnabled(false)
        .SampleCount(Gfx::Desc().SampleCount)
        .Done());
    this->displayDrawState.VertexBuffers[0] = shapesVertexBuffer;
    this->displayDrawState.IndexBuffer = shapesIndexBuffer;
    this->displayDrawState.FSTexture[DisplayShader::redTex] = rtColor0;
    this->displayDrawState.FSTexture[DisplayShader::greenTex] = rtColor1;
    this->displayDrawState.FSTexture[DisplayShader::blueTex] = rtColor2;

    this->proj = glm::perspectiveFov(glm::radians(45.0f), float(OffscreenWidth), float(OffscreenHeight), 0.01f, 100.0f);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::OnRunning() {

    // if rendering backend has no MRT support, drop out
    if (!Gfx::QueryFeature(GfxFeature::MultipleRenderTarget)) {
        return this->notSupported();
    }

    // compute all the vertex shader uniforms
    this->angleY += 0.01f;
    this->angleX += 0.02f;
    this->cubeParams.mvp = this->computeMVP(this->angleX, this->angleY, glm::vec3(0, 0, -3));
    this->displayParams.mvp = this->computeMVP(this->angleX * 0.5f, this->angleY * 0.5f, glm::vec3(0.0f, 0.55f, -3.0f));
    this->displayParams.offsets = glm::sin(glm::vec2(this->angleX, this->angleY)) * 0.1f;

    // render the cube into the 3 MRT render targets using a single draw call,
    // the fragment shader writes 3 colors, one for each color attachment
    Gfx::BeginPass(this->mrtPass, this->mrtPassAction);
    Gfx::ApplyDrawState(this->cubeDrawState);
    Gfx::ApplyUniformBlock(this->cubeParams);
    Gfx::Draw(this->cubePrimGroup);
    Gfx::EndPass();

    // debug-visualize the 3 offscreen render targets at the bottom of the screen
    Gfx::BeginPass(PassAction::New().Clear(0.5f, 0.5f, 0.5f, 1.0f));
    Gfx::ApplyViewPort(0, 0, 200, 200);
    Gfx::ApplyDrawState(this->rt0DrawState);
    Gfx::Draw(0, 4);
    Gfx::ApplyViewPort(200, 0, 200, 200);
    Gfx::ApplyDrawState(this->rt1DrawState);
    Gfx::Draw(0, 4);
    Gfx::ApplyViewPort(400, 0, 200, 200);
    Gfx::ApplyDrawState(this->rt2DrawState);
    Gfx::Draw(0, 4);

    // render the final plane which samples from all 3 offscreen rendertarget textures
    const auto& rpAttrs = Gfx::PassAttrs();
    Gfx::ApplyViewPort(0, 0, rpAttrs.Width, rpAttrs.Height);
    Gfx::ApplyDrawState(this->displayDrawState);
    Gfx::ApplyUniformBlock(this->displayParams);
    Gfx::Draw(this->planePrimGroup);

    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
MultipleRenderTargetApp::computeMVP(float ax, float ay, const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, ax, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, ay, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::notSupported() {
    #if ORYOL_EMSCRIPTEN
    const char* msg = "This demo needs WebGL2\n";
    #else
    const char* msg = "This demo needs MultipleRenderTarget\n";
    #endif
    uint8_t x = uint8_t((Gfx::DisplayAttrs().Width/16 - std::strlen(msg))/2);
    uint8_t y = uint8_t((Gfx::DisplayAttrs().Height/16)/2);
    Gfx::BeginPass(PassAction::New().Clear(0.5f, 0.0f, 0.0f, 1.0f));
    Dbg::TextScale(2.0f, 2.0f);
    Dbg::CursorPos(x, y);
    Dbg::Print(msg);
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}
