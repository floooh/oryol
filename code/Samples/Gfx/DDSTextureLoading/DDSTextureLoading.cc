//------------------------------------------------------------------------------
//  DDSTextureLoading.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Core/Containers/StaticArray.h"
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
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
private:
    glm::mat4 computeMVP(const glm::vec3& pos);
    
    float32 distVal = 0.0f;
    Id drawState;
    static const int32 NumTextures = 16;
    StaticArray<Id, NumTextures> texId;
    glm::mat4 view;
    glm::mat4 proj;
};
OryolMain(DDSTextureLoadingApp);

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnRunning() {
    
    this->distVal += 0.01f;
    
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::Clear(PixelChannel::All, glm::vec4(0.5f));
    
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
    for (int32 i = 0; i < NumTextures; i++) {
        const Id& tex = this->texId[i];
        if (tex.IsValid()) {
            const auto resState = Gfx::Resource().QueryState(tex);
            if (resState == ResourceState::Valid) {
                glm::vec3 p = pos[i] + glm::vec3(0.0f, 0.0f, -20.0f + glm::sin(this->distVal) * 19.0f);
                Gfx::ApplyVariable(Shaders::Main::ModelViewProjection, this->computeMVP(p));
                Gfx::ApplyVariable(Shaders::Main::Texture, tex);
                Gfx::Draw(0);
            }
        }
    }
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnInit() {

    // setup IO system
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("tex:", ORYOL_SAMPLE_URL);
    IO::Setup(ioSetup);

    // setup rendering system
    auto gfxSetup = GfxSetup::Window(600, 400, "Oryol DDS Loading Sample");
    Gfx::Setup(gfxSetup);
    Gfx::Resource().AttachLoader(TextureLoader::Create());

    // setup resources
    TextureSetup texBluePrint;
    texBluePrint.MinFilter = TextureFilterMode::LinearMipmapLinear;
    texBluePrint.MagFilter = TextureFilterMode::Linear;
    texBluePrint.WrapU = TextureWrapMode::ClampToEdge;
    texBluePrint.WrapV = TextureWrapMode::ClampToEdge;
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
    for (int32 i = 0; i < NumTextures; i++) {
        this->texId[i] = Gfx::Resource().Load(TextureSetup::FromFile(paths[i], texBluePrint));
    }

    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Transform(rot90).Plane(1.0f, 1.0f, 4).Build();
    Id mesh = Gfx::Resource().Create(shapeBuilder.Result());
    Id prog = Gfx::Resource().Create(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = Gfx::Resource().Create(dss);
    
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
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
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    return this->proj * this->view * modelTform;
}

