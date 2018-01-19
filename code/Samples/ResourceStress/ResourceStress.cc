//------------------------------------------------------------------------------
//  ResourceStress.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "IO/IO.h"
#include "Gfx/Gfx.h"
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

    struct Object {
        uint32_t createdFrame = 0;
        PrimitiveGroup primGroup;
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
    auto gfxDesc = GfxDesc::Window(600, 400, "Oryol Resource Stress Test");
    gfxDesc.ResourcePoolSize[GfxResourceType::Buffer] = 2 * (MaxNumObjects + 32);
    gfxDesc.ResourcePoolSize[GfxResourceType::Texture] = MaxNumObjects + 32;
    gfxDesc.ResourcePoolSize[GfxResourceType::Pipeline] = MaxNumObjects + 32;
    gfxDesc.ResourcePoolSize[GfxResourceType::Shader] = 4;
    Gfx::Setup(gfxDesc);
    
    // setup the shader that is used by all objects
    this->shader = Gfx::CreateShader(Shader::Desc());

    // setup matrices
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ResourceStressApp::OnRunning() {

    // delete and create objects
    this->frameCount++;
    this->updateObjects();
    this->createObjects();

    Gfx::BeginPass(PassAction::Clear(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)));
    for (const auto& obj : this->objects) {
        // only render objects that have successfully loaded (technically
        // the check is not necessary since rendering for non-valid resources
        // will be skipped anyway, but this way we have test coverage for
        // Gfx::QueryResourceState()
        const Id& tex = obj.drawState.FSTexture[Shader::tex];
        if (Gfx::QueryResourceState(tex) == ResourceState::Valid) {
            Gfx::ApplyDrawState(obj.drawState);
            Shader::vsParams vsParams;
            vsParams.mvp = this->proj * this->view * obj.modelTransform;
            Gfx::ApplyUniformBlock(vsParams);
            Gfx::Draw(obj.primGroup);
        }
    }
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // quit or keep running?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ResourceStressApp::OnCleanup() {
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

    // create a cube object
    // NOTE: we're deliberatly not sharing resources to actually
    // put some stress on the resource system
    Object obj;
    obj.label = Gfx::PushResourceLabel();
    obj.createdFrame = this->frameCount;
    auto shape = ShapeBuilder::New()
        .Positions("position", VertexFormat::Float3)
        .TexCoords("texcoord0", VertexFormat::Float2)
        .Box(0.1f, 0.1f, 0.1f, 1)
        .Build();
    obj.primGroup = shape.PrimitiveGroups[0];
    obj.drawState.VertexBuffers[0] = Gfx::Buffer()
        .From(shape.VertexBufferDesc)
        .Content(shape.Data)
        .Create();
    obj.drawState.IndexBuffer = Gfx::Buffer()
        .From(shape.IndexBufferDesc)
        .Content(shape.Data)
        .Create();
    obj.drawState.Pipeline = Gfx::Pipeline()
        .From(shape.PipelineDesc)
        .Shader(this->shader)
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .CullFaceEnabled(true)
        .Create();
    obj.drawState.FSTexture[Shader::tex] = TextureLoader::Load(Gfx::Texture()
        .Locator(Locator::NonShared("tex:lok_dxt1.dds"))
        .MinFilter(TextureFilterMode::LinearMipmapLinear)
        .MagFilter(TextureFilterMode::Linear)
        .WrapU(TextureWrapMode::ClampToEdge)
        .WrapV(TextureWrapMode::ClampToEdge)
        .Desc);
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
        int age = this->frameCount - obj.createdFrame;
        
        // check if object should be destroyed (it will be
        // destroyed after the texture object had been valid for
        // at least 3 seconds, or if it failed to load)
        const Id& tex = obj.drawState.FSTexture[Shader::tex];
        ResourceState::Code state = Gfx::QueryResourceState(tex);
        if ((state == ResourceState::Failed) ||
            ((state == ResourceState::Valid) && (age > (20 * 60)))) {

            Gfx::DestroyResources(obj.label);
            this->objects.Erase(i);
        }
    }
}

