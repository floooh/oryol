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
    auto gfxSetup = GfxSetup::Window(1024, 768, "Oryol Blend Sample");
    gfxSetup.ResourcePoolSize[GfxResourceType::Pipeline] =  512;
    Gfx::Setup(gfxSetup);

    // create pipeline object for a patterned background
    float bgVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    auto bgBuf = BufferSetup::Make(sizeof(bgVertices));
    this->bgDrawState.VertexBuffers[0] = Gfx::CreateResource(bgBuf, bgVertices, sizeof(bgVertices));
    Id bgShd = Gfx::CreateResource(BGShader::Setup());
    auto ps = PipelineSetup::FromShaderAndLayout(bgShd, {
        { "in_pos", VertexFormat::Float2 }
    });
    ps.PrimType = PrimitiveType::TriangleStrip;
    this->bgDrawState.Pipeline = Gfx::CreateResource(ps);

    // setup a triangle mesh and shader
    float triVertices[] = {
          // pos                color
          0.0f, 0.05f, 0.5f,    0.7f, 0.0f, 0.0f, 0.75f,
          0.05f, -0.05f, 0.5f,  0.0f, 0.75f, 0.0f, 0.75f,
          -0.05f, -0.05f, 0.5f, 0.0f, 0.0f, 0.75f, 0.75f
    };
    this->triVBuf = Gfx::CreateResource(BufferSetup::Make(sizeof(triVertices)), triVertices, sizeof(triVertices));

    // setup one draw state for each blend factor combination
    Id shd = Gfx::CreateResource(TriShader::Setup());
    ps = PipelineSetup::FromShaderAndLayout(shd, {
        { "in_pos", VertexFormat::Float3 },
        { "in_color", VertexFormat::Float4 }
    });
    ps.BlendState.BlendEnabled = true;
    ps.BlendColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    ps.BlendState.ColorWriteMask = PixelChannel::RGB;
    for (uint32_t y = 0; y < BlendFactor::Num; y++) {
        for (uint32_t x = 0; x < BlendFactor::Num; x++) {
            ps.BlendState.SrcFactorRGB = (BlendFactor::Code) x;
            ps.BlendState.DstFactorRGB = (BlendFactor::Code) y;
            this->pipelines[y][x] = Gfx::CreateResource(ps);
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
