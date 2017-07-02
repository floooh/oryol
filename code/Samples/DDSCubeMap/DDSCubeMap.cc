//------------------------------------------------------------------------------
//  DDSCubeMap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "IO/IO.h"
#include "HttpFS/HTTPFileSystem.h"
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
    
    glm::mat4 computeMVP(const glm::vec3& pos);

    DrawState drawState;
    Shader::vsParams vsParams;
    glm::mat4 view;
    glm::mat4 proj;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(DDSCubeMapApp);

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
    gfxSetup.DefaultPassAction = PassAction::Clear(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    Gfx::Setup(gfxSetup);

    // create resources
    Id shd = Gfx::CreateResource(Shader::Setup());

    TextureSetup texBluePrint;
    texBluePrint.Sampler.MinFilter = TextureFilterMode::LinearMipmapLinear;
    texBluePrint.Sampler.MagFilter = TextureFilterMode::Linear;
    texBluePrint.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    texBluePrint.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    StringAtom texPath;
    if (Gfx::QueryFeature(GfxFeature::TextureCompressionPVRTC)) {
        texPath = "tex:romechurch_bpp2.pvr";
    }
    else {
        texPath = "tex:romechurch_dxt1.dds";
    }
    this->drawState.FSTexture[Shader::tex] = Gfx::LoadResource(
        TextureLoader::Create(TextureSetup::FromFile(texPath, texBluePrint))
    );
    glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Normal, VertexFormat::Float3 }
    };
    shapeBuilder.Transform(rot90).Sphere(1.0f, 36, 20);
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState.Pipeline = Gfx::CreateResource(ps);

    // setup projection and view matrices
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnRunning() {
    
    // update rotation angles
    this->angleY += 0.02f;
    this->angleX += 0.01f;
    
    Gfx::BeginPass();
    const Id& tex = this->drawState.FSTexture[Shader::tex];
    if (Gfx::QueryResourceInfo(tex).State == ResourceState::Valid) {
        this->vsParams.mvp = this->computeMVP(glm::vec3(0.0f, 0.0f, 0.0f));
        Gfx::ApplyDrawState(this->drawState);
        Gfx::ApplyUniformBlock(this->vsParams);
        Gfx::Draw();
    }
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
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

