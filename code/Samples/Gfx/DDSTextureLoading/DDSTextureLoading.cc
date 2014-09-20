//------------------------------------------------------------------------------
//  DDSTextureLoading.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
#include "Gfx/Gfx.h"
#include "Gfx/Util/RawMeshLoader.h"
#include "Gfx/Util/ShapeBuilder.h"
#include "Gfx/Util/TextureLoader.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <array>
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
    GfxId drawState;
    static const int32 NumTextures = 15;
    std::array<GfxId, NumTextures> texId;
    glm::mat4 view;
    glm::mat4 proj;
};
OryolMain(DDSTextureLoadingApp);

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnRunning() {
    // render one frame
    if (Gfx::BeginFrame()) {
        
        this->distVal += 0.01f;
        
        Gfx::ApplyDefaultRenderTarget();
        Gfx::ApplyDrawState(this->drawState);
        Gfx::Clear(PixelChannel::All, glm::vec4(0.5f), 1.0f, 0);
        
        // only render when texture is loaded (until texture placeholder are implemented)
        static const std::array<glm::vec3, NumTextures> pos{ {
            // dxt1, dxt3, dxt5
            glm::vec3(-2.2f, +1.1f, 0.0f),
            glm::vec3(-1.1f, +1.1f, 0.0f),
            glm::vec3( 0.0f, +1.1f, 0.0f),
            
            // pvr2bpp pvr4bpp
            glm::vec3(+1.1f, +1.1f, 0.0f),
            glm::vec3(+2.2f, +1.1f, 0.0f),
            
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
        } };
        for (int32 i = 0; i < NumTextures; i++) {
            const GfxId& tex = this->texId[i];
            if (tex.IsValid()) {
                const auto resState = Gfx::QueryResourceState(tex);
                if (resState == ResourceState::Valid) {
                    glm::vec3 p = pos[i] + glm::vec3(0.0f, 0.0f, -20.0f + glm::sin(this->distVal) * 19.0f);
                    Gfx::ApplyVariable(Shaders::Main::ModelViewProjection, this->computeMVP(p));
                    Gfx::ApplyVariable(Shaders::Main::Texture, tex);
                    Gfx::Draw(0);
                }
            }
        }
        Gfx::EndFrame();
    }
    
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
    auto gfxSetup = GfxSetup::Window(600, 400, false, "Oryol DDS Loading Sample");
    gfxSetup.Loaders.Add(RawMeshLoader::Creator());
    gfxSetup.Loaders.Add(TextureLoader::Creator());
    Gfx::Setup(gfxSetup);

    // setup resources
    TextureSetup texBluePrint;
    texBluePrint.MinFilter = TextureFilterMode::LinearMipmapLinear;
    texBluePrint.MagFilter = TextureFilterMode::Linear;
    texBluePrint.WrapU = TextureWrapMode::ClampToEdge;
    texBluePrint.WrapV = TextureWrapMode::ClampToEdge;
    this->texId[0]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_dxt1.dds", texBluePrint));
    this->texId[1]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_dxt3.dds", texBluePrint));
    this->texId[2]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_dxt5.dds", texBluePrint));
    this->texId[3]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_bpp2.pvr", texBluePrint));
    this->texId[4]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_bpp4.pvr", texBluePrint));
    this->texId[5]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_rgba8.dds", texBluePrint));
    this->texId[6]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_bgra8.dds", texBluePrint));
    this->texId[7]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_rgb8.dds", texBluePrint));
    this->texId[8]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_bgr8.dds", texBluePrint));
    this->texId[9]  = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_argb4.dds", texBluePrint));
    this->texId[10] = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_abgr4.dds", texBluePrint));
    this->texId[11] = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_argb1555.dds", texBluePrint));
    this->texId[12] = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_abgr1555.dds", texBluePrint));
    this->texId[13] = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_rgb565.dds", texBluePrint));
    this->texId[14] = Gfx::CreateResource(TextureSetup::FromFile("tex:lok_bgr565.dds", texBluePrint));

    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Transform(rot90).Plane(1.0f, 1.0f, 4).Build();
    GfxId mesh = Gfx::CreateResource(MeshSetup::FromStream(), shapeBuilder.Result());
    GfxId prog = Gfx::CreateResource(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = Gfx::CreateResource(dss);
    
    const float32 fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnCleanup() {
    // cleanup everything
    for (auto& tex : this->texId) {
        if (tex.IsValid()) {
            tex.Release();
        }
    }
    this->drawState.Release();
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

