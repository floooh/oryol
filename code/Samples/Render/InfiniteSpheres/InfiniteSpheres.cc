//------------------------------------------------------------------------------
//  InfiniteSpheres.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/Render.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;

class InfiniteSpheresApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    glm::mat4 computeModel(float32 rotX, float32 rotY, const glm::vec3& pos);
    glm::mat4 computeMVP(const glm::mat4& proj, const glm::mat4& model);

    Id renderTargets[2];
    Id drawState;
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
    int32 frameIndex = 0;
};
OryolMain(InfiniteSpheresApp);

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnRunning() {
    // render one frame
    if (Render::BeginFrame()) {
        
        // update angles
        this->angleY += 0.01f;
        this->angleX += 0.02f;
        this->frameIndex++;
        const int32 index0 = this->frameIndex % 2;
        const int32 index1 = (this->frameIndex + 1) % 2;
        
        // generall state
        Render::ApplyDrawState(this->drawState);
        
        // render sphere to offscreen render target, using the other render target as
        // source texture
        Render::ApplyOffscreenRenderTarget(this->renderTargets[index0]);
        Render::Clear(PixelChannel::All, glm::vec4(0.0f), 1.0f, 0);
        glm::mat4 model = this->computeModel(this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -2.0f));
        glm::mat4 mvp = this->computeMVP(this->offscreenProj, model);
        Render::ApplyVariable(Shaders::Main::ModelViewProjection, mvp);
        Render::ApplyVariable(Shaders::Main::Texture, this->renderTargets[index1]);
        Render::Draw(0);
        
        // ...and again to display
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::All, glm::vec4(0.25f), 1.0f, 0);
        model = this->computeModel(-this->angleX, -this->angleY, glm::vec3(0.0f, 0.0f, -2.0f));
        mvp = this->computeMVP(this->displayProj, model);
        Render::ApplyVariable(Shaders::Main::ModelViewProjection, mvp);
        Render::ApplyVariable(Shaders::Main::Texture, this->renderTargets[index0]);
        Render::Draw(0);
        
        Render::EndFrame();
    }
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnInit() {
    // setup rendering system
    auto renderSetup = RenderSetup::Window(800, 600, true, "Oryol Infinite Spheres Sample");
    renderSetup.Loaders.Add(RawMeshLoader::Creator());
    Render::Setup(renderSetup);

    // create resources
    for (int32 i = 0; i < 2; i++) {
        auto rtSetup = TextureSetup::RenderTarget(512, 512);
        rtSetup.ColorFormat = PixelFormat::RGB8;
        rtSetup.DepthFormat = PixelFormat::D16;
        rtSetup.MinFilter = TextureFilterMode::Linear;
        rtSetup.MagFilter = TextureFilterMode::Linear;
        rtSetup.WrapU = TextureWrapMode::Repeat;
        rtSetup.WrapV = TextureWrapMode::Repeat;
        this->renderTargets[i] = Render::CreateResource(rtSetup);
    }
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Sphere(0.75f, 72.0f, 40.0f).Build();
    Id sphere = Render::CreateResource(MeshSetup::FromStream(), shapeBuilder.Result());
    Id prog = Render::CreateResource(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(sphere, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->drawState = Render::CreateResource(dss);
    
    Render::ReleaseResource(sphere);
    Render::ReleaseResource(prog);
    
    // setup static transform matrices
    const float32 fbWidth = Render::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = Render::DisplayAttrs().FramebufferHeight;
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 20.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnCleanup() {
    // cleanup everything
    Render::ReleaseResource(this->drawState);
    for (int32 i = 0; i < 2; i++) {
        Render::ReleaseResource(this->renderTargets[i]);
    }
    Render::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
InfiniteSpheresApp::computeModel(float32 rotX, float32 rotY, const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    return modelTform;
}

//------------------------------------------------------------------------------
glm::mat4
InfiniteSpheresApp::computeMVP(const glm::mat4& proj, const glm::mat4& modelTform) {
    return proj * this->view * modelTform;
}

