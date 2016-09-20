//------------------------------------------------------------------------------
//  VertexCapture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "glm/gtc/random.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class VertexCaptureApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    static const int NumPoints = 1024;
    static const int NumPointMeshes = 2;
    StaticArray<Id, NumPointMeshes> pointMeshes;
    DrawState drawState;
    Shader::Params vsParams;
};
OryolMain(VertexCaptureApp);

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(800, 600, "Oryol Vertex Capture Sample");
    Gfx::Setup(gfxSetup);

    if (!Gfx::QueryFeature(GfxFeature::VertexCapture)) {
        o_error("ERROR: VertexCapture not supported!\n");
    }

    // create 2 double-buffered point cloud meshes, start with a random data set
    auto meshSetup = MeshSetup::FromData();
    meshSetup.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    meshSetup.NumVertices = NumPoints;
    const int initDataSize = NumPoints * meshSetup.Layout.ByteSize();
    float* initData = (float*) Memory::Alloc(initDataSize);
    for (int i = 0; i < NumPoints * 3; i++) {
        initData[i] = glm::linearRand(-1.0f, 1.0f);
    }
    for (int i = 0; i < NumPointMeshes; i++) {
        this->pointMeshes[i] = Gfx::CreateResource(meshSetup, initData, initDataSize);
    }
    Memory::Free(initData);

    // pipeline state to render the vertices as point cloud
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto pipSetup = PipelineSetup::FromLayoutAndShader(meshSetup.Layout, shd);
    pipSetup.PrimType = PrimitiveType::Points;
    pipSetup.DepthStencilState.DepthWriteEnabled = true;
    pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    pipSetup.EnableVertexCapture = true;
    pipSetup.CaptureLayout = meshSetup.Layout;
    pipSetup.Layouts[0] = meshSetup.Layout;
    drawState.Pipeline = Gfx::CreateResource(pipSetup);
    drawState.Mesh[0] = this->pointMeshes[0];
    drawState.CaptureMesh = this->pointMeshes[1];

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    glm::mat4 view = glm::mat4();
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -6.0f));
    this->vsParams.ModelViewProjection = proj * view * model;

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnRunning() {

    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::Draw(PrimitiveGroup(0, NumPoints));

    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

