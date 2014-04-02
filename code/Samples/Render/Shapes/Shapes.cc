//------------------------------------------------------------------------------
//  Shapes.cc
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

OryolApp("Shapes", "1.0");

// derived application class
class ShapeApp : public App {
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
"VS_INPUT(vec4, color0);\n"
"VS_OUTPUT(vec4, color);\n"
"void main() {\n"
"  gl_Position = mvp * position;\n"
"  color = color0;\n"
"}\n";

// the pixel shader
static const char* fsSource =
"FS_INPUT(vec4, color);\n"
"void main() {\n"
"  FragmentColor = color;\n"
"}\n";

//------------------------------------------------------------------------------
void
OryolMain() {
    // execution starts here, create our app and start the main loop
    ShapeApp app;
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
AppState::Code
ShapeApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingleton();
    this->render->AttachLoader(RawMeshLoader::Create());
    this->render->Setup(RenderSetup::Windowed(600, 400, "Oryol Shapes Sample"));

    // create a cube mesh
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetRandomColorsFlag(true);
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Color0, VertexFormat::Float4);
    
    // add a cube (FIXME:
    shapeBuilder.BeginPrimitiveGroup();
    shapeBuilder.AddBox(1.0f, 1.0f, 1.0f, 4);
    shapeBuilder.EndPrimitiveGroup();
    
    // add a sphere
    shapeBuilder.BeginPrimitiveGroup();
    shapeBuilder.AddSphere(0.75f, 36, 20);
    shapeBuilder.EndPrimitiveGroup();
    
    // add a cylinder
    shapeBuilder.BeginPrimitiveGroup();
    shapeBuilder.AddCylinder(0.5f, 0.5f, 1.5f, 36, 10);
    shapeBuilder.EndPrimitiveGroup();
    
    // add a torus
    shapeBuilder.BeginPrimitiveGroup();
    shapeBuilder.AddTorus(0.3f, 0.5f, 20, 36);
    shapeBuilder.EndPrimitiveGroup();
    
    // add a plane
    shapeBuilder.BeginPrimitiveGroup();
    shapeBuilder.AddPlane(1.5f, 1.5f, 10);
    shapeBuilder.EndPrimitiveGroup();
    
    shapeBuilder.Build();
    this->meshId = this->render->CreateResource(MeshSetup::FromData("cube"), shapeBuilder.GetStream());

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
    
    this->proj = glm::perspective(glm::radians(45.0f), 1.5f, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return AppState::Running;
}

//------------------------------------------------------------------------------
glm::mat4
ShapeApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
ShapeApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // compute a new ModelViewProj matrix
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
        
        // render the cube
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(-1.0, 1.0, -6.0f)));
        this->render->Draw(0);
        
        // render the sphere
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(1.0f, 1.0f, -6.0f)));
        this->render->Draw(1);
        
        // render the cylinder
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(-2.0f, -1.0f, -6.0f)));
        this->render->Draw(2);
        
        // render the torus
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(+2.0f, -1.0f, -6.0f)));
        this->render->Draw(3);
        
        // render the plane
        this->render->ApplyVariable(ModelViewProjection, this->computeMVP(glm::vec3(0.0f, -1.0f, -6.0f)));
        this->render->Draw(4);
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ShapeApp::OnCleanup() {
    // cleanup everything
    this->render->DiscardResource(this->progId);
    this->render->DiscardResource(this->meshId);
    this->render->Discard();
    this->render = nullptr;
    RenderFacade::DestroySingleton();
    return AppState::Destroy;
}
