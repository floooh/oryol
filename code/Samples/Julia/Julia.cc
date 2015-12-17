//------------------------------------------------------------------------------
//  Julia.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "fractal.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"

using namespace Oryol;
using namespace Julia;

class JuliaApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();
private:
    glm::mat4 computeMVP(const glm::vec3& pos, const glm::vec2& rot);

    int frameCount = 0;
    ClearState displayClearState;
    fractal test;
    Id shapeDrawState;
    Shaders::Shape::VSParams shapeVSParams;
    Shaders::Shape::FSParams shapeFSParams;
    Shaders::Shape::FSTextures shapeFSTextures;
    glm::mat4 view;
    glm::mat4 proj;
};
OryolMain(JuliaApp)

//------------------------------------------------------------------------------
AppState::Code
JuliaApp::OnInit() {
    auto gfxSetup = GfxSetup::WindowMSAA4(800, 600, "Julia");
    Gfx::Setup(gfxSetup);
    this->displayClearState = ClearState::ClearAll(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    Id fsq = Gfx::CreateResource(MeshSetup::FullScreenQuad(Gfx::QueryFeature(GfxFeature::OriginTopLeft)));
    this->test.setup(512, 512, glm::vec4(-0.75f, -0.75f, +0.75f, +0.75f), glm::vec2(0.292991f, -0.014885), fsq);

    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 64);
    Id shapeMesh = Gfx::CreateResource(shapeBuilder.Build());
    Id shd = Gfx::CreateResource(Shaders::Shape::Setup());
    auto dss = DrawStateSetup::FromMeshAndShader(shapeMesh, shd);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    dss.RasterizerState.SampleCount = gfxSetup.SampleCount;
    dss.RasterizerState.CullFaceEnabled = false;
    dss.BlendState.BlendEnabled = false;
    dss.BlendState.SrcFactorRGB = dss.BlendState.SrcFactorAlpha = BlendFactor::One;
    dss.BlendState.DstFactorRGB = dss.BlendState.DstFactorAlpha = BlendFactor::One;
    dss.BlendState.ColorFormat = gfxSetup.ColorFormat;
    dss.BlendState.DepthFormat = gfxSetup.DepthFormat;
    this->shapeDrawState = Gfx::CreateResource(dss);
    this->shapeFSParams.NumColors = 128.0;

    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
JuliaApp::OnRunning() {

    if ((frameCount & 1) == 0) {
        this->test.update();
    }

    glm::vec2 rot = glm::vec2(0.001f, 0.002f) * float(this->frameCount);
    this->frameCount++;
    this->shapeVSParams.ModelViewProj = this->computeMVP(glm::vec3(0.0f, 0.0f, 0.0), rot);
    this->shapeVSParams.Time = float(this->frameCount) / 60.0f;
    this->shapeVSParams.UVScale = glm::vec2(2.0f, 2.0f);
    this->shapeFSTextures.Texture = this->test.colorTexture;

    Gfx::ApplyDefaultRenderTarget(this->displayClearState);
    Gfx::ApplyDrawState(this->shapeDrawState, this->shapeFSTextures);
    Gfx::ApplyUniformBlock(this->shapeVSParams);
    Gfx::ApplyUniformBlock(this->shapeFSParams);
    Gfx::Draw(0);
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
JuliaApp::OnCleanup() {
    this->test.discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
JuliaApp::computeMVP(const glm::vec3& pos, const glm::vec2& rot) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}
