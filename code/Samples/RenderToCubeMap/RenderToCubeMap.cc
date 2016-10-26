//------------------------------------------------------------------------------
//  RenderToCubeMap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

class RenderToCubeMapApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();

    Id cubeMap;
    static const int NumFaces = 6;
    const glm::vec4 ClearColors[NumFaces] = {
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
    };
    Id passes[NumFaces];
};
OryolMain(RenderToCubeMapApp);

//------------------------------------------------------------------------------
AppState::Code
RenderToCubeMapApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(800, 600, "Render To CubeMap");
    Gfx::Setup(gfxSetup);

    // create a cubemap which will serve as render target
    auto cubeMapSetup = TextureSetup::RenderTargetCube(256, 256, PixelFormat::RGBA8, PixelFormat::DEPTHSTENCIL);
    this->cubeMap = Gfx::CreateResource(cubeMapSetup);

    // create 6 render passes, one per cube map
    auto rpSetup = RenderPassSetup::From(this->cubeMap, this->cubeMap);
    for (int faceIndex = 0; faceIndex < NumFaces; faceIndex++) {
        rpSetup.ColorAttachments[0].Face = faceIndex;
        rpSetup.ColorAttachments[0].DefaultClearColor = ClearColors[faceIndex];
        this->passes[faceIndex] = Gfx::CreateResource(rpSetup);
    }

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
RenderToCubeMapApp::OnRunning() {
    Gfx::BeginPass(PassState(glm::vec4(0.5f, 0.5f, 0.7f, 1.0f)));

    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
RenderToCubeMapApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
