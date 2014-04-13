//------------------------------------------------------------------------------
//  Shapes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Application/App.h"
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
using namespace Oryol::Application;
using namespace Oryol::Render;
using namespace Oryol::Resource;

OryolApp("DDSTextureLoader", "1.0");

// derived application class
class DDSTextureLoadingApp : public App {
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
    static const int32 NumTextures = 5;
    std::array<Resource::Id, NumTextures> texId;
    glm::mat4 view;
    glm::mat4 proj;
    
    // shader slots
    static const int32 ModelViewProjection = 0;
    static const int32 Texture = 1;
};

// the vertex shader
static const char* vsSource =
"uniform mat4 mvp;\n"
"VS_INPUT(vec4, position);\n"
"VS_INPUT(vec2, texcoord0);\n"
"VS_OUTPUT(vec2, uv);\n"
"void main() {\n"
"  gl_Position = mvp * position;\n"
"  uv = texcoord0;\n"
"}\n";

// the pixel shader
static const char* fsSource =
"uniform sampler2D tex;\n"
"FS_INPUT(vec2, uv);\n"
"void main() {\n"
"  FragmentColor = TEXTURE2D(tex, uv);\n"
"}\n";

//------------------------------------------------------------------------------
void
OryolMain() {
    // execution starts here, create our app and start the main loop
    DDSTextureLoadingApp app;
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnInit() {

    // setup IO system
    this->io = IOFacade::CreateSingleton();
    this->io->RegisterFileSystem<HTTPFileSystem>("http", &HTTPFileSystem::Create<>);
    this->io->SetAssign("tex:", "http://localhost:8000/");

    // setup rendering system
    this->render = RenderFacade::CreateSingleton();
    this->render->AttachLoader(RawMeshLoader::Create());
    this->render->AttachLoader(TextureLoader::Create());
    this->render->Setup(RenderSetup::Windowed(600, 400, "Oryol DDS Loading Sample"));

    // start loading textures
    this->texId[0] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_dxt1.dds"));
/*
    this->texId[1] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_dxt3.dds"));
    this->texId[2] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_dxt5.dds"));
    this->texId[3] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_rgb.dds"));
    this->texId[4] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_lumi.dds"));
*/

    // create a shape with uvs
    ShapeBuilder shapeBuilder;
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.AddBox(1.0f, 1.0f, 1.0f, 4);
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
    
    this->proj = glm::perspective(glm::radians(45.0f), 1.5f, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return AppState::Running;
}

//------------------------------------------------------------------------------
glm::mat4
DDSTextureLoadingApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // clear, apply mesh and shader program, and draw
        this->render->ApplyState(Render::State::DepthMask, true);
        this->render->ApplyState(Render::State::DepthTestEnabled, true);
        this->render->ApplyState(Render::State::DepthFunc, Render::State::LessEqual);
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyState(Render::State::ClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
        this->render->Clear(true, true, true);
        this->render->ApplyProgram(this->progId, 0);
        this->render->ApplyMesh(this->meshId);
        
        // only render when texture is loaded (until texture placeholder are implemented)
        const auto resState = this->render->QueryResourceState(this->texId[0]);
        if (resState == Resource::State::Valid) {
            this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(0.0f, 0.0f, 0.0f)));
            this->render->ApplyVariable(Texture, this->texId[0]);
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
DDSTextureLoadingApp::OnCleanup() {
    // cleanup everything
    for (auto tex : this->texId) {
        if (tex.IsValid()) {
            this->render->DiscardResource(tex);
        }
    }
    this->render->DiscardResource(this->progId);
    this->render->DiscardResource(this->meshId);
    this->render->Discard();
    this->render = nullptr;
    RenderFacade::DestroySingleton();
    this->io = nullptr;
    IOFacade::DestroySingleton();
    
    return AppState::Destroy;
}
