//------------------------------------------------------------------------------
//  Triangle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/MeshBuilder.h"

using namespace Oryol;
using namespace Oryol::Core;
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
    meshBuilder.Vertex(0, VertexAttr::Position, 0.0f, 0.5f, 0.5f);
    meshBuilder.Vertex(0, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f);
    // second vertex pos and color
    meshBuilder.Vertex(1, VertexAttr::Position, 0.5f, -0.5f, 0.5f);
    meshBuilder.Vertex(1, VertexAttr::Color0, 0.0f, 1.0f, 0.0f, 1.0f);
    // third vertex pos and color
    meshBuilder.Vertex(2, VertexAttr::Position, -0.5f, -0.5f, 0.5f);
    meshBuilder.Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 1.0f, 1.0f);
    meshBuilder.End();
    this->meshId = this->render->CreateResource(MeshSetup::FromData("msh"), meshBuilder.GetStream());
    
    // build a shader program (shortcut without precompiled vertex/fragment shaders)
    ProgramBundleSetup progSetup("prog");
    progSetup.AddProgramFromSources(0, vsSource, fsSource);
    this->progId = this->render->CreateResource(progSetup);
    
    return App::OnInit();
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
    return App::OnCleanup();
}
