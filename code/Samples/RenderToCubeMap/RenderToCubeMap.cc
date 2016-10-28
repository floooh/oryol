//------------------------------------------------------------------------------
//  RenderToCubeMap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "shaders.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/polar_coordinates.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/random.hpp"

using namespace Oryol;

class RenderToCubeMapApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();

    /// handle input to move viewer position
    void handleInput();
    /// update shape orbit positions
    void updateShapes();
    /// draw the environment shapes
    void drawEnvShapes(Id pipeline, const glm::mat4& view, const glm::mat4& proj);

    static const int NumFaces = 6;
    const glm::vec4 ClearColor = glm::vec4(0.5f, 0.5f, 0.7f, 1.0f);

    Id cubeMap;
    Id passes[NumFaces];

    Id shapesMesh;
    Id displayShapesPipeline;
    Id offscreenShapesPipeline;

    DrawState sphereDrawState;

    glm::mat4 displayProj;
    glm::mat4 offscreenProj;
    glm::vec2 polar;
    float distance = 20.0f;

    static const int NumShapes = 64;
    struct {
        glm::vec4 color;
        glm::vec3 axis;
        int shapeIndex = 0;
        float radius = 0.0f;
        float angle = 0.0f;
        float angularVelocity = 0.0f;
    } Shapes[NumShapes];
};
OryolMain(RenderToCubeMapApp);

//------------------------------------------------------------------------------
AppState::Code
RenderToCubeMapApp::OnInit() {
    auto gfxSetup = GfxSetup::WindowMSAA4(800, 600, "Render To CubeMap");
    Gfx::Setup(gfxSetup);
    Input::Setup();

    // create a cubemap which will serve as render target
    auto cubeMapSetup = TextureSetup::RenderTargetCube(1024, 1024, PixelFormat::RGBA8, PixelFormat::DEPTHSTENCIL);
    cubeMapSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    cubeMapSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    this->cubeMap = Gfx::CreateResource(cubeMapSetup);

    // create 6 render passes, one per cube map
    auto rpSetup = RenderPassSetup::From(this->cubeMap, this->cubeMap);
    rpSetup.ColorAttachments[0].DefaultClearColor = ClearColor;
    for (int faceIndex = 0; faceIndex < NumFaces; faceIndex++) {
        rpSetup.ColorAttachments[0].Face = faceIndex;
        this->passes[faceIndex] = Gfx::CreateResource(rpSetup);
    }

    // mesh, shader and pipeline to render color shapes
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Float3);
    shapeBuilder
        .Box(1.0f, 1.0f, 1.0f, 1, true)
        .Cylinder(0.5f, 1.0f, 36, 1, true)
        .Torus(0.25f, 0.5f, 8, 36, true)
        .Sphere(0.5f, 18, 12, true);
    this->shapesMesh = Gfx::CreateResource(shapeBuilder.Build());
    Id envShd = Gfx::CreateResource(ShapeShader::Setup());
    auto pipSetup = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, envShd);
    pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    pipSetup.DepthStencilState.DepthWriteEnabled = true;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->displayShapesPipeline = Gfx::CreateResource(pipSetup);
    pipSetup.RasterizerState.SampleCount = cubeMapSetup.SampleCount;
    this->offscreenShapesPipeline = Gfx::CreateResource(pipSetup);

    // create a sphere where the env-shapes reflect and refract in
    this->sphereDrawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Sphere(3.5f, 36, 24).Build());
    Id sphereShd = Gfx::CreateResource(SphereShader::Setup());
    pipSetup = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, sphereShd);
    pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    pipSetup.DepthStencilState.DepthWriteEnabled = true;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->sphereDrawState.Pipeline = Gfx::CreateResource(pipSetup);
    this->sphereDrawState.FSTexture[Textures::Texture] = this->cubeMap;

    // setup projection matrix for main view
    float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->displayProj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);

    // setup projection matrix for cubemap rendering
    this->offscreenProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 100.0f);
    this->polar = glm::vec2(glm::radians(45.0f), glm::radians(45.0f));

    // setup orbital camera initial position
    this->polar = glm::vec2(glm::radians(45.0f), glm::radians(45.0f));

    // initialize render-shape data
    for (int i = 0; i < NumShapes; i++) {
        auto& shape = this->Shapes[i];
        shape.shapeIndex = i % 4;
        shape.color = glm::linearRand(glm::vec4(0.0f), glm::vec4(1.0f));
        shape.axis = glm::ballRand(1.0f);
        shape.radius = glm::linearRand(5.0f, 10.0f);
        shape.angle = glm::linearRand(0.0f, glm::pi<float>());
        shape.angularVelocity = glm::linearRand(-3.0f, +3.0f);
    }

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
RenderToCubeMapApp::OnRunning() {

    // render environment shapes into cubemap
    const glm::vec3 centerAndUp[NumFaces][2] = {
        { glm::vec3(+1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
        { glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
        { glm::vec3(0.0f, +1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
        { glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
        { glm::vec3(0.0f, 0.0f, +1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
        { glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
    };
    for (int i = 0; i < NumFaces; i++) {
        Gfx::BeginPass(this->passes[i]);
        const glm::mat4 view = glm::lookAt(glm::vec3(0.0f), centerAndUp[i][0], centerAndUp[i][1]);
        this->drawEnvShapes(this->offscreenShapesPipeline, view, this->offscreenProj);
        Gfx::EndPass();
    }

    // render the main view
    Gfx::BeginPass(PassState(ClearColor));

    // draw the environment shapes
    this->handleInput();
    this->updateShapes();
    const glm::vec3 eyePos = glm::euclidean(this->polar) * distance;
    const glm::mat4 view = glm::lookAt(eyePos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->drawEnvShapes(this->displayShapesPipeline, view, this->displayProj);

    // draw the sphere with reflection/refraction from cubemap
    Gfx::ApplyDrawState(this->sphereDrawState);
    SphereShader::VSParams vsParams;
    vsParams.ModelViewProjection = this->displayProj * view;
    vsParams.Model = glm::mat4();
    vsParams.EyePos = eyePos;
    Gfx::ApplyUniformBlock(vsParams);
    Gfx::Draw();

    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
RenderToCubeMapApp::OnCleanup() {
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
void
RenderToCubeMapApp::handleInput() {
    if (Input::MouseAttached()) {
        static const float minLatitude = glm::radians(-85.0f);
        static const float maxLatitude = glm::radians(85.0f);
        static const float minDist = 3.0f;
        static const float maxDist = 20.0f;
        if (Input::MouseButtonPressed(MouseButton::Left)) {
            this->polar.y -= Input::MouseMovement().x * 0.01f;
            this->polar.x = glm::clamp(this->polar.x + Input::MouseMovement().y * 0.01f, minLatitude, maxLatitude);
        }
        this->distance = glm::clamp(this->distance + Input::MouseScroll().y * 0.1f, minDist, maxDist);
    }
}

//------------------------------------------------------------------------------
void
RenderToCubeMapApp::updateShapes() {
    for (auto& shape : this->Shapes) {
        shape.angle += shape.angularVelocity * (1.0f/60.0f);
    }
}

//------------------------------------------------------------------------------
void
RenderToCubeMapApp::drawEnvShapes(Id pipeline, const glm::mat4& view, const glm::mat4& proj) {

    DrawState drawState;
    drawState.Pipeline = pipeline;
    drawState.Mesh[0] = this->shapesMesh;
    Gfx::ApplyDrawState(drawState);
    ShapeShader::VSParams vsParams;
    for (int i = 0; i < NumShapes; i++) {
        const auto& shape = this->Shapes[i];
        glm::mat4 model = glm::rotate(glm::mat4(), shape.angle, shape.axis);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, shape.radius));
        vsParams.ModelViewProjection = proj * view * model;
        vsParams.Color = shape.color;
        Gfx::ApplyUniformBlock(vsParams);
        Gfx::Draw(shape.shapeIndex);
    }
}

