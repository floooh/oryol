//------------------------------------------------------------------------------
//  Triangle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Application/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/MeshBuilder.h"

using namespace Oryol;
using namespace Oryol::Application;
using namespace Oryol::Render;
using namespace Oryol::Resource;

OryolApp("Triangle", "1.0");

// derived application class
class TriangleApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    Resource::Id meshId;
    Resource::Id progId;
};

// the vertex shader
static const char* vsSource =
"VS_INPUT(vec4, position);\n"
"VS_INPUT(vec4, color0);\n"
"VS_OUTPUT(vec4, color);\n"
"void main() {\n"
"  gl_Position = position;\n"
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
    TriangleApp app;
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingleton();
    this->render->AttachLoader(RawMeshLoader::Create());
    this->render->Setup(RenderSetup::Windowed(400, 400, "Oryol Triangle Sample"));
    
    // create a triangle mesh, with position and vertex color
    MeshBuilder meshBuilder;
    meshBuilder.SetNumVertices(3);
    meshBuilder.SetIndexType(IndexType::None);
    meshBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    meshBuilder.AddComponent(VertexAttr::Color0, VertexFormat::Float4);
    meshBuilder.AddPrimitiveGroup(PrimitiveType::Triangles, 0, 3);
    meshBuilder.Begin();
    // first vertex pos and color
    meshBuilder.Vertex(0, 0, 0.0f, 0.5f, 0.5f);
    meshBuilder.Vertex(0, 1, 1.0f, 0.0f, 0.0f, 1.0f);
    // second vertex pos and color
    meshBuilder.Vertex(1, 0, 0.5f, -0.5f, 0.5f);
    meshBuilder.Vertex(1, 1, 0.0f, 1.0f, 0.0f, 1.0f);
    // third vertex pos and color
    meshBuilder.Vertex(2, 0, -0.5f, -0.5f, 0.5f);
    meshBuilder.Vertex(2, 1, 0.0f, 0.0f, 1.0f, 1.0f);
    meshBuilder.End();
    this->meshId = this->render->CreateResource(MeshSetup::FromData("msh"), meshBuilder.GetStream());
    
    // build a shader program from a vertex- and fragment shader
    Id vs = this->render->CreateResource(ShaderSetup::FromSource("vs", ShaderType::VertexShader, vsSource));
    Id fs = this->render->CreateResource(ShaderSetup::FromSource("fs", ShaderType::FragmentShader, fsSource));
    ProgramBundleSetup progSetup("prog");
    progSetup.AddProgram(0, vs, fs);
    this->progId = this->render->CreateResource(progSetup);
    
    // can release vertex- and fragment shader handle now
    this->render->DiscardResource(vs);
    this->render->DiscardResource(fs);
    
    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // clear, apply mesh and shader program, and draw
        this->render->ApplyState(Render::State::ClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
        this->render->Clear(true, false, false);
        this->render->ApplyMesh(this->meshId);
        this->render->ApplyProgram(this->progId, 0);
        this->render->Draw(0);
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnCleanup() {
    // cleanup everything
    this->render->DiscardResource(this->progId);
    this->render->DiscardResource(this->meshId);
    this->render->Discard();
    this->render = nullptr;
    RenderFacade::DestroySingleton();
    return AppState::Destroy;
}
