//------------------------------------------------------------------------------
//  VertexCapture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "Dbg/Dbg.h"
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

    VertexLayout initPointMeshes();
    glm::mat4 computeMVP(const glm::vec3& pos);
    glm::vec3 computeCenterOfGravity();

    static const int NumPointMeshes = 2;
    ResourceLabel pointMeshLabel;
    StaticArray<Id, NumPointMeshes> pointMeshes;
    DrawState captureState;
    DrawState drawState;
    CaptureShader::Params captureParams;
    DrawShader::Params drawParams;

    int numPoints = 128 * 1024;
    uint64_t frameIndex = 0;
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 invProj;
    glm::mat4 invView;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(VertexCaptureApp);

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnInit() {
    auto gfxSetup = GfxSetup::WindowMSAA4(800, 600, "Oryol Vertex Capture Sample");
    Gfx::Setup(gfxSetup);
    Input::Setup();
    Dbg::Setup();

    if (!Gfx::QueryFeature(GfxFeature::VertexCapture)) {
        o_error("ERROR: VertexCapture not supported!\n");
    }
    VertexLayout layout = this->initPointMeshes();

    // pipeline state for the vertex capture pass
    auto pipSetup = PipelineSetup::FromLayoutAndShader(layout, Gfx::CreateResource(CaptureShader::Setup()));
    pipSetup.PrimType = PrimitiveType::Points;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    pipSetup.EnableVertexCapture = true;
    pipSetup.CaptureLayout = layout;
    pipSetup.Layouts[0] = layout;
    this->captureState.Pipeline = Gfx::CreateResource(pipSetup);

    // pipeline state to render the vertices as point cloud
    pipSetup = PipelineSetup::FromLayoutAndShader(layout, Gfx::CreateResource(DrawShader::Setup()));
    pipSetup.PrimType = PrimitiveType::Points;
    pipSetup.DepthStencilState.DepthWriteEnabled = true;
    pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    pipSetup.Layouts[0] = layout;
    this->drawState.Pipeline = Gfx::CreateResource(pipSetup);

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    this->invProj = glm::inverse(this->proj);
    this->invView = glm::inverse(this->view);

    return App::OnInit();
}

//------------------------------------------------------------------------------
VertexLayout
VertexCaptureApp::initPointMeshes() {
    if (this->pointMeshLabel.IsValid()) {
        Gfx::DestroyResources(this->pointMeshLabel);
    }
    this->pointMeshLabel = Gfx::PushResourceLabel();

    // create 2 double-buffered point cloud meshes, start with a random data set
    VertexLayout pointLayout;
    pointLayout.Add(VertexAttr::Position, VertexFormat::Float3);
    pointLayout.Add(VertexAttr::Normal, VertexFormat::Float3);
    auto meshSetup = MeshSetup::FromData();
    meshSetup.Layout = pointLayout;
    meshSetup.NumVertices = this->numPoints;
    const int initDataSize = this->numPoints * meshSetup.Layout.ByteSize();
    float* initData = (float*) Memory::Alloc(initDataSize);
    for (int i = 0, j = 0; i < this->numPoints; i++) {

        float x = glm::linearRand(-1.0f, 1.0f);
        float y = glm::linearRand(-1.0f, 1.0f);
        float z = glm::linearRand(-1.0f, 1.0f);
        // pos
        initData[j++] = x;
        initData[j++] = y;
        initData[j++] = z;

        // velocity
        initData[j++] = 0.0f;
        initData[j++] = 0.0f;
        initData[j++] = 0.0f;
    }
    this->pointMeshes[0] = Gfx::CreateResource(meshSetup, initData, initDataSize);
    Memory::Free(initData);

    meshSetup = MeshSetup::Empty(this->numPoints, Usage::Immutable);
    meshSetup.Layout = pointLayout;
    this->pointMeshes[1] = Gfx::CreateResource(meshSetup);

    Gfx::PopResourceLabel();
    this->frameIndex = 0;

    return meshSetup.Layout;
}

//------------------------------------------------------------------------------
glm::mat4
VertexCaptureApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
/*
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
*/
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
glm::vec3
VertexCaptureApp::computeCenterOfGravity() {
    glm::vec2 mousePos(0.0f, 0.0f);
    if (Input::MouseAttached()) {
        mousePos = Input::MousePosition();
    }
    if ((mousePos.x > 0.0f) && (mousePos.y > 0.0f)) {
        int fbWidth = (float) Gfx::RenderTargetAttrs().FramebufferWidth;
        int fbHeight = (float) Gfx::RenderTargetAttrs().FramebufferHeight;
        float x = 3.1f * ((mousePos.x / fbWidth) * 2.0f - 1.0f);
        float y = 2.1f * ((mousePos.y / fbHeight) * -2.0f + 1.0f);
        glm::vec3 centerOfGravity = glm::vec3(x, y, 0.0f);
        return centerOfGravity;
    }
    else {
        return glm::vec3(1024.0f);
    }
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnRunning() {

    // important, creating/destroying resources needs to happen after
    // last CommitFrame()!
    if (Input::KeyDown(Key::N1)) {
        this->numPoints = 128 * 1024;
        this->initPointMeshes();
    }
    else if (Input::KeyDown(Key::N2)) {
        this->numPoints = 512 * 1024;
        this->initPointMeshes();
    }
    else if (Input::KeyDown(Key::N3)) {
        this->numPoints = 1024 * 1024;
        this->initPointMeshes();
    }


    Gfx::ApplyDefaultRenderTarget(ClearState::ClearAll(glm::vec4(0.2f, 0.3f, 0.4f, 1.0f)));

    int readIndex = this->frameIndex & 1;
    int writeIndex = (this->frameIndex + 1) & 1;
    this->captureState.Mesh[0] = this->pointMeshes[readIndex];
    this->captureState.CaptureMesh = this->pointMeshes[writeIndex];
    this->captureParams.CenterOfGravity = this->computeCenterOfGravity();
    Gfx::ApplyDrawState(this->captureState);
    Gfx::ApplyUniformBlock(this->captureParams);
    Gfx::Draw(PrimitiveGroup(0, this->numPoints));

    // perform the render pass
    this->angleX += 0.002;
    this->angleY += 0.003;
    this->drawParams.ModelViewProjection = this->computeMVP(glm::vec3(0.0f, 0.0f, -6.0f));
    this->drawState.Mesh[0] = this->pointMeshes[writeIndex];
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->drawParams);
    Gfx::Draw(PrimitiveGroup(0, this->numPoints));

    Dbg::PrintF("\n\r Move the mouse!\n\r"
                " 1: 128k particles\n\r"
                " 2: 512k particles\n\r"
                " 3: 1m particles\n\r");
    Dbg::DrawTextBuffer();

    Gfx::CommitFrame();

    this->frameIndex++;
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnCleanup() {
    Dbg::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

