//------------------------------------------------------------------------------
//  MultipleRenderTarget.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class MultipleRenderTargetApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    glm::mat4 computeMVP(const glm::vec3& pos);

    const int DisplayWidth = 640;
    const int DisplayHeight = 480;
    const int OffscreenWidth = 200;
    const int OffscreenHeight = 200;

    Id mrtPass;
    DrawState rt0DrawState;
    DrawState rt1DrawState;
    DrawState cubeDrawState;
    DisplayShader::VSParams cubeParams;
    glm::mat4 proj;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(MultipleRenderTargetApp);

//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(DisplayWidth, DisplayHeight, "Oryol MRT Sample");
    gfxSetup.DefaultClearColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    Gfx::Setup(gfxSetup);

    auto rtSetup = TextureSetup::RenderTarget(OffscreenWidth, OffscreenHeight);
    rtSetup.DepthFormat = PixelFormat::DEPTHSTENCIL;
    Id rt0 = Gfx::CreateResource(rtSetup);
    rtSetup.DepthFormat = PixelFormat::None;
    Id rt1 = Gfx::CreateResource(rtSetup);
    auto passSetup = RenderPassSetup::From({ rt0, rt1 }, rt0);
    passSetup.ColorAttachments[0].DefaultClearColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    passSetup.ColorAttachments[1].DefaultClearColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    this->mrtPass = Gfx::CreateResource(passSetup);

    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::UByte4N);
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 1);
    this->cubeDrawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    Id shd = Gfx::CreateResource(OffscreenShader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    ps.RasterizerState.CullFaceEnabled = true;
    ps.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->cubeDrawState.Pipeline = Gfx::CreateResource(ps);

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
    this->rt0DrawState.FSTexture[Textures::Texture] = rt0;

    this->rt1DrawState.Pipeline = this->rt0DrawState.Pipeline;
    this->rt1DrawState.Mesh[0] = quadMesh;
    this->rt1DrawState.FSTexture[Textures::Texture] = rt1;

    this->proj = glm::perspectiveFov(glm::radians(45.0f), float(OffscreenWidth), float(OffscreenHeight), 0.01f, 100.0f);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::OnRunning() {

    this->angleY += 0.01f;
    this->angleX += 0.02f;
    this->cubeParams.ModelViewProjection = this->computeMVP(glm::vec3(0, 0, -3));

    Gfx::BeginPass(this->mrtPass);
    Gfx::ApplyDrawState(this->cubeDrawState);
    Gfx::ApplyUniformBlock(this->cubeParams);
    Gfx::Draw();
    Gfx::EndPass();

    Gfx::BeginPass();
    Gfx::ApplyViewPort(0, 0, 200, 200);
    Gfx::ApplyDrawState(this->rt0DrawState);
    Gfx::Draw();
    Gfx::ApplyViewPort(200, 0, 200, 200);
    Gfx::ApplyDrawState(this->rt1DrawState);
    Gfx::Draw();
    Gfx::EndPass();

    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
MultipleRenderTargetApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
MultipleRenderTargetApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * modelTform;
}

