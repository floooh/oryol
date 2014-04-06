//------------------------------------------------------------------------------
//  SimpleRenderTarget.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Application/App.h"
#include "Render/RenderFacade.h"
#include "Render/Util/RawMeshLoader.h"
#include "Render/Util/ShapeBuilder.h"
#define GLM_FORCE_RADIANS
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace Oryol;
using namespace Oryol::Application;
using namespace Oryol::Render;
using namespace Oryol::Resource;

OryolApp("SimpleRenderTarget", "1.0");

// derived application class
class SimpleRenderTargetApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    glm::mat4 computeMVP(const glm::mat4& proj, float32 rotX, float32 rotY, const glm::vec3& pos);

    RenderFacade* render = nullptr;
    Resource::Id renderTarget;
    Resource::Id sphere;
    Resource::Id torus;
    Resource::Id rtProg;
    Resource::Id dispProg;
    glm::mat4 view;
    glm::mat4 offscreenProj;
    glm::mat4 displayProj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
    
    // shader slots
    static const int32 ModelViewProjection = 0;
    static const int32 Texture = 1;
};

// vertex shader for rendering to render target
static const char* rtVsSource =
"uniform mat4 mvp;\n"
"VS_INPUT(vec4, position);\n"
"VS_INPUT(vec4, normal);\n"
"VS_OUTPUT(vec4, nrm);\n"
"void main() {\n"
"  gl_Position = mvp * position;\n"
"  nrm = normal;\n"
"}\n";

// fragment shader for rendering to render target
static const char* rtFsSource =
"FS_INPUT(vec4, nrm);\n"
"void main() {\n"
"  FragmentColor = nrm * 0.5 + 0.5;\n"
"}\n";

// vertex shader for rendering to display
static const char* dispVsSource =
"uniform mat4 mvp;\n"
"VS_INPUT(vec4, position);\n"
"VS_INPUT(vec4, normal);\n"
"VS_INPUT(vec2, texcoord0);\n"
"VS_OUTPUT(vec4, nrm);\n"
"VS_OUTPUT(vec2, uv);\n"
"void main() {\n"
"  gl_Position = mvp * position;\n"
"  uv = texcoord0;\n"
"  nrm = normalize(mvp * normal);\n"
"}\n";

// fragment shader for rendering to display
static const char* dispFsSource =
"uniform sampler2D tex;\n"
"FS_INPUT(vec2, uv);\n"
"FS_INPUT(vec4, nrm);\n"
"void main() {\n"
"  vec4 c = TEXTURE2D(tex, uv * vec2(20.0, 10.0));\n"
"  float l = clamp(dot(nrm.xyz, normalize(vec3(1.0, 1.0, -1.0))), 0.0, 1.0) * 2.0;\n"
"  FragmentColor = c * (l + 0.25);\n"
"}\n";

//------------------------------------------------------------------------------
void
OryolMain() {
    // execution starts here, create our app and start the main loop
    SimpleRenderTargetApp app;
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnInit() {
    // setup rendering system
    this->render = RenderFacade::CreateSingleton();
    this->render->AttachLoader(RawMeshLoader::Create());
    this->render->Setup(RenderSetup::Windowed(800, 600, "Oryol Simple Render Target Sample"));

    // create an offscreen render target, we explicitely want repeat texture wrap mode
    // and linear blending...
    auto rtSetup = TextureSetup::AsRenderTarget("rt", 128, 128, PixelFormat::R8G8B8, PixelFormat::D16);
    rtSetup.SetWrapU(TextureWrapMode::Repeat);
    rtSetup.SetWrapV(TextureWrapMode::Repeat);
    rtSetup.SetMagFilter(TextureFilterMode::Linear);
    rtSetup.SetMinFilter(TextureFilterMode::Linear);
    this->renderTarget = this->render->CreateResource(rtSetup);
    
    // create a donut (this will be rendered into the offscreen render target)
    ShapeBuilder shapeBuilder;
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.AddTorus(0.3f, 0.5f, 20, 36);
    shapeBuilder.Build();
    this->torus = this->render->CreateResource(MeshSetup::FromData("sphere"), shapeBuilder.GetStream());
    
    // create a sphere mesh with normals and uv coords
    shapeBuilder.Clear();
    shapeBuilder.AddComponent(VertexAttr::Position, VertexFormat::Float3);
    shapeBuilder.AddComponent(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.AddComponent(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.AddSphere(0.5f, 72.0f, 40.0f);
    shapeBuilder.Build();
    this->sphere = this->render->CreateResource(MeshSetup::FromData("torus"), shapeBuilder.GetStream());

    // build shader for rendering to render-target
    Id rtVs = this->render->CreateResource(ShaderSetup::FromSource("rtVs", ShaderType::VertexShader, rtVsSource));
    Id rtFs = this->render->CreateResource(ShaderSetup::FromSource("rtFs", ShaderType::FragmentShader, rtFsSource));
    ProgramBundleSetup rtProgSetup("rtProg");
    rtProgSetup.AddProgram(0, rtVs, rtFs);
    rtProgSetup.AddUniform("mvp", ModelViewProjection);
    this->rtProg = this->render->CreateResource(rtProgSetup);
    
    // build shader for rendering to display
    Id dispVs = this->render->CreateResource(ShaderSetup::FromSource("dispVs", ShaderType::VertexShader, dispVsSource));
    Id dispFs = this->render->CreateResource(ShaderSetup::FromSource("dispFs", ShaderType::FragmentShader, dispFsSource));
    ProgramBundleSetup dispProgSetup("dispProg");
    dispProgSetup.AddProgram(0, dispVs, dispFs);
    dispProgSetup.AddUniform("mvp", ModelViewProjection);
    dispProgSetup.AddTextureUniform("tex", Texture);
    this->dispProg = this->render->CreateResource(dispProgSetup);
    
    // can release vertex- and fragment shader handles now
    this->render->DiscardResource(rtVs);
    this->render->DiscardResource(rtFs);
    this->render->DiscardResource(dispVs);
    this->render->DiscardResource(dispFs);
    
    // setup static transform matrices
    this->offscreenProj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 20.0f);
    this->displayProj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return AppState::Running;
}

//------------------------------------------------------------------------------
glm::mat4
SimpleRenderTargetApp::computeMVP(const glm::mat4& proj, float32 rotX, float32 rotY, const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    return proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
        
        // update angles
        this->angleY += 0.01f;
        this->angleX += 0.02f;
        
        // general render states
        this->render->ApplyState(Render::State::DepthMask, true);
        this->render->ApplyState(Render::State::DepthTestEnabled, true);
        this->render->ApplyState(Render::State::DepthFunc, Render::State::LessEqual);
        this->render->ApplyState(Render::State::ClearDepth, 1.0f);
        
        // render donut to offscreen render target
        this->render->ApplyRenderTarget(this->renderTarget);
        this->render->ApplyState(Render::State::ClearColor, 0.25f, 0.25f, 0.25f, 0.0f);
        this->render->Clear(true, true, true);
        this->render->ApplyMesh(this->torus);
        this->render->ApplyProgram(this->rtProg, 0);
        glm::mat4 donutMVP = this->computeMVP(this->offscreenProj, this->angleX, this->angleY, glm::vec3(0.0f, 0.0f, -3.0f));
        this->render->ApplyVariable(ModelViewProjection, donutMVP);
        this->render->Draw(0);
        
        // render sphere to display, with offscreen render target as texture
        this->render->ApplyRenderTarget(Resource::Id());
        this->render->ApplyState(Render::State::ClearColor, 0.25f, 0.25f, 0.25f, 0.0f);
        this->render->Clear(true, true, true);
        this->render->ApplyMesh(this->sphere);
        this->render->ApplyProgram(this->dispProg, 0);
        glm::mat4 sphereMVP = this->computeMVP(this->displayProj, -this->angleX * 0.25f, this->angleY * 0.25f, glm::vec3(0.0f, 0.0f, -1.5f));
        this->render->ApplyVariable(ModelViewProjection, sphereMVP);
        this->render->ApplyVariable(Texture, this->renderTarget);
        this->render->Draw(0);
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SimpleRenderTargetApp::OnCleanup() {
    // cleanup everything
    this->render->DiscardResource(this->rtProg);
    this->render->DiscardResource(this->dispProg);
    this->render->DiscardResource(this->sphere);
    this->render->DiscardResource(this->torus);
    this->render->DiscardResource(this->renderTarget);
    this->render->Discard();
    this->render = nullptr;
    RenderFacade::DestroySingleton();
    return AppState::Destroy;
}
