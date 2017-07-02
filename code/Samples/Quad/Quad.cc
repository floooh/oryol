//------------------------------------------------------------------------------
//  Quad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "shaders.h"

using namespace Oryol;

class QuadApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    DrawState drawState;
};
OryolMain(QuadApp);

//------------------------------------------------------------------------------
AppState::Code
QuadApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(400, 400, "Oryol Quad Sample"));
    
    // quad mesh with vertices followed by index data
    static struct data_t {
        const float vertices[4 * 7] = {
            // positions            colors
            -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
             0.5f,  0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 0.0f, 1.0f,
        };
        const uint16_t indices[2 * 3] = {
            0, 1, 2,    // first triangle
            0, 2, 3,    // second triangle
        };
    } data;

    auto meshSetup = MeshSetup::FromData();
    meshSetup.NumVertices = 4;
    meshSetup.NumIndices = 6;
    meshSetup.IndicesType = IndexType::Index16;
    meshSetup.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Color0, VertexFormat::Float4 }
    };
    meshSetup.AddPrimitiveGroup({0, 6});
    meshSetup.VertexDataOffset = 0;
    meshSetup.IndexDataOffset = offsetof(data_t, indices);
    this->drawState.Mesh[0] = Gfx::CreateResource(meshSetup, &data, sizeof(data));

    // create shader and pipeline-state-object
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(meshSetup.Layout, shd);
    this->drawState.Pipeline = Gfx::CreateResource(ps);

    return App::OnInit();
}


//------------------------------------------------------------------------------
AppState::Code
QuadApp::OnRunning() {
    
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::Draw();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
QuadApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
