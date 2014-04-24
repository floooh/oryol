//------------------------------------------------------------------------------
//  PackedNormals.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Application/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#define GLM_FORCE_RADIANS
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace Oryol;
using namespace Oryol::Application;
using namespace Oryol::Render;
using namespace Oryol::Resource;

OryolApp("PackedNormals", "1.0");

// derived application class
class PackedNormalsApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::vec3& pos);

    RenderFacade* render = nullptr;
    Resource::Id meshId;
    Resource::Id progId;
    glm::mat4 view;
    glm::mat4 proj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
    
    // shader slots
    static const int32 ModelViewProjection = 0;
};

// the vertex shader
static const char* vsSource =
"uniform mat4 mvp;\n"
"VS_INPUT(vec4, position);\n"
"VS_INPUT(vec4, normal);\n"
"VS_OUTPUT(vec4, nrm);\n"
"void main() {\n"
"  gl_Position = mvp * position;\n"
"  nrm = normal;\n"
"}\n";

// the pixel shader
static const char* fsSource =
"FS_INPUT(vec4, nrm);\n"
"void main() {\n"
"  FragmentColor = nrm * 0.5 + 0.5;\n"
"}\n";

//------------------------------------------------------------------------------
void
OryolMain() {
    // execution starts here, create our app and start the main loop
    PackedNormalsApp app;
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
AppState::Code
PackedNormalsApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingleton();
    this->render->AttachLoader(RawMeshLoader::Create());
    this->render->Setup(RenderSetup::Windowed(600, 400, "Oryol Packed Normals Sample"));
    float32 fbWidth = this->render->GetDisplayAttrs().GetFramebufferWidth();
    float32 fbHeight = this->render->GetDisplayAttrs().GetFramebufferHeight();

    // create shapes (each shape gets its own primitive group)
    ShapeBuilder shapeBuilder;
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.AddBox(1.0f, 1.0f, 1.0f, 4);
    shapeBuilder.AddSphere(0.75f, 36, 20);
    shapeBuilder.AddCylinder(0.5f, 1.5f, 36, 10);
    shapeBuilder.AddTorus(0.3f, 0.5f, 20, 36);
    shapeBuilder.AddPlane(1.5f, 1.5f, 10);
    shapeBuilder.Build();
    this->meshId = this->render->CreateResource(MeshSetup::FromData("shapes"), shapeBuilder.GetStream());

    // build a shader program from a vertex- and fragment shader
    Id vs = this->render->CreateResource(ShaderSetup::FromSource("vs", ShaderType::VertexShader, vsSource));
    Id fs = this->render->CreateResource(ShaderSetup::FromSource("fs", ShaderType::FragmentShader, fsSource));
    ProgramBundleSetup progSetup("prog");
    progSetup.AddProgram(0, vs, fs);
    progSetup.AddUniform("mvp", ModelViewProjection);
    this->progId = this->render->CreateResource(progSetup);
    
    // can release vertex- and fragment shader handle now
    this->render->DiscardResource(vs);
    this->render->DiscardResource(fs);
    
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return AppState::Running;
}

//------------------------------------------------------------------------------
glm::mat4
PackedNormalsApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
PackedNormalsApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // update angles
        this->angleY += 0.01f;
        this->angleX += 0.02f;

        // clear, apply mesh and shader program, and draw
        this->render->ApplyState(Render::State::DepthMask, true);
        this->render->ApplyState(Render::State::DepthTestEnabled, true);
        this->render->ApplyState(Render::State::DepthFunc, Render::State::LessEqual);
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyState(Render::State::ClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
        this->render->Clear(true, true, true);
        this->render->ApplyProgram(this->progId, 0);
        this->render->ApplyMesh(this->meshId);
        
        // draw shape primitive groups
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(-1.0, 1.0f, -6.0f)));
        this->render->Draw(0);
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(1.0f, 1.0f, -6.0f)));
        this->render->Draw(1);
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(-2.0f, -1.0f, -6.0f)));
        this->render->Draw(2);
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(+2.0f, -1.0f, -6.0f)));
        this->render->Draw(3);
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(0.0f, -1.0f, -6.0f)));
        this->render->Draw(4);
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PackedNormalsApp::OnCleanup() {
    // cleanup everything
    this->render->DiscardResource(this->progId);
    this->render->DiscardResource(this->meshId);
    this->render->Discard();
    this->render = nullptr;
    RenderFacade::DestroySingleton();
    return AppState::Destroy;
}
