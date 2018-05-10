//------------------------------------------------------------------------------
//  BlendTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
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
    Gfx::Setup(GfxDesc()
        .Width(1024)
        .Height(768)
        .Title("Oryol Blend Sample")
        .HtmlTrackElementSize(true)
        .ResourcePoolSize(GfxResourceType::Pipeline, 512));

    // create pipeline object for a patterned background
    const float bgVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    this->bgDrawState.VertexBuffers[0] = Gfx::CreateBuffer(BufferDesc()
        .Size(sizeof(bgVertices))
        .Content(bgVertices));
    this->bgDrawState.Pipeline = Gfx::CreatePipeline(PipelineDesc()
        .Shader(Gfx::CreateShader(BGShader::Desc()))
        .Layout(0, {
            { "in_pos", VertexFormat::Float2 }
        })
        .PrimitiveType(PrimitiveType::TriangleStrip));

    // setup a triangle mesh and shader
    float triVertices[] = {
          // pos                color
          0.0f, 0.05f, 0.5f,    0.7f, 0.0f, 0.0f, 0.75f,
          0.05f, -0.05f, 0.5f,  0.0f, 0.75f, 0.0f, 0.75f,
          -0.05f, -0.05f, 0.5f, 0.0f, 0.0f, 0.75f, 0.75f
    };
    this->triVBuf = Gfx::CreateBuffer(BufferDesc()
        .Size(sizeof(triVertices))
        .Content(triVertices));

    // setup one draw state for each blend factor combination
    auto ps = PipelineDesc()
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
            this->pipelines[y][x] = Gfx::CreatePipeline(ps);
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
            bool valid = true;
            /* WebGL exceptions: 
                - "GL_SRC_ALPHA_SATURATE as a destination blend function is disallowed in WebGL 1"
                - "constant color and constant alpha cannot be used together as source and 
                    destination factors in the blend function"
            */
            const BlendFactor::Code src = (BlendFactor::Code) x;
            const BlendFactor::Code dst = (BlendFactor::Code) y;
            if (dst == BlendFactor::SrcAlphaSaturated) {
                valid = false;
            }
            else if (((src == BlendFactor::BlendColor) || (src == BlendFactor::OneMinusBlendColor)) &&
                     ((dst == BlendFactor::BlendAlpha) || (dst == BlendFactor::OneMinusBlendAlpha)))
            {
                valid = false;
            }
            else if (((src == BlendFactor::BlendAlpha) || (src == BlendFactor::OneMinusBlendAlpha)) &&
                     ((dst == BlendFactor::BlendColor) || (dst == BlendFactor::OneMinusBlendColor)))
            {
                valid = false;
            }
            if (valid) {
                this->params.translate.x = ((d * x) + d*0.5f) * 2.0f - 1.0f;
                this->params.translate.y = ((d * y) + d*0.5f) * 2.0f - 1.0f;
                triDrawState.Pipeline = this->pipelines[y][x];
                Gfx::ApplyDrawState(triDrawState);
                Gfx::ApplyUniformBlock(this->params);
                Gfx::Draw(0, 3);
            }
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
