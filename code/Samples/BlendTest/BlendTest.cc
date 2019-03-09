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

    Id bgPipeline;
    Bindings bgBindings;
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
        .SetWidth(1024)
        .SetHeight(768)
        .SetTitle("Oryol Blend Sample")
        .SetHtmlTrackElementSize(true)
        .SetResourcePoolSize(GfxResourceType::Pipeline, 512));

    // create pipeline object for a patterned background
    const float bgVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    this->bgBindings.VertexBuffers[0] = Gfx::CreateBuffer(BufferDesc()
        .SetSize(sizeof(bgVertices))
        .SetContent(bgVertices));
    this->bgPipeline = Gfx::CreatePipeline(PipelineDesc()
        .SetShader(Gfx::CreateShader(BGShader::Desc()))
        .SetLayout(0, {
            { "in_pos", VertexFormat::Float2 }
        })
        .SetPrimitiveType(PrimitiveType::TriangleStrip));

    // setup a triangle mesh and shader
    float triVertices[] = {
          // pos                color
          0.0f, 0.05f, 0.5f,    0.7f, 0.0f, 0.0f, 0.75f,
          0.05f, -0.05f, 0.5f,  0.0f, 0.75f, 0.0f, 0.75f,
          -0.05f, -0.05f, 0.5f, 0.0f, 0.0f, 0.75f, 0.75f
    };
    this->triVBuf = Gfx::CreateBuffer(BufferDesc()
        .SetSize(sizeof(triVertices))
        .SetContent(triVertices));

    // setup one draw state for each blend factor combination
    auto ps = PipelineDesc()
        .SetShader(Gfx::CreateShader(TriShader::Desc()))
        .SetLayout(0, {
            { "in_pos", VertexFormat::Float3 },
            { "in_color", VertexFormat::Float4 }
        })
        .SetBlendEnabled(true)
        .SetBlendColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        .SetColorWriteMask(PixelChannel::RGB);
    for (uint32_t y = 0; y < BlendFactor::Num; y++) {
        for (uint32_t x = 0; x < BlendFactor::Num; x++) {
            ps.SetBlendSrcFactorRGB((BlendFactor::Code)x);
            ps.SetBlendDstFactorRGB((BlendFactor::Code)y);
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
    Gfx::ApplyPipeline(this->bgPipeline);
    Gfx::ApplyBindings(this->bgBindings);
    Gfx::Draw(0, 4);

    // draw blended triangles
    Bindings triBind;
    triBind.VertexBuffers[0] = this->triVBuf;
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
                Gfx::ApplyPipeline(this->pipelines[y][x]);
                Gfx::ApplyBindings(triBind);
                Gfx::ApplyUniforms(this->params);
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
