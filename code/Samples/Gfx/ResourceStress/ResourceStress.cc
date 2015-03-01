//------------------------------------------------------------------------------
//  ResourceStress.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IO.h"
#include "Gfx/Gfx.h"
#include "HTTP/HTTPFileSystem.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Assets/Gfx/TextureLoader.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class ResourceStressApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
private:
    struct Object {
        Id drawState;
        ResourceLabel label;
        glm::mat4 modelTransform;
        uint32 startFrame = 0;
    };
    Array<Object> objects;
    glm::mat4 view;
    glm::mat4 proj;
};
OryolMain(ResourceStressApp);

//------------------------------------------------------------------------------
AppState::Code
ResourceStressApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    
    Gfx::CommitFrame();
    
    // quit or keep running?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ResourceStressApp::OnInit() {
    // setup IO system
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("tex:", ORYOL_SAMPLE_URL);
    IO::Setup(ioSetup);

    // setup Gfx system
    Gfx::Setup(GfxSetup::Window(600, 400, "Oryol Resource Stress Test"));

    // setup matrices
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ResourceStressApp::OnCleanup() {
    Gfx::Discard();
    IO::Discard();
    return App::OnCleanup();
}

