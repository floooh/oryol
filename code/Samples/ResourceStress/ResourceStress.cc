//------------------------------------------------------------------------------
//  ResourceStress.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "IO/IO.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "HttpFS/HTTPFileSystem.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Assets/Gfx/TextureLoader.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;

class ResourceStressApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    void createObjects();
    void updateObjects();
    void showInfo();

    struct Object {
        DrawState drawState;
        ResourceLabel label;
        glm::mat4 modelTransform;
    };
    glm::mat4 computeMVP(const Object& obj);
    
    static const int MaxNumObjects = 1024;
    uint32_t frameCount = 0;
    Id shader;
    Array<Object> objects;
    glm::mat4 view;
    glm::mat4 proj;
    TextureSetup texBlueprint;
};
OryolMain(ResourceStressApp);

//------------------------------------------------------------------------------
AppState::Code
ResourceStressApp::OnInit() {
    // setup IO system
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("tex:", ORYOL_SAMPLE_URL);
    IO::Setup(ioSetup);

    // setup Gfx system
    auto gfxSetup = GfxSetup::Window(600, 400, "Oryol Resource Stress Test");
    gfxSetup.DefaultPassAction = PassAction::Clear(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    gfxSetup.ResourcePoolSize[GfxResourceType::Mesh] = MaxNumObjects + 32;
    gfxSetup.ResourcePoolSize[GfxResourceType::Texture] = MaxNumObjects + 32;
    gfxSetup.ResourcePoolSize[GfxResourceType::Pipeline] = MaxNumObjects + 32;
    gfxSetup.ResourcePoolSize[GfxResourceType::Shader] = 4;
    Gfx::Setup(gfxSetup);
    
    // setup debug text rendering
    Dbg::Setup();    
    
    // setup the shader that is used by all objects
    this->shader = Gfx::CreateResource(Shader::Setup());

    // setup matrices
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    this->texBlueprint.Sampler.MinFilter = TextureFilterMode::LinearMipmapLinear;
    this->texBlueprint.Sampler.MagFilter = TextureFilterMode::Linear;
    this->texBlueprint.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    this->texBlueprint.Sampler.WrapV = TextureWrapMode::ClampToEdge;

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ResourceStressApp::OnRunning() {

    // delete and create objects
    this->frameCount++;
    this->updateObjects();
    this->createObjects();
    this->showInfo();

    Gfx::BeginPass();
    for (const auto& obj : this->objects) {
        // only render objects that have successfully loaded
        const Id& tex = obj.drawState.FSTexture[Shader::tex];
        if (Gfx::QueryResourceInfo(tex).State == ResourceState::Valid) {
            Gfx::ApplyDrawState(obj.drawState);
            Shader::vsParams vsParams;
            vsParams.mvp = this->proj * this->view * obj.modelTransform;
            Gfx::ApplyUniformBlock(vsParams);
            Gfx::Draw();
        }
    }
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // quit or keep running?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ResourceStressApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    IO::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
void
ResourceStressApp::createObjects() {

    if (this->objects.Size() >= MaxNumObjects) {
        return;
    }
    if (Gfx::QueryFreeResourceSlots(GfxResourceType::Mesh) == 0) {
        return;
    }
    if (Gfx::QueryFreeResourceSlots(GfxResourceType::Texture) == 0) {
        return;
    }

    // create a cube object
    // NOTE: we're deliberatly not sharing resources to actually
    // put some stress on the resource system
    Object obj;
    obj.label = Gfx::PushResourceLabel();
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::TexCoord0, VertexFormat::Float2 }
    };
    shapeBuilder.Box(0.1f, 0.1f, 0.1f, 1);
    obj.drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, this->shader);
    obj.drawState.Pipeline = Gfx::CreateResource(ps);
    obj.drawState.FSTexture[Shader::tex] = Gfx::LoadResource(TextureLoader::Create(
        TextureSetup::FromFile(Locator::NonShared("tex:lok_dxt1.dds"), this->texBlueprint)));
    glm::vec3 pos = glm::ballRand(2.0f) + glm::vec3(0.0f, 0.0f, -6.0f);
    obj.modelTransform = glm::translate(glm::mat4(), pos);
    this->objects.Add(obj);
    Gfx::PopResourceLabel();
}

//------------------------------------------------------------------------------
void
ResourceStressApp::updateObjects() {
    for (int i = this->objects.Size() - 1; i >= 0; i--) {
        Object& obj = this->objects[i];
        
        // check if object should be destroyed (it will be
        // destroyed after the texture object had been valid for
        // at least 3 seconds, or if it failed to load)
        const Id& tex = obj.drawState.FSTexture[Shader::tex];
        const auto info = Gfx::QueryResourceInfo(tex);
        if ((info.State == ResourceState::Failed) ||
            ((info.State == ResourceState::Valid) && (info.StateAge > (20 * 60)))) {

            Gfx::DestroyResources(obj.label);
            this->objects.Erase(i);
        }
    }
}

//------------------------------------------------------------------------------
void
ResourceStressApp::showInfo() {
    ResourcePoolInfo texPoolInfo = Gfx::QueryResourcePoolInfo(GfxResourceType::Texture);
    ResourcePoolInfo mshPoolInfo = Gfx::QueryResourcePoolInfo(GfxResourceType::Mesh);
    
    Dbg::PrintF("texture pool\r\n"
                "  num slots: %d, free: %d, used: %d\r\n"
                "  by state:\r\n"
                "    initial: %d\r\n"
                "    setup:   %d\r\n"
                "    pending: %d\r\n"
                "    valid:   %d\r\n"
                "    failed:  %d\r\n\n",
                texPoolInfo.NumSlots, texPoolInfo.NumFreeSlots, texPoolInfo.NumUsedSlots,
                texPoolInfo.NumSlotsByState[ResourceState::Initial],
                texPoolInfo.NumSlotsByState[ResourceState::Setup],
                texPoolInfo.NumSlotsByState[ResourceState::Pending],
                texPoolInfo.NumSlotsByState[ResourceState::Valid],
                texPoolInfo.NumSlotsByState[ResourceState::Failed]);
    
    Dbg::PrintF("mesh pool\r\n"
                "  num slots: %d, free: %d, used: %d\r\n"
                "  by state:\r\n"
                "    initial: %d\r\n"
                "    setup:   %d\r\n"
                "    pending: %d\r\n"
                "    valid:   %d\r\n"
                "    failed:  %d",
                mshPoolInfo.NumSlots, mshPoolInfo.NumFreeSlots, mshPoolInfo.NumUsedSlots,
                mshPoolInfo.NumSlotsByState[ResourceState::Initial],
                mshPoolInfo.NumSlotsByState[ResourceState::Setup],
                mshPoolInfo.NumSlotsByState[ResourceState::Pending],
                mshPoolInfo.NumSlotsByState[ResourceState::Valid],
                mshPoolInfo.NumSlotsByState[ResourceState::Failed]);
}
