//------------------------------------------------------------------------------
//  PackedNormals.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class PackedNormalsApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    glm::mat4 computeMVP(const glm::vec3& pos);

    Array<PrimitiveGroup> primGroups;
    DrawState drawState;
    Shader::params params;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(PackedNormalsApp);

//------------------------------------------------------------------------------
AppState::Code
PackedNormalsApp::OnInit() {
    Gfx::Setup(GfxDesc()
        .Width(600).Height(400)
        .SampleCount(4)
        .Title("Oryol Packed Normals Sample")
        .HtmlTrackElementSize(true));

    auto shapes = ShapeBuilder()
        .Positions("position", VertexFormat::Float3)
        .Normals("normal", VertexFormat::Byte4N)
        .Box(1.0f, 1.0f, 1.0f, 4)
        .Sphere(0.75f, 36, 20)
        .Cylinder(0.5f, 1.5f, 36, 10)
        .Torus(0.3f, 0.5f, 20, 36)
        .Plane(1.5f, 1.5f, 10)
        .Build();
    this->drawState.VertexBuffers[0] = Gfx::CreateBuffer(shapes.VertexBufferDesc);
    this->drawState.IndexBuffer = Gfx::CreateBuffer(shapes.IndexBufferDesc);
    this->drawState.Pipeline = Gfx::CreatePipeline(PipelineDesc(shapes.PipelineDesc)
        .Shader(Gfx::CreateShader(Shader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .CullFaceEnabled(true)
        .SampleCount(4));
    this->primGroups = std::move(shapes.PrimitiveGroups);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PackedNormalsApp::OnRunning() {
    
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
        Gfx::Draw(this->primGroups[primGroupIndex++]);
    }
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PackedNormalsApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
PackedNormalsApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), float(Gfx::Width()), float(Gfx::Height()), 0.01f, 100.0f);
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return proj * modelTform;
}
