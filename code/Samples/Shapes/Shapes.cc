//------------------------------------------------------------------------------
//  Shapes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class ShapeApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    glm::mat4 computeMVP(const glm::vec3& pos);
    DrawState drawState;
    Shader::params params;
    glm::mat4 view;
    glm::mat4 proj;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(ShapeApp);

//------------------------------------------------------------------------------
AppState::Code
ShapeApp::OnInit() {

    auto gfxSetup = GfxSetup::WindowMSAA4(600, 400, "Oryol Shapes Sample");
    Gfx::Setup(gfxSetup);

    ShapeBuilder shapeBuilder;
    shapeBuilder.RandomColors = true;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Color0, VertexFormat::UByte4N }
    };
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 4)
        .Sphere(0.75f, 36, 20)
        .Cylinder(0.5f, 1.5f, 36, 10)
        .Torus(0.3f, 0.5f, 20, 36)
        .Plane(1.5f, 1.5f, 10);
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    Id shd = Gfx::CreateResource(Shader::Setup());
    
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    ps.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->drawState.Pipeline = Gfx::CreateResource(ps);

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ShapeApp::OnRunning() {
    
    this->angleY += 0.01f;
    this->angleX += 0.02f;
    
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    static const glm::vec3 positions[] = {
        glm::vec3(-1.0, 1.0f, -6.0f),
        glm::vec3(1.0f, 1.0f, -6.0f),
        glm::vec3(-2.0f, -1.0f, -6.0f),
        glm::vec3(+2.0f, -1.0f, -6.0f),
        glm::vec3(0.0f, -1.0f, -6.0f)
    };
    int primGroupIndex = 0;
    for (const auto& pos : positions) {
        this->params.mvp = this->computeMVP(pos);
        Gfx::ApplyUniformBlock(this->params);
        Gfx::Draw(primGroupIndex++);
    }
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ShapeApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
ShapeApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

