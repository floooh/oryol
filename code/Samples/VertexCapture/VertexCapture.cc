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

    glm::mat4 computeMVP(const glm::vec3& pos);

    static const int NumPoints = 1024 * 1024;
    static const int NumPointMeshes = 2;
    StaticArray<Id, NumPointMeshes> pointMeshes;
    DrawState captureState;
    DrawState drawState;
    CaptureShader::Params captureParams;
    DrawShader::Params drawParams;

    uint64_t frameIndex = 0;
    glm::mat4 proj;
    glm::mat4 view;
    float angleX = 0.0f;
    float angleY = 0.0f;
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
    this->pointMeshes[0] = Gfx::CreateResource(meshSetup, initData, initDataSize);
    Memory::Free(initData);

    meshSetup = MeshSetup::Empty(NumPoints, Usage::Immutable);
    meshSetup.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    this->pointMeshes[1] = Gfx::CreateResource(meshSetup);

    // pipeline state for the vertex capture pass
    auto pipSetup = PipelineSetup::FromLayoutAndShader(meshSetup.Layout, Gfx::CreateResource(CaptureShader::Setup()));
    pipSetup.PrimType = PrimitiveType::Points;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    pipSetup.EnableVertexCapture = true;
    pipSetup.CaptureLayout = meshSetup.Layout;
    pipSetup.Layouts[0] = meshSetup.Layout;
    this->captureState.Pipeline = Gfx::CreateResource(pipSetup);

    // pipeline state to render the vertices as point cloud
    pipSetup = PipelineSetup::FromLayoutAndShader(meshSetup.Layout, Gfx::CreateResource(DrawShader::Setup()));
    pipSetup.PrimType = PrimitiveType::Points;
    pipSetup.DepthStencilState.DepthWriteEnabled = true;
    pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    pipSetup.Layouts[0] = meshSetup.Layout;
    this->drawState.Pipeline = Gfx::CreateResource(pipSetup);

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();

    this->captureParams.Time = 0.0f;

    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
VertexCaptureApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnRunning() {

    this->captureParams.Time += 1.0f/60.0f;
    this->angleX += 0.002;
    this->angleY += 0.003;
    this->drawParams.ModelViewProjection = this->computeMVP(glm::vec3(0.0f, 0.0f, -6.0f));

    Gfx::ApplyDefaultRenderTarget();

    // perform the vertex capture pass
    int readIndex = this->frameIndex & 1;
    int writeIndex = (this->frameIndex + 1) & 1;
    this->captureState.Mesh[0] = this->pointMeshes[readIndex];
    this->captureState.CaptureMesh = this->pointMeshes[writeIndex];
    Gfx::ApplyDrawState(this->captureState);
    Gfx::ApplyUniformBlock(this->captureParams);
    Gfx::Draw(PrimitiveGroup(0, NumPoints));

    // perform the render pass
    this->drawState.Mesh[0] = this->pointMeshes[writeIndex];
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->drawParams);
    Gfx::Draw(PrimitiveGroup(0, NumPoints));

    Gfx::CommitFrame();
    this->frameIndex++;
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

