//------------------------------------------------------------------------------
//  PackedNormals.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Gfx/Util/RawMeshLoader.h"
#include "Gfx/Util/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class PackedNormalsApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
private:
    glm::mat4 computeMVP(const glm::vec3& pos);

    Id msaaDrawState;
    Id noMsaaDrawState;
    glm::mat4 view;
    glm::mat4 proj;
    float32 angleX = 0.0f;
    float32 angleY = 0.0f;
};
OryolMain(PackedNormalsApp);

//------------------------------------------------------------------------------
AppState::Code
PackedNormalsApp::OnRunning() {
    // render one frame
    if (Gfx::BeginFrame()) {
        
        // update angles
        this->angleY += 0.01f;
        this->angleX += 0.02f;
        
        // apply state and render
        Gfx::ApplyDefaultRenderTarget();
        Gfx::ApplyDrawState(this->msaaDrawState);
        Gfx::Clear(PixelChannel::All, glm::vec4(0.0f), 1.0f, 0);
        
        // draw shape primitive groups
        static const int numShapes = 5;
        static const glm::vec3 pos[numShapes] = {
            glm::vec3(-1.0, 1.0f, -6.0f),
            glm::vec3(1.0f, 1.0f, -6.0f),
            glm::vec3(-2.0f, -1.0f, -6.0f),
            glm::vec3(+2.0f, -1.0f, -6.0f),
            glm::vec3(0.0f, -1.0f, -6.0f)
        };
        
        for (int i = 0; i < numShapes; i++) {
            Gfx::ApplyVariable(Shaders::PackedNormals::ModelViewProjection, this->computeMVP(pos[i]));
            Gfx::Draw(0);
        }
        
        Gfx::EndFrame();
    }
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PackedNormalsApp::OnInit() {
    // setup rendering system
    auto gfxSetup = GfxSetup::Window(600, 400, true, "Oryol Packed Normals Sample");
    gfxSetup.Loaders.Add(RawMeshLoader::Creator());
    Gfx::Setup(gfxSetup);

    // create resources
    // NOTE: we draw some shapes with MSAA, some without
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout()
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 4)
        .Sphere(0.75f, 36, 20)
        .Cylinder(0.5f, 1.5f, 36, 10)
        .Torus(0.3f, 0.5f, 20, 36)
        .Plane(1.5f, 1.5f, 10)
        .Build();
    Id mesh = Gfx::CreateResource(MeshSetup::FromStream(), shapeBuilder.Result());
    Id prog = Gfx::CreateResource(Shaders::PackedNormals::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    dss.RasterizerState.CullFaceEnabled = true;
    dss.RasterizerState.MultisampleEnabled = true;
    this->msaaDrawState = Gfx::CreateResource(dss);
    dss.RasterizerState.MultisampleEnabled = false;
    this->noMsaaDrawState = Gfx::CreateResource(dss);

    Gfx::ReleaseResource(mesh);
    Gfx::ReleaseResource(prog);
    
    // setup projection and view matrices
    float32 fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    float32 fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PackedNormalsApp::OnCleanup() {
    // cleanup everything
    Gfx::ReleaseResource(this->msaaDrawState);
    Gfx::ReleaseResource(this->noMsaaDrawState);
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
PackedNormalsApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}
