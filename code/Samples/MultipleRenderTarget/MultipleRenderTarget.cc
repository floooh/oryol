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

    Id mrtPass;
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
    auto gfxSetup = GfxSetup::WindowMSAA4(DisplayWidth, DisplayHeight, "Oryol MRT Sample");
    gfxSetup.DefaultPassAction = PassAction::Clear(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    Gfx::Setup(gfxSetup);
    Dbg::Setup();

    // if rendering backend doesn't support MRT, drop out now
    // and show a warning during frame rendering
    if (!Gfx::QueryFeature(GfxFeature::MultipleRenderTarget)) {
        return App::OnInit();
    }

    // create 3 rendertarget textures, the first one with a DepthStencil
    // surface, note that the render target textures use MSAA anti-aliasing
    auto rtSetup = TextureSetup::RenderTarget2D(OffscreenWidth, OffscreenHeight, PixelFormat::RGBA8, PixelFormat::DEPTHSTENCIL);
    rtSetup.SampleCount = 4;
    rtSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    rtSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    Id rt0 = Gfx::CreateResource(rtSetup);
    rtSetup.DepthFormat = PixelFormat::None;
    Id rt1 = Gfx::CreateResource(rtSetup);
    Id rt2 = Gfx::CreateResource(rtSetup);

    // create a render pass which uses the 3 texture we just created
    // as color attachments, and the first texture as depth-stencil buffer
    // FIXME: depth-stencil should be its own texture!
    auto passSetup = PassSetup::From({ rt0, rt1, rt2 }, rt0);
    passSetup.DefaultAction
        .ClearColor(0, glm::vec4(0.25f, 0.0f, 0.0f, 1.0f))
        .ClearColor(1, glm::vec4(0.0f, 0.25f, 0.0f, 1.0f))
        .ClearColor(2, glm::vec4(0.0f, 0.0f, 0.25f, 1.0f));
    this->mrtPass = Gfx::CreateResource(passSetup);

    // create a mesh with 2 shapes, a box and a plane
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Normal, VertexFormat::UByte4N },
        { VertexAttr::TexCoord0, VertexFormat::Float2 }
    };
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 1);
    shapeBuilder.Plane(1.0f, 1.0f, 1);
    Id cubeMesh = Gfx::CreateResource(shapeBuilder.Build());

    // create a draw state to render a cube into the
    // offscreen render targets (this is where the MRT rendering happens)
    Id shd = Gfx::CreateResource(OffscreenShader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    ps.RasterizerState.CullFaceEnabled = true;
    ps.RasterizerState.SampleCount = rtSetup.SampleCount;
    ps.BlendState.ColorFormat = rtSetup.ColorFormat;
    ps.BlendState.MRTCount = 3;
    this->cubeDrawState.Pipeline = Gfx::CreateResource(ps);
    this->cubeDrawState.Mesh[0] = cubeMesh;

    // create a quad-mesh for displaying the 3 render target
    // textures on screen
    auto quadMeshSetup = MeshSetup::FullScreenQuad();
    Id quadMesh = Gfx::CreateResource(quadMeshSetup);
    Id quadShd = Gfx::CreateResource(QuadShader::Setup());
    ps = PipelineSetup::FromLayoutAndShader(quadMeshSetup.Layout, quadShd);
    ps.DepthStencilState.DepthWriteEnabled = false;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    ps.RasterizerState.CullFaceEnabled = false;
    ps.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->rt0DrawState.Pipeline = Gfx::CreateResource(ps);
    this->rt0DrawState.Mesh[0] = quadMesh;
    this->rt0DrawState.FSTexture[QuadShader::tex] = rt0;
    this->rt1DrawState.Pipeline = this->rt0DrawState.Pipeline;
    this->rt1DrawState.Mesh[0] = quadMesh;
    this->rt1DrawState.FSTexture[QuadShader::tex] = rt1;
    this->rt2DrawState.Pipeline = this->rt0DrawState.Pipeline;
    this->rt2DrawState.Mesh[0] = quadMesh;
    this->rt2DrawState.FSTexture[QuadShader::tex] = rt2;

    // and finally create a draw state to render a plane to the
    // main display which samples the 3 offscreen render targets
    Id displayShd = Gfx::CreateResource(DisplayShader::Setup());
    ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, displayShd);
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    ps.RasterizerState.CullFaceEnabled = false;
    ps.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->displayDrawState.Pipeline = Gfx::CreateResource(ps);
    this->displayDrawState.Mesh[0] = cubeMesh;
    this->displayDrawState.FSTexture[DisplayShader::redTex] = rt0;
    this->displayDrawState.FSTexture[DisplayShader::greenTex] = rt1;
    this->displayDrawState.FSTexture[DisplayShader::blueTex] = rt2;

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
    Gfx::BeginPass(this->mrtPass);
    Gfx::ApplyDrawState(this->cubeDrawState);
    Gfx::ApplyUniformBlock(this->cubeParams);
    Gfx::Draw(0);
    Gfx::EndPass();

    // debug-visualize the 3 offscreen render targets at the bottom of the screen
    Gfx::BeginPass();
    Gfx::ApplyViewPort(0, 0, 200, 200);
    Gfx::ApplyDrawState(this->rt0DrawState);
    Gfx::Draw();
    Gfx::ApplyViewPort(200, 0, 200, 200);
    Gfx::ApplyDrawState(this->rt1DrawState);
    Gfx::Draw();
    Gfx::ApplyViewPort(400, 0, 200, 200);
    Gfx::ApplyDrawState(this->rt2DrawState);
    Gfx::Draw();

    // render the final plane which samples from all 3 offscreen rendertarget textures
    const auto& rpAttrs = Gfx::PassAttrs();
    Gfx::ApplyViewPort(0, 0, rpAttrs.FramebufferWidth, rpAttrs.FramebufferHeight);
    Gfx::ApplyDrawState(this->displayDrawState);
    Gfx::ApplyUniformBlock(this->displayParams);
    Gfx::Draw(1);

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
    uint8_t x = uint8_t((Gfx::DisplayAttrs().FramebufferWidth/16 - std::strlen(msg))/2);
    uint8_t y = uint8_t(Gfx::DisplayAttrs().FramebufferHeight/16/2);
    Gfx::BeginPass(PassAction::Clear(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f)));
    Dbg::TextScale(2.0f, 2.0f);
    Dbg::CursorPos(x, y);
    Dbg::Print(msg);
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}
