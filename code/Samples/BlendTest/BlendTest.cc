//------------------------------------------------------------------------------
//  BlendTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/MeshBuilder.h"
#include "shaders.h"

using namespace Oryol;

class BlendTestApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    DrawState bgDrawState;
    Id triVBuf;
    Id pipelines[BlendFactor::Num][BlendFactor::Num];
    TriShader::params params;
};
OryolMain(BlendTestApp);

//------------------------------------------------------------------------------
AppState::Code
BlendTestApp::OnInit() {
    // setup rendering system
    auto gfxDesc = GfxDesc::Window(1024, 768, "Oryol Blend Sample");
    gfxDesc.ResourcePoolSize[GfxResourceType::Pipeline] =  512;
    Gfx::Setup(gfxDesc);

    // create pipeline object for a patterned background
    const float bgVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    this->bgDrawState.VertexBuffers[0] = Gfx::Buffer()
        .Size(sizeof(bgVertices))
        .Content(bgVertices)
        .Create();
    this->bgDrawState.Pipeline = Gfx::Pipeline()
        .Shader(Gfx::CreateShader(BGShader::Desc()))
        .Layout(0, {
            { "in_pos", VertexFormat::Float2 }
        })
        .PrimitiveType(PrimitiveType::TriangleStrip)
        .Create();

    // setup a triangle mesh and shader
    float triVertices[] = {
          // pos                color
          0.0f, 0.05f, 0.5f,    0.7f, 0.0f, 0.0f, 0.75f,
          0.05f, -0.05f, 0.5f,  0.0f, 0.75f, 0.0f, 0.75f,
          -0.05f, -0.05f, 0.5f, 0.0f, 0.0f, 0.75f, 0.75f
    };
    this->triVBuf = Gfx::Buffer()
        .Size(sizeof(triVertices))
        .Content(triVertices)
        .Create();

    // setup one draw state for each blend factor combination
    auto ps = Gfx::Pipeline()
        .Shader(Gfx::CreateShader(TriShader::Desc()))
        .Layout(0, {
            { "in_pos", VertexFormat::Float3 },
            { "in_color", VertexFormat::Float4 }
        })
        .BlendEnabled(true)
        .BlendColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        .ColorWriteMask(PixelChannel::RGB);
    for (uint32_t y = 0; y < BlendFactor::Num; y++) {
        for (uint32_t x = 0; x < BlendFactor::Num; x++) {
            ps.BlendSrcFactorRGB((BlendFactor::Code)x);
            ps.BlendDstFactorRGB((BlendFactor::Code)y);
            this->pipelines[y][x] = ps.Create();
        }
    }
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
BlendTestApp::OnRunning() {
    
    // draw checkboard background
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->bgDrawState);
    Gfx::Draw(0, 4);

    // draw blended triangles
    DrawState triDrawState;
    triDrawState.VertexBuffers[0] = this->triVBuf;
    float d = 1.0f / BlendFactor::Num;
    for (uint32_t y = 0; y < BlendFactor::Num; y++) {
        for (uint32_t x = 0; x < BlendFactor::Num; x++) {
            this->params.translate.x = ((d * x) + d*0.5f) * 2.0f - 1.0f;
            this->params.translate.y = ((d * y) + d*0.5f) * 2.0f - 1.0f;
            triDrawState.Pipeline = this->pipelines[y][x];
            Gfx::ApplyDrawState(triDrawState);
            Gfx::ApplyUniformBlock(this->params);
            Gfx::Draw(0, 3);
        }
    }
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
BlendTestApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
