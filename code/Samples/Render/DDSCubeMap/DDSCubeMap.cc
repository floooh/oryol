//------------------------------------------------------------------------------
//  DDSCubeMap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IOFacade.h"
#include "HTTP/HTTPFileSystem.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Render/Util/TextureLoader.h"
#define GLM_FORCE_RADIANS
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <array>

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;
using namespace Oryol::HTTP;
using namespace Oryol::Render;
using namespace Oryol::Resource;

OryolApp("DDSCubeMap", "1.0");

// derived application class
class DDSCubeMapApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::vec3& pos);
    
    IOFacade* io = nullptr;
    RenderFacade* render = nullptr;
    Resource::Id meshId;
    Resource::Id progId;
    Resource::Id texId;
    glm::mat4 view;
    glm::mat4 proj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
    
    // shader slots
    static const int32 ModelViewProjection = 0;
    static const int32 Texture = 1;
};

// the vertex shader
static const char* vsSource =
"uniform mat4 mvp;\n"
"VS_INPUT(vec4, position);\n"
"VS_INPUT(vec3, normal);\n"
"VS_OUTPUT(vec3, nrm);\n"
"void main() {\n"
"  gl_Position = mvp * position;\n"
"  nrm = normal;\n"
"}\n";

// the pixel shader
static const char* fsSource =
"uniform samplerCube tex;\n"
"FS_INPUT(vec3, nrm);\n"
"void main() {\n"
"  FragmentColor = TEXTURECUBE(tex, nrm);\n"
"}\n";

//------------------------------------------------------------------------------
void
OryolMain() {
    // execution starts here, create our app and start the main loop
    DDSCubeMapApp app;
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnInit() {

    // setup IO system
    this->io = IOFacade::CreateSingleton();
    this->io->RegisterFileSystem<HTTPFileSystem>("http", &HTTPFileSystem::Create<>);
    this->io->SetAssign("tex:", "http://localhost:8000/");

    // setup rendering system
    this->render = RenderFacade::CreateSingleton();
    this->render->AttachLoader(RawMeshLoader::Create());
    this->render->AttachLoader(TextureLoader::Create());
    this->render->Setup(RenderSetup::Windowed(600, 400, "Oryol DXT Cube Map Sample"));
    float32 fbWidth = this->render->GetDisplayAttrs().GetFramebufferWidth();
    float32 fbHeight = this->render->GetDisplayAttrs().GetFramebufferHeight();

    // start loading textures
    TextureSetup texBluePrint;
    texBluePrint.SetMinFilter(TextureFilterMode::LinearMipmapLinear);
    texBluePrint.SetMagFilter(TextureFilterMode::Linear);
    texBluePrint.SetWrapU(TextureWrapMode::ClampToEdge);
    texBluePrint.SetWrapV(TextureWrapMode::ClampToEdge);
    this->texId = this->render->CreateResource(TextureSetup::FromFile("tex:romechurch_dxt1.dds", texBluePrint));

    // create a shape with normals
    glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetTransform(rot90);
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Normal, VertexFormat::Float3);
    shapeBuilder.AddSphere(1.0f, 36, 20);
    shapeBuilder.Build();
    this->meshId = this->render->CreateResource(MeshSetup::FromData("shape"), shapeBuilder.GetStream());

    // build a shader program from a vertex- and fragment shader
    Id vs = this->render->CreateResource(ShaderSetup::FromSource("vs", ShaderType::VertexShader, vsSource));
    Id fs = this->render->CreateResource(ShaderSetup::FromSource("fs", ShaderType::FragmentShader, fsSource));
    ProgramBundleSetup progSetup("prog");
    progSetup.AddProgram(0, vs, fs);
    progSetup.AddUniform("mvp", ModelViewProjection);
    progSetup.AddTextureUniform("tex", Texture);
    this->progId = this->render->CreateResource(progSetup);
    
    // can release vertex- and fragment shader handle now
    this->render->DiscardResource(vs);
    this->render->DiscardResource(fs);
    
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
DDSCubeMapApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
    
        // update rotation angles
        this->angleY += 0.02f;
        this->angleX += 0.01f;
        
        // clear, apply mesh and shader program, and draw
        this->render->ApplyState(Render::State::DepthMask, true);
        this->render->ApplyState(Render::State::DepthTestEnabled, true);
        this->render->ApplyState(Render::State::DepthFunc, Render::State::LessEqual);
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyState(Render::State::ClearColor, 0.5f, 0.5f, 0.5f, 1.0f);
        this->render->Clear(true, true, true);
        this->render->ApplyProgram(this->progId, 0);
        this->render->ApplyMesh(this->meshId);
        
        const auto resState = this->render->QueryResourceState(this->texId);
        if (resState == Resource::State::Valid) {
            this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(0.0f, 0.0f, 0.0f)));
            this->render->ApplyVariable(Texture, this->texId);
            this->render->Draw(0);
        }
        else if (resState == Resource::State::Failed) {
            Log::Warn("Failed to load texture resource!\n");
        }
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnCleanup() {
    // cleanup everything
    this->render->DiscardResource(this->texId);
    this->render->DiscardResource(this->progId);
    this->render->DiscardResource(this->meshId);
    this->render->Discard();
    this->render = nullptr;
    RenderFacade::DestroySingleton();
    this->io = nullptr;
    IOFacade::DestroySingleton();
    
    return App::OnCleanup();
}
