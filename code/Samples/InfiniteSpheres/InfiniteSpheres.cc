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

    ShapeBuilder::Result sphere;
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
    auto gfxDesc = GfxDesc::WindowMSAA4(800, 600, "Oryol Infinite Spheres Sample");
    Gfx::Setup(gfxDesc);

    // create 2 ping-pong offscreen render targets, only need 1 depth buffer
    const PixelFormat::Code rtColorFormat = PixelFormat::RGBA8;
    const PixelFormat::Code rtDepthFormat = PixelFormat::DEPTH;
    const int rtWidth = 512;
    const int rtHeight = 512;
    Id rtDepth = Gfx::Texture()
        .RenderTarget(true)
        .Width(rtWidth)
        .Height(rtHeight)
        .Format(rtDepthFormat)
        .Create();
    for (int i = 0; i < 2; i++) {
        this->passInfo[i].texture = Gfx::Texture()
            .RenderTarget(true)
            .Width(rtWidth)
            .Height(rtHeight)
            .Format(rtColorFormat)
            .MinFilter(TextureFilterMode::Linear)
            .MagFilter(TextureFilterMode::Linear)
            .WrapU(TextureWrapMode::Repeat)
            .WrapV(TextureWrapMode::Repeat)
            .Create();
        this->passInfo[i].pass = Gfx::Pass()
            .ColorAttachment(0, this->passInfo[i].texture)
            .DepthStencilAttachment(rtDepth)
            .Create();
    }

    // create a sphere shape mesh
    ShapeBuilder shapeBuilder;
    this->sphere = shapeBuilder
        .AddPositions("in_pos", VertexFormat::Float3)
        .AddNormals("in_normal", VertexFormat::Byte4N)
        .AddTexCoords("in_uv", VertexFormat::Float2)
        .Sphere(0.75f, 72, 40)
        .Build();
    Id vbuf = Gfx::Buffer().From(this->sphere.VertexBufferDesc).Content(this->sphere.Data).Create();
    Id ibuf = Gfx::Buffer().From(this->sphere.IndexBufferDesc).Content(this->sphere.Data).Create();
    this->offscreenDrawState.VertexBuffers[0] = vbuf;
    this->offscreenDrawState.IndexBuffer = ibuf;
    this->displayDrawState.VertexBuffers[0] = vbuf;
    this->displayDrawState.IndexBuffer = ibuf;

    // create shader which is used for both offscreen- and display-rendering
    Id shd = Gfx::CreateShader(Shader::Desc());

    // create draw state for rendering into default render target
    this->displayDrawState.Pipeline = Gfx::Pipeline()
        .Shader(shd)
        .Layout(0, this->sphere.Layout)
        .IndexType(this->sphere.IndexType)
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .SampleCount(gfxDesc.SampleCount)
        .Create();
    this->offscreenDrawState.Pipeline = Gfx::Pipeline()
        .Shader(shd)
        .Layout(0, this->sphere.Layout)
        .IndexType(this->sphere.IndexType)
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .ColorFormat(rtColorFormat)
        .DepthFormat(rtDepthFormat)
        .Create();

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
    Gfx::Draw(this->sphere.PrimitiveGroups[0]);
    Gfx::EndPass();
    
    // ...and again to display
    model = this->computeModel(-this->angleX, -this->angleY, glm::vec3(0.0f, 0.0f, -2.0f));
    this->vsParams.mvp = this->computeMVP(this->displayProj, model);
    Gfx::BeginPass(this->passAction);
    this->displayDrawState.FSTexture[Shader::tex] = this->passInfo[index0].texture;
    Gfx::ApplyDrawState(this->displayDrawState);
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::Draw(this->sphere.PrimitiveGroups[0]);
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

