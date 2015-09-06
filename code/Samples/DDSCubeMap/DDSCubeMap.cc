//------------------------------------------------------------------------------
//  DDSCubeMap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Assets/Gfx/TextureLoader.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class DDSCubeMapApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::vec3& pos);
    
    Id drawState;
    Id texBundle;
    glm::mat4 view;
    glm::mat4 proj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
    Shaders::Main::VSParams vsParams;
    ClearState clearState;
};
OryolMain(DDSCubeMapApp);

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnRunning() {
    
    // update rotation angles
    this->angleY += 0.02f;
    this->angleX += 0.01f;
    
    // apply state and draw
    Gfx::ApplyDefaultRenderTarget(this->clearState);
    Gfx::ApplyDrawState(this->drawState);
    
    // check whether the cube map has finished loading
    if (Gfx::QueryResourceInfo(this->texBundle).State == ResourceState::Valid) {
        this->vsParams.ModelViewProjection = this->computeMVP(glm::vec3(0.0f, 0.0f, 0.0f));
        Gfx::ApplyUniformBlock(this->vsParams);
        Gfx::ApplyTextureBundle(this->texBundle);
        Gfx::Draw(0);
    }
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnInit() {

    // setup IO system
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("tex:", ORYOL_SAMPLE_URL);
    IO::Setup(ioSetup);

    // setup rendering system
    auto gfxSetup = GfxSetup::Window(600, 400, "Oryol DXT Cube Map Sample");
    Gfx::Setup(gfxSetup);

    // create resources
    Id shd = Gfx::CreateResource(Shaders::Main::CreateSetup());

    TextureSetup texBluePrint;
    texBluePrint.MinFilter = TextureFilterMode::LinearMipmapLinear;
    texBluePrint.MagFilter = TextureFilterMode::Linear;
    texBluePrint.WrapU = TextureWrapMode::ClampToEdge;
    texBluePrint.WrapV = TextureWrapMode::ClampToEdge;
    StringAtom texPath;
    if (Gfx::QueryFeature(GfxFeature::TextureCompressionPVRTC)) {
        texPath = "tex:romechurch_bpp2.pvr";
    }
    else {
        texPath = "tex:romechurch_dxt1.dds";
    }
    Id tex = Gfx::LoadResource(TextureLoader::Create(TextureSetup::FromFile(texPath, texBluePrint), 0));
    auto tbSetup = TextureBundleSetup::FromShader(shd);
    tbSetup.FS[Shaders::Main::FS_Texture] = tex;
    this->texBundle = Gfx::CreateResource(tbSetup);
    glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Float3);
    shapeBuilder.Transform(rot90).Sphere(1.0f, 36, 20).Build();
    Id mesh = Gfx::CreateResource(shapeBuilder.Result());
    auto dss = DrawStateSetup::FromMeshAndShader(mesh, shd);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = Gfx::CreateResource(dss);
    this->clearState.Color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    
    // setup projection and view matrices
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnCleanup() {
    // cleanup everything
    Gfx::Discard();
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

