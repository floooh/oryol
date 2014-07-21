//------------------------------------------------------------------------------
//  DDSTextureLoading.cc
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

class DDSTextureLoadingApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::vec3& pos);
    
    float32 distVal = 0.0f;
    IOFacade* io = nullptr;
    RenderFacade* render = nullptr;
    Resource::Id drawState;
    static const int32 NumTextures = 15;
    std::array<Resource::Id, NumTextures> texId;
    glm::mat4 view;
    glm::mat4 proj;
};
OryolMain(DDSTextureLoadingApp);

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnInit() {

    // setup IO system
    this->io = IOFacade::CreateSingle();
    this->io->RegisterFileSystem("http", Creator<HTTPFileSystem, FileSystem>());
    this->io->SetAssign("tex:", "http://localhost:8000/");

    // setup rendering system
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(600, 400, "Oryol DDS Loading Sample"));
    this->render->AttachLoader(RawMeshLoader::Create());
    this->render->AttachLoader(TextureLoader::Create());
    float32 fbWidth = this->render->GetDisplayAttrs().FramebufferWidth;
    float32 fbHeight = this->render->GetDisplayAttrs().FramebufferHeight;

    // setup resources
    TextureSetup texBluePrint;
    texBluePrint.MinFilter = TextureFilterMode::LinearMipmapLinear;
    texBluePrint.MagFilter = TextureFilterMode::Linear;
    texBluePrint.WrapU = TextureWrapMode::ClampToEdge;
    texBluePrint.WrapV = TextureWrapMode::ClampToEdge;
    this->texId[0]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_dxt1.dds", texBluePrint));
    this->texId[1]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_dxt3.dds", texBluePrint));
    this->texId[2]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_dxt5.dds", texBluePrint));
    this->texId[3]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_bpp2.pvr", texBluePrint));
    this->texId[4]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_bpp4.pvr", texBluePrint));
    this->texId[5]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_rgba8.dds", texBluePrint));
    this->texId[6]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_bgra8.dds", texBluePrint));
    this->texId[7]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_rgb8.dds", texBluePrint));
    this->texId[8]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_bgr8.dds", texBluePrint));
    this->texId[9]  = this->render->CreateResource(TextureSetup::FromFile("tex:lok_argb4.dds", texBluePrint));
    this->texId[10] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_abgr4.dds", texBluePrint));
    this->texId[11] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_argb1555.dds", texBluePrint));
    this->texId[12] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_abgr1555.dds", texBluePrint));
    this->texId[13] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_rgb565.dds", texBluePrint));
    this->texId[14] = this->render->CreateResource(TextureSetup::FromFile("tex:lok_bgr565.dds", texBluePrint));

    glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.SetTransform(rot90);
    shapeBuilder.VertexLayout().Add(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.VertexLayout().Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.AddPlane(1.0f, 1.0f, 4);
    shapeBuilder.Build();
    Id mesh = this->render->CreateResource(MeshSetup::FromData("shape"), shapeBuilder.GetStream());
    Id prog = this->render->CreateResource(Shaders::Main::CreateSetup());
    DrawStateSetup dsSetup("ds", mesh, prog, 0);
    dsSetup.DepthStencilState.SetDepthWriteEnabled(true);
    dsSetup.DepthStencilState.SetDepthCompareFunc(CompareFunc::LessEqual);
    this->drawState = this->render->CreateResource(dsSetup);
    
    this->render->ReleaseResource(mesh);
    this->render->ReleaseResource(prog);
    
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
glm::mat4
DDSTextureLoadingApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
    
        this->distVal += 0.01f;
        
        this->render->ApplyDefaultRenderTarget();
        this->render->ApplyDrawState(this->drawState);
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        this->render->ApplyState(Render::State::ClearColor, 0.5f, 0.5f, 0.5f, 0.0f);
        this->render->Clear(true, true, true);
        
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
            const Id& tex = this->texId[i];
            if (tex.IsValid()) {
                const auto resState = this->render->QueryResourceState(tex);
                if (resState == Resource::State::Valid) {
                    glm::vec3 p = pos[i] + glm::vec3(0.0f, 0.0f, -20.0f + glm::sin(this->distVal) * 19.0f);
                    this->render->ApplyVariable(Shaders::Main::ModelViewProjection, this->computeMVP(p));
                    this->render->ApplyVariable(Shaders::Main::Texture, tex);
                    this->render->Draw(0);
                }
            }
        }
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DDSTextureLoadingApp::OnCleanup() {
    // cleanup everything
    for (auto tex : this->texId) {
        if (tex.IsValid()) {
            this->render->ReleaseResource(tex);
        }
    }
    this->render->ReleaseResource(this->drawState);
    this->render = nullptr;
    RenderFacade::DestroySingle();
    this->io = nullptr;
    IOFacade::DestroySingle();
    
    return App::OnCleanup();
}
