//------------------------------------------------------------------------------
//  InfiniteSpheres.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;

class InfiniteSpheresApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
    
    glm::mat4 computeModel(float rotX, float rotY, const glm::vec3& pos);
    glm::mat4 computeMVP(const glm::mat4& proj, const glm::mat4& model);

    DrawState offscreenDrawState;
    DrawState displayDrawState;
    struct {
        Id texture;
        Id pass;
    } passInfo[2];
    Shader::vsParams vsParams;
    PassAction passAction = PassAction::Clear(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float angleX = 0.0f;
    float angleY = 0.0f;
    int frameIndex = 0;
};
OryolMain(InfiniteSpheresApp);

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnInit() {
    // setup rendering system
    auto gfxSetup = GfxSetup::WindowMSAA4(800, 600, "Oryol Infinite Spheres Sample");
    Gfx::Setup(gfxSetup);

    // create 2 ping-pong offscreen render targets
    auto rtSetup = TextureSetup::RenderTarget2D(512, 512, PixelFormat::RGBA8, PixelFormat::DEPTH);
    rtSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    rtSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    rtSetup.Sampler.WrapU = TextureWrapMode::Repeat;
    rtSetup.Sampler.WrapV = TextureWrapMode::Repeat;
    for (int i = 0; i < 2; i++) {
        Id tex = Gfx::CreateResource(rtSetup);
        this->passInfo[i].texture = tex;
        auto rpSetup = PassSetup::From(tex, tex);
        this->passInfo[i].pass = Gfx::CreateResource(rpSetup);
    }

    // create a sphere shape mesh
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Normal, VertexFormat::Byte4N },
        { VertexAttr::TexCoord0, VertexFormat::Float2 }
    };
    shapeBuilder.Sphere(0.75f, 72, 40);
    Id sphere = Gfx::CreateResource(shapeBuilder.Build());
    this->offscreenDrawState.Mesh[0] = sphere;
    this->displayDrawState.Mesh[0] = sphere;

    // create shader which is used for both offscreen- and display-rendering
    Id shd = Gfx::CreateResource(Shader::Setup());

    // create draw state for rendering into default render target
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    ps.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->displayDrawState.Pipeline = Gfx::CreateResource(ps);

    // create draw state for rendering into offscreen render target
    ps.BlendState.ColorFormat = rtSetup.ColorFormat;
    ps.BlendState.DepthFormat = rtSetup.DepthFormat;
    ps.RasterizerState.SampleCount = 1;
    this->offscreenDrawState.Pipeline = Gfx::CreateResource(ps);

    // setup static transform matrices
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 20.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnRunning() {
    
    // update angles
    this->angleY += 0.01f;
    this->angleX += 0.02f;
    this->frameIndex++;
    const int index0 = this->frameIndex % 2;
    const int index1 = (this->frameIndex + 1) % 2;

    // render sphere to offscreen render target, using the other render target as
    // source texture
    glm::mat4 model = this->computeModel(this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -2.0f));
    this->vsParams.mvp = this->computeMVP(this->offscreenProj, model);
    Gfx::BeginPass(this->passInfo[index0].pass);
    this->offscreenDrawState.FSTexture[Shader::tex] = this->passInfo[index1].texture;
    Gfx::ApplyDrawState(this->offscreenDrawState);
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::Draw();
    Gfx::EndPass();
    
    // ...and again to display
    model = this->computeModel(-this->angleX, -this->angleY, glm::vec3(0.0f, 0.0f, -2.0f));
    this->vsParams.mvp = this->computeMVP(this->displayProj, model);
    Gfx::BeginPass(this->passAction);
    this->displayDrawState.FSTexture[Shader::tex] = this->passInfo[index0].texture;
    Gfx::ApplyDrawState(this->displayDrawState);
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::Draw();
    Gfx::EndPass();
    
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
InfiniteSpheresApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
InfiniteSpheresApp::computeModel(float rotX, float rotY, const glm::vec3& pos) {
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

