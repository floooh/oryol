//-----------------------------------------------------------------------------
//  MeshViewer.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "IO/IO.h"
#include "IMUI/IMUI.h"
#include "Input/Input.h"
#include "HTTP/HTTPFileSystem.h"
#include "Assets/Gfx/MeshLoader.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/polar_coordinates.hpp"
#include "shaders.h"

using namespace Oryol;

class MeshViewerApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

private:
    void handleInput();
    void updateCamera();
    void drawUI();
    void loadMesh(const char* path);

    int32 frameCount = 0;
    ResourceLabel curResLabel;
    Id mesh;
    Id drawState;
    MeshSetup curMeshSetup;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 modelViewProj;
    int32 curMeshIndex = 0;
    static const int32 numMeshes = 3;
    const char* meshNames[numMeshes] = {
        "Tiger",
        "Blitz",
        "Teapot"
    };
    const char* meshPaths[numMeshes] = {
        "msh:tiger.omsh",
        "msh:opelblitz.omsh",
        "msh:teapot.omsh"
    };
    float camDist = 8.0f;
    const float minCamDist = 1.0f;
    const float maxCamDist = 20.0f;
    const float minLatitude = -85.0f;
    const float maxLatitude = 85.0f;
    glm::vec2 orbital = glm::vec2(glm::radians(25.0f), 0.0f);
    float camHeight = 1.0f;
    const float minCamHeight = -2.0f;
    const float maxCamHeight = 5.0f;
    bool autoOrbit = true;
};
OryolMain(MeshViewerApp);

//-----------------------------------------------------------------------------
AppState::Code
MeshViewerApp::OnInit() {

    // setup IO system
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("msh:", "http://localhost:8000/");
    IO::Setup(ioSetup);

    // setup rendering and input system
    Gfx::Setup(GfxSetup::WindowMSAA4(800, 512, "Oryol Mesh Viewer"));
    Input::Setup();

    // setup IMUI ui system
    IMUI::Setup();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.5f, 1.0f, 0.7f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.5f, 1.0f, 0.20f);

    this->loadMesh(this->meshPaths[this->curMeshIndex]);

    // setup projection and view matrices
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(60.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    
    return App::OnInit();
}

//-----------------------------------------------------------------------------
AppState::Code
MeshViewerApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget();

    this->frameCount++;
    this->handleInput();
    this->updateCamera();
    this->drawUI();

    Gfx::Clear(PixelChannel::All, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyVariable(Shaders::Main::ModelViewProjection, this->modelViewProj);
    for (int i = 0; i < this->curMeshSetup.NumPrimitiveGroups(); i++) {
        Gfx::Draw(i);
    }
    ImGui::Render();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//-----------------------------------------------------------------------------
AppState::Code
MeshViewerApp::OnCleanup() {
    IMUI::Discard();
    Input::Discard();
    Gfx::Discard();
    IO::Discard();
    return App::OnCleanup();
}

//-----------------------------------------------------------------------------
void
MeshViewerApp::handleInput() {
    if (!ImGui::IsMouseHoveringAnyWindow()) {
        const Mouse& mouse = Input::Mouse();
        if (mouse.Attached) {
            if (mouse.ButtonPressed(Mouse::LMB)) {
                this->orbital.y -= mouse.Movement.x * 0.01f;
                this->orbital.x = glm::clamp(this->orbital.x + mouse.Movement.y * 0.01f, glm::radians(minLatitude), glm::radians(maxLatitude));            
            }
            this->camDist = glm::clamp(this->camDist + mouse.Scroll.y * 0.1f, minCamDist, maxCamDist);
        }
    }
}

//------------------------------------------------------------------------------
void
MeshViewerApp::updateCamera() {
    if (this->autoOrbit) {
        this->orbital.y += 0.01f;
        if (this->orbital.y > glm::radians(360.0f)) {
            this->orbital.y = 0.0f;
        }
    }
    glm::vec3 eucl = glm::euclidean(this->orbital) * this->camDist;
    glm::vec3 poi  = glm::vec3(0.0f, this->camHeight, 0.0f);
    this->view = glm::lookAt(eucl + poi, poi, glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelViewProj = this->proj * this->view;
}

//-----------------------------------------------------------------------------
void
MeshViewerApp::drawUI() {
    const char* state = "Loaded";
    switch (Gfx::QueryResourceInfo(this->mesh).State) {
        case ResourceState::Valid: state = "Valid"; break;
        case ResourceState::Failed: state = "Failed"; break;
        case ResourceState::Pending: state = "Pending"; break;
        default: state = "Invalid"; break;
    }
    IMUI::NewFrame();
    ImGui::Begin("Controls", nullptr, ImVec2(200, 300), 0.25f, 0);
    if (ImGui::Combo("Mesh", (int*) &this->curMeshIndex, this->meshNames, numMeshes)) {
        this->loadMesh(this->meshPaths[this->curMeshIndex]);
    }
    ImGui::Text("state: %s\n", state);
    ImGui::Text("primitive groups:\n");
    for (int i = 0; i < this->curMeshSetup.NumPrimitiveGroups(); i++) {
        ImGui::Text("%d: %d triangles\n", i, this->curMeshSetup.PrimitiveGroup(i).NumElements / 3);
    }
    ImGui::Separator();
    ImGui::Text("Camera:\n");
    ImGui::SliderFloat("Dist", &this->camDist, minCamDist, maxCamDist);
    ImGui::SliderFloat("Height", &this->camHeight, minCamHeight, maxCamHeight);
    ImGui::SliderAngle("Long", &this->orbital.y, 0, 360.0f);
    ImGui::SliderAngle("Lat", &this->orbital.x, minLatitude, maxLatitude);
    ImGui::Checkbox("Auto Orbit", &this->autoOrbit);
    if (ImGui::Button("Reset")) {
        this->camDist = 8.0f;
        this->camHeight = 1.0f;
        this->orbital = glm::vec2(0.0f, 0.0f);
        this->autoOrbit = false;
    }
    ImGui::End();
}

//-----------------------------------------------------------------------------
void
MeshViewerApp::loadMesh(const char* path) {

    // unload current mesh
    if (this->curResLabel.IsValid()) {
        Gfx::DestroyResources(this->curResLabel);
        this->curResLabel.Invalidate();
        this->drawState.Invalidate();
        this->mesh.Invalidate();
        this->curMeshSetup = MeshSetup();
    }

    // load new mesh
    this->curResLabel = Gfx::PushResourceLabel();
    this->mesh = Gfx::LoadResource(MeshLoader::Create(MeshSetup::FromFile(path), 0, [this](MeshSetup& setup) {
        this->curMeshSetup = setup;
    }));
    Id prog = Gfx::CreateResource(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(this->mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    dss.RasterizerState.CullFaceEnabled = true;
    this->drawState = Gfx::CreateResource(dss);
    Gfx::PopResourceLabel();
}
