//------------------------------------------------------------------------------
//  DDSTextureLoading.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/Containers/StaticArray.h"
#include "IO/IO.h"
#include "HttpFS/HTTPFileSystem.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Assets/Gfx/TextureLoader.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class DDSTextureLoadingApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    glm::mat4 computeMVP(const glm::vec3& pos);
    
    float distVal = 0.0f;
    PrimitiveGroup primGroup;
    DrawState drawState;
    static const int NumTextures = 16;
    StaticArray<Id, NumTextures> textures;
    Shader::vsParams vsParams;
};
OryolMain(DDSTextureLoadingApp);

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnInit() {

    // setup IO system
    IO::Setup(IODesc()
        .FileSystem("http", HTTPFileSystem::Creator())
        .Assign("tex:", ORYOL_SAMPLE_URL));

    // setup rendering system
    Gfx::Setup(GfxDesc()
        .Width(600).Height(400)
        .Title("Oryol DDS Loading Sample")
        .HtmlTrackElementSize(true));

    // setup resources
    static const char *paths[NumTextures] = {
        "tex:lok_dxt1.dds",
        "tex:lok_dxt3.dds",
        "tex:lok_dxt5.dds",
        "tex:lok_bpp2.pvr",
        "tex:lok_bpp4.pvr",
        "tex:lok_etc2.ktx",
        "tex:lok_rgba8.dds",
        "tex:lok_bgra8.dds",
        "tex:lok_rgb8.dds",
        "tex:lok_bgr8.dds",
        "tex:lok_argb4.dds",
        "tex:lok_abgr4.dds",
        "tex:lok_argb1555.dds",
        "tex:lok_abgr1555.dds",
        "tex:lok_rgb565.dds",
        "tex:lok_bgr565.dds",
    };
    for (int i = 0; i < NumTextures; i++) {
        this->textures[i] = TextureLoader::Load(TextureDesc()
            .Locator(paths[i])
            .MinFilter(TextureFilterMode::LinearMipmapLinear)
            .MagFilter(TextureFilterMode::Linear)
            .WrapU(TextureWrapMode::ClampToEdge)
            .WrapV(TextureWrapMode::ClampToEdge));
    }

    auto shape = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .TexCoords("in_uv", VertexFormat::Float2)
        .Transform(glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))
        .Plane(1.0f, 1.0f, 4)
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
DDSTextureLoadingApp::OnRunning() {
    
    this->distVal += 0.01f;
    
    Gfx::BeginPass(PassAction().Clear(0.5f, 0.5f, 0.5f, 1.0f));

    // only render when texture is loaded (until texture placeholder are implemented)
    static const glm::vec3 pos[NumTextures] = {
        // dxt1, dxt3, dxt5, pvr2, pvr4, etc2
        glm::vec3(-2.75f, +1.1f, 0.0f),
        glm::vec3(-1.65f, +1.1f, 0.0f),
        glm::vec3(-0.55f, +1.1f, 0.0f),
        glm::vec3(+0.55f, +1.1f, 0.0f),
        glm::vec3(+1.65f, +1.1f, 0.0f),
        glm::vec3(+2.75f, +1.1f, 0.0f),
        
        // rgba8, bgra8, rgb8, bgr8
        glm::vec3(-1.65f, 0.0f, 0.0f),
        glm::vec3(-0.55f, 0.0f, 0.0f),
        glm::vec3(+0.55f, 0.0f, 0.0f),
        glm::vec3(+1.65f, 0.0f, 0.0f),
        
        // rgba4444, bgra4444, rgba5551, bgra5551, rgb565, bgr565
        glm::vec3(-2.75f, -1.1f, 0.0f),
        glm::vec3(-1.65f, -1.1f, 0.0f),
        glm::vec3(-0.55f, -1.1f, 0.0f),
        glm::vec3(+0.55f, -1.1f, 0.0f),
        glm::vec3(+1.65f, -1.1f, 0.0f),
        glm::vec3(+2.75f, -1.1f, 0.0f)
    };
    for (int i = 0; i < NumTextures; i++) {
        glm::vec3 p = pos[i] + glm::vec3(0.0f, 0.0f, -20.0f + glm::sin(this->distVal) * 19.0f);
        this->vsParams.mvp = this->computeMVP(p);
        this->drawState.FSTexture[Shader::tex] = this->textures[i];
        Gfx::ApplyDrawState(this->drawState);
        Gfx::ApplyUniformBlock(this->vsParams);
        Gfx::Draw(this->primGroup);
    }
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnCleanup() {
    Gfx::Discard();
    IO::Discard();    
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
DDSTextureLoadingApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), float(Gfx::Width()), float(Gfx::Height()), 0.01f, 100.0f);
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    return proj * modelTform;
}

