//------------------------------------------------------------------------------
//  Triangle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/MeshBuilder.h"
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Resource;

// derived application class
class TriangleApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    Resource::Id drawState;
};
OryolMain(TriangleApp);

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnRunning() {
    
    // render one frame
    if (this->render->BeginFrame()) {
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::RGBA, glm::vec4(0.0f), 1.0f, 0);
        this->render->ApplyDrawState(this->drawState);
        this->render->Draw(0);
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnInit() {
    // setup rendering system
    auto renderSetup = RenderSetup::AsWindow(400, 400, false, "Oryol Triangle Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    this->render = RenderFacade::CreateSingle(renderSetup);
    
    // create a triangle mesh, with position and vertex color
    MeshBuilder meshBuilder;
    meshBuilder.SetNumVertices(3);
    meshBuilder.SetIndexType(IndexType::None);
    meshBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    meshBuilder.VertexLayout().Add(VertexAttr::Color0, VertexFormat::Float4);
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
    Id mesh = this->render->CreateResource(MeshSetup::FromData("msh"), meshBuilder.GetStream());
    Id prog = this->render->CreateResource(Shaders::Triangle::CreateSetup());
    this->drawState = this->render->CreateResource(DrawStateSetup("ds", mesh, prog, 0));

    this->render->ReleaseResource(mesh);
    this->render->ReleaseResource(prog);
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
TriangleApp::OnCleanup() {
    // cleanup everything
    this->render->ReleaseResource(this->drawState);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
