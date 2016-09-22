//------------------------------------------------------------------------------
//  PrimitiveTypes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/MeshBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class PrimitiveTypesApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    static const int NumX = 64;
    static const int NumY = 64;
    static const int NumVertices = NumX * NumY;

    Id vertexMesh;            // mesh object with 2D grid vertex data
    DrawState points;
    DrawState lineList;
    DrawState lineStrip;
    DrawState triList;
    DrawState triStrip;

    glm::mat4 view;
    glm::mat4 proj;
    float angleX = 0.0f;
    float angleY = 0.0f;    
    Shader::Params vsParams;
};
OryolMain(PrimitiveTypesApp);

//------------------------------------------------------------------------------
AppState::Code
PrimitiveTypesApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(640, 480, "Oryol PrimitiveTypes Test");
    Gfx::Setup(gfxSetup);

    // create a 2D vertex grid mesh
    MeshBuilder meshBuilder;
    meshBuilder.NumVertices = NumVertices;
    meshBuilder.IndicesType = IndexType::None;
    meshBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Color0, VertexFormat::UByte4N);
    meshBuilder.Begin();
    const float dx = 1.0f / NumX;
    const float dy = 1.0f / NumY;
    const float xOffset = -dx * (NumX/2);
    const float yOffset = -dy * (NumY/2);
    for (int x = 0, vi=0; x < NumX; x++) {
        for (int y = 0; y < NumY; y++, vi++) {
            meshBuilder.Vertex(vi, VertexAttr::Position, x*dx+xOffset, y*dy+yOffset, 0.0f);
            meshBuilder.Vertex(vi, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    this->vertexMesh = Gfx::CreateResource(meshBuilder.Build());

    // pipeline object for point list
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto pipSetup = PipelineSetup::FromLayoutAndShader(meshBuilder.Layout, shd);
    pipSetup.DepthStencilState.DepthWriteEnabled = true;
    pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    pipSetup.PrimType = PrimitiveType::Points;
    this->points.Pipeline = Gfx::CreateResource(pipSetup);
    this->points.Mesh[0] = this->vertexMesh;

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PrimitiveTypesApp::OnRunning() {

    this->angleY += 0.001f;
    this->angleX += 0.002f;

    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f));
    model = glm::rotate(model, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    this->vsParams.ModelViewProjection = this->proj * this->view * model;

    Gfx::ApplyDefaultRenderTarget();

    // draw points
    Gfx::ApplyDrawState(this->points);
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::Draw(PrimitiveGroup(0, NumVertices));

    Gfx::CommitFrame();

    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PrimitiveTypesApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}


