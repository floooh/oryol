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

    PrimitiveGroup primGroup;
    DrawState drawState;
    Shader::vsParams vsParams;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(DDSCubeMapApp);

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnInit() {

    // setup IO system
    IO::Setup(IODesc()
        .FileSystem("http", HTTPFileSystem::Creator())
        .Assign("tex:", ORYOL_SAMPLE_URL));

    // setup rendering system
    Gfx::Setup(GfxDesc()
        .Width(600).Height(400)
        .Title("Oryol DXT Cube Map Sample")
        .HtmlTrackElementSize(true));

    // create resources
    StringAtom texPath;
    if (Gfx::QueryFeature(GfxFeature::TextureCompressionPVRTC)) {
        texPath = "tex:romechurch_bpp2.pvr";
    }
    else {
        texPath = "tex:romechurch_dxt1.dds";
    }
    this->drawState.FSTexture[Shader::tex] = TextureLoader::Load(TextureDesc()
        .Locator(texPath)
        .MinFilter(TextureFilterMode::LinearMipmapLinear)
        .MagFilter(TextureFilterMode::Linear)
        .WrapU(TextureWrapMode::ClampToEdge)
        .WrapV(TextureWrapMode::ClampToEdge));

    auto shape = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .Normals("in_normal", VertexFormat::Float3)
        .Transform(glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))
        .Sphere(1.0f, 36, 20)
        .Build();
    this->primGroup = shape.PrimitiveGroups[0];
    this->drawState.VertexBuffers[0] = Gfx::CreateBuffer(shape.VertexBufferDesc);
    this->drawState.IndexBuffer = Gfx::CreateBuffer(shape.IndexBufferDesc);
    this->drawState.Pipeline = Gfx::CreatePipeline(PipelineDesc(shape.PipelineDesc)
        .Shader(Gfx::CreateShader(Shader::Desc()))
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual));

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DDSCubeMapApp::OnRunning() {
    
    // update rotation angles
    this->angleY += 0.02f;
    this->angleX += 0.01f;
    
    Gfx::BeginPass(PassAction().Clear(0.5f, 0.5f, 0.5f, 1.0f));
    this->vsParams.mvp = this->computeMVP(glm::vec3(0.0f, 0.0f, 0.0f));
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::Draw(this->primGroup);
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
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), float(Gfx::Width()), float(Gfx::Height()), 0.01f, 100.0f);
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return proj * modelTform;
}

