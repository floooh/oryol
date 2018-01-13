//------------------------------------------------------------------------------
//  SeperateBuffers.cc
//
//  Render 3 cubes from the same vertex position data, but 3 separate
//  color data buffers, demonstrating how to combine vertex data
//  from different buffers. This can be useful to prevent duplicating
//  data (like in this sample), or (more common) to save vertex fetching bandwidth.
//  For instance in multi-pass rendering, some rendering passes might
//  not need all vertex data (i.e. vertex normals are not
//  needed when rendering shadow maps).
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

class SeparateBuffersApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    glm::mat4 computeMVP(const glm::vec3& pos);
    static const int NumColorBuffer = 3;
    ShapeBuilder::Result cubeShape;
    StaticArray<Id, NumColorBuffer> colorBuffers;
    DrawState drawState;
    Shader::params params;
    glm::mat4 view;
    glm::mat4 proj;
    float angleX = 0.0f;
    float angleY = 0.0f;
};
OryolMain(SeparateBuffersApp);

//------------------------------------------------------------------------------
AppState::Code
SeparateBuffersApp::OnInit() {

    auto gfxSetup = GfxSetup::WindowMSAA4(600, 400, "Separate Buffers");
    Gfx::Setup(gfxSetup);

    // create a cube mesh with positions only, this will be placed
    // into the first vertex buffer bind slot
    ShapeBuilder shapeBuilder;
    this->cubeShape = shapeBuilder
        .AddPositions("in_pos", VertexFormat::Float3)
        .Box(1.0f, 1.0f, 1.0f, 1)
        .Build();
    this->drawState.VertexBuffers[0] = Gfx::CreateResource(this->cubeShape.VertexBufferSetup, this->cubeShape.Data);
    this->drawState.IndexBuffer = Gfx::CreateResource(this->cubeShape.IndexBufferSetup, this->cubeShape.Data);

    // create 3 meshes with only color data
    static const int NumVertices = 24;
    static const int NumColorChannels = 3;
    float colorVertices[NumVertices][NumColorChannels]= { };
    auto colorSetup = BufferSetup::Make(sizeof(colorVertices));
    for (int i = 0; i < NumColorChannels; i++) {
        for (int vi = 0; vi < NumVertices; vi++) {
            colorVertices[vi][i] = glm::linearRand(0.5f, 1.0f);
        }
        this->colorBuffers[i] = Gfx::CreateResource(colorSetup, colorVertices, sizeof(colorVertices));
    }

    // create shader and pipeline, the position data vertex Layout
    // goes into the first slot, and the color data vertex layout into the second slot
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto ps = PipelineSetup::FromShader(shd);
    ps.Layouts[0] = this->cubeShape.Layout;
    ps.Layouts[1] = { { "in_color", VertexFormat::Float3 } };
    ps.IndexType = this->cubeShape.IndexType;
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    ps.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->drawState.Pipeline = Gfx::CreateResource(ps);

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
SeparateBuffersApp::OnRunning() {
    
    this->angleY += 0.01f;
    this->angleX += 0.02f;
    
    Gfx::BeginPass();
    static const glm::vec3 positions[] = {
        glm::vec3(-2.0, 0.0f, -6.0f),
        glm::vec3(0.0f, 0.0f, -6.0f),
        glm::vec3(+2.0f, 0.0f, -6.0f),
    };
    for (int i = 0; i < 3; i++) {
        // switch to the next color data buffer, but keep the
        // same position data buffer
        this->drawState.VertexBuffers[1] = this->colorBuffers[i];
        Gfx::ApplyDrawState(this->drawState);
        this->params.mvp = this->computeMVP(positions[i]);
        Gfx::ApplyUniformBlock(this->params);
        Gfx::Draw(this->cubeShape.PrimitiveGroups[0]);
    }
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
SeparateBuffersApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
SeparateBuffersApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

