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
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <array>
#include "shaders.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;
using namespace Oryol::HTTP;
using namespace Oryol::Render;
using namespace Oryol::Resource;

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
    Resource::Id drawState;
    Resource::Id tex;
    glm::mat4 view;
    glm::mat4 proj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
};
OryolMain(DDSCubeMapApp);

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnInit() {

    // setup IO system
    this->io = IOFacade::CreateSingle();
    this->io->RegisterFileSystem("http", Creator<HTTPFileSystem, FileSystem>());
    this->io->SetAssign("tex:", "http://localhost:8000/");

    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(600, 400, "Oryol DXT Cube Map Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    this->render->AttachLoader(TextureLoader::Create());
    float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;

    // create resources
    TextureSetup texBluePrint;
    texBluePrint.MinFilter = TextureFilterMode::LinearMipmapLinear;
    texBluePrint.MagFilter = TextureFilterMode::Linear;
    texBluePrint.WrapU = TextureWrapMode::ClampToEdge;
    texBluePrint.WrapV = TextureWrapMode::ClampToEdge;
    if (this->render->Supports(Feature::TextureCompressionPVRTC)) {
        this->tex = this->render->CreateResource(TextureSetup::FromFile("tex:romechurch_bpp2.pvr", texBluePrint));
    }
    else {
        this->tex = this->render->CreateResource(TextureSetup::FromFile("tex:romechurch_dxt1.dds", texBluePrint));
    }
    glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetTransform(rot90);
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Normal, VertexFormat::Float3);
    shapeBuilder.AddSphere(1.0f, 36, 20);
    shapeBuilder.Build();
    Id mesh = this->render->CreateResource(MeshSetup::FromData("shape"), shapeBuilder.GetStream());
    Id prog = this->render->CreateResource(Shaders::Main::CreateSetup());
    DrawStateSetup dsSetup("ds", mesh, prog, 0);
    dsSetup.DepthStencilState.SetDepthWriteEnabled(true);
    dsSetup.DepthStencilState.SetDepthCompareFunc(CompareFunc::LessEqual);
    this->drawState = this->render->CreateResource(dsSetup);
    
    this->render->ReleaseResource(mesh);
    this->render->ReleaseResource(prog);
    
    // setup projection and view matrices
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
        
        // apply state and draw
        this->render->ApplyDefaultRenderTarget();
        this->render->ApplyDrawState(this->drawState);
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyState(Render::State::ClearColor, 0.5f, 0.5f, 0.5f, 1.0f);
        this->render->Clear(true, true, true);
        
        const auto resState = this->render->QueryResourceState(this->tex);
        if (resState == Resource::State::Valid) {
            this->render->ApplyVariable(Shaders::Main::ModelViewProjection, this->computeMVP(glm::vec3(0.0f, 0.0f, 0.0f)));
            this->render->ApplyVariable(Shaders::Main::Texture, this->tex);
            this->render->Draw(0);
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
    this->render->ReleaseResource(this->tex);
    this->render->ReleaseResource(this->drawState);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    this->io = nullptr;
    IOFacade::DestroySingle();
    
    return App::OnCleanup();
}
