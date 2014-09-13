//------------------------------------------------------------------------------
//  DDSCubeMap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
#include "Render/Render.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "Render/Util/TextureLoader.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

// derived application class
class DDSCubeMapApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::vec3& pos);
    
    Id drawState;
    Id tex;
    glm::mat4 view;
    glm::mat4 proj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
};
OryolMain(DDSCubeMapApp);

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnRunning() {
    // render one frame
    if (Render::BeginFrame()) {
        
        // update rotation angles
        this->angleY += 0.02f;
        this->angleX += 0.01f;
        
        // apply state and draw
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::All, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), 1.0f, 0);
        Render::ApplyDrawState(this->drawState);
        
        const auto resState = Render::QueryResourceState(this->tex);
        if (resState == ResourceState::Valid) {
            Render::ApplyVariable(Shaders::Main::ModelViewProjection, this->computeMVP(glm::vec3(0.0f, 0.0f, 0.0f)));
            Render::ApplyVariable(Shaders::Main::Texture, this->tex);
            Render::Draw(0);
        }
        Render::EndFrame();
    }
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnInit() {

    // setup IO system
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("tex:", "http://localhost:8000/");
    IO::Setup(ioSetup);

    // setup rendering system
    auto renderSetup = RenderSetup::AsWindow(600, 400, false, "Oryol DXT Cube Map Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    renderSetup.Loaders.Add(TextureLoader::Creator());
    Render::Setup(renderSetup);

    // create resources
    TextureSetup texBluePrint;
    texBluePrint.MinFilter = TextureFilterMode::LinearMipmapLinear;
    texBluePrint.MagFilter = TextureFilterMode::Linear;
    texBluePrint.WrapU = TextureWrapMode::ClampToEdge;
    texBluePrint.WrapV = TextureWrapMode::ClampToEdge;
    if (Render::Supports(RenderFeature::TextureCompressionPVRTC)) {
        this->tex = Render::CreateResource(TextureSetup::FromFile("tex:romechurch_bpp2.pvr", texBluePrint));
    }
    else {
        this->tex = Render::CreateResource(TextureSetup::FromFile("tex:romechurch_dxt1.dds", texBluePrint));
    }
    glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetTransform(rot90);
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::Normal, VertexFormat::Float3);
    shapeBuilder.AddSphere(1.0f, 36, 20);
    shapeBuilder.Build();
    Id mesh = Render::CreateResource(MeshSetup::FromData("shape"), shapeBuilder.GetStream());
    Id prog = Render::CreateResource(Shaders::Main::CreateSetup());
    DrawStateSetup dsSetup("ds", mesh, prog, 0);
    dsSetup.DepthStencilState.DepthWriteEnabled = true;
    dsSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = Render::CreateResource(dsSetup);
    
    Render::ReleaseResource(mesh);
    Render::ReleaseResource(prog);
    
    // setup projection and view matrices
    const float32 fbWidth = Render::GetDisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Render::GetDisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnCleanup() {
    // cleanup everything
    Render::ReleaseResource(this->tex);
    Render::ReleaseResource(this->drawState);
    Render::Discard();
    IO::Discard();
    
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
DDSCubeMapApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

