//------------------------------------------------------------------------------
//  VertexCapture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "glm/gtc/random.hpp"

using namespace Oryol;

class VertexCaptureApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    static const int NumPoints = 1024;
    static const int NumPointMeshes = 2;
    StaticArray<Id, NumPointMeshes> pointMeshes;
};
OryolMain(VertexCaptureApp);

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnInit() {
    Gfx::Setup(GfxSetup::WindowMSAA4(800, 600, "Oryol Vertex Capture Sample"));

    if (!Gfx::QueryFeature(GfxFeature::VertexCapture)) {
        o_error("ERROR: VertexCapture not supported!\n");
    }

    // create 2 double-buffered point cloud meshes, start with a random
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

    // FIXME: pipeline state to render the vertices as point cloud


    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnRunning() {

    Gfx::ApplyDefaultRenderTarget();

    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
VertexCaptureApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

