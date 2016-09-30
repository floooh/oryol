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
    glm::vec3 computeCenterOfGravity();
    AppState::Code notSupported();

    static const int NumPointMeshes = 2;
    ResourceLabel pointMeshLabel;
    StaticArray<Id, NumPointMeshes> pointMeshes;
    DrawState captureState;
    DrawState drawState;
    CaptureShader::Params captureParams;
    DrawShader::Params drawParams;

    int numPoints = 128 * 1024;
    uint64_t frameIndex = 0;
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
        return App::OnInit();
    }

    // pipeline state for the vertex capture pass
    VertexLayout layout = this->initPointMeshes();
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
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -6.0f));
    this->drawParams.ModelViewProjection = proj * model;

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
        // pos
        initData[j++] = glm::linearRand(-1.0f, 1.0f);
        initData[j++] = glm::linearRand(-1.0f, 1.0f);
        initData[j++] = glm::linearRand(-1.0f, 1.0f);
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

    // if vertex-capture is not supported just display a message
    if (!Gfx::QueryFeature(GfxFeature::VertexCapture)) {
        return notSupported();
    }

    // creating/destroying resources needs to happen after
    // outside ApplyRenderTarget() and CommitFrame()!
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
    else if (Input::KeyDown(Key::N4)) {
        this->numPoints = 2048 * 1024;
        this->initPointMeshes();
    }

    Gfx::ApplyDefaultRenderTarget(ClearState::ClearAll(glm::vec4(0.2f, 0.3f, 0.4f, 1.0f)));

    // update point positions
    int readIndex = this->frameIndex & 1;
    int writeIndex = (this->frameIndex + 1) & 1;
    this->captureState.Mesh[0] = this->pointMeshes[readIndex];
    this->captureState.CaptureMesh = this->pointMeshes[writeIndex];
    this->captureParams.CenterOfGravity = this->computeCenterOfGravity();
    Gfx::ApplyDrawState(this->captureState);
    Gfx::ApplyUniformBlock(this->captureParams);
    Gfx::Draw(PrimitiveGroup(0, this->numPoints));

    // render points
    this->drawState.Mesh[0] = this->pointMeshes[writeIndex];
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->drawParams);
    Gfx::Draw(PrimitiveGroup(0, this->numPoints));

    Dbg::PrintF("\n\r Move the mouse!\n\r"
                " 1: 128k particles\n\r"
                " 2: 512k particles\n\r"
                " 3: 1m particles\n\r"
                " 4: 2m particles\n\r");
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

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::notSupported() {
    // print a warning if vertex-capture is not supported by platform
    #if ORYOL_EMSCRIPTEN
    const char* msg = "This demo needs WebGL2\n";
    #else
    const char* msg = "This demo needs VertexCapture\n";
    #endif
    uint8_t x = (Gfx::DisplayAttrs().FramebufferWidth/16 - strlen(msg))/2;
    uint8_t y = Gfx::DisplayAttrs().FramebufferHeight/16/2;
    Gfx::ApplyDefaultRenderTarget(ClearState::ClearColor(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f)));
    Dbg::SetTextScale(glm::vec2(2.0f, 2.0f));
    Dbg::CursorPos(x, y);
    Dbg::Print(msg);
    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}
