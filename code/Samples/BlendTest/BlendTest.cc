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
    Id triMesh;
    Id pipelines[BlendFactor::NumBlendFactors][BlendFactor::NumBlendFactors];
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
    auto ms = MeshSetup::FullScreenQuad();
    this->bgDrawState.Mesh[0] = Gfx::CreateResource(ms);
    Id bgShd = Gfx::CreateResource(BGShader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(ms.Layout, bgShd);
    this->bgDrawState.Pipeline = Gfx::CreateResource(ps);

    // setup a triangle mesh and shader
    MeshBuilder meshBuilder;
    meshBuilder.NumVertices = 3;
    meshBuilder.IndicesType = IndexType::None;
    meshBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Color0, VertexFormat::Float4 }
    };
    meshBuilder.PrimitiveGroups.Add(0, 3);
    meshBuilder.Begin()
        .Vertex(0, VertexAttr::Position, 0.0f, 0.05f, 0.5f)
        .Vertex(0, VertexAttr::Color0, 0.75f, 0.0f, 0.0f, 0.75f)
        .Vertex(1, VertexAttr::Position, 0.05f, -0.05f, 0.5f)
        .Vertex(1, VertexAttr::Color0, 0.0f, 0.75f, 0.0f, 0.75f)
        .Vertex(2, VertexAttr::Position, -0.05f, -0.05f, 0.5f)
        .Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 0.75f, 0.75f);
    this->triMesh = Gfx::CreateResource(meshBuilder.Build());
    Id shd = Gfx::CreateResource(TriShader::Setup());
    
    // setup one draw state for each blend factor combination
    ps = PipelineSetup::FromLayoutAndShader(meshBuilder.Layout, shd);
    ps.BlendState.BlendEnabled = true;
    ps.BlendColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    ps.BlendState.ColorWriteMask = PixelChannel::RGB;
    for (uint32_t y = 0; y < BlendFactor::NumBlendFactors; y++) {
        for (uint32_t x = 0; x < BlendFactor::NumBlendFactors; x++) {
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
    Gfx::Draw();

    // draw blended triangles
    DrawState triDrawState;
    triDrawState.Mesh[0] = this->triMesh;
    float d = 1.0f / BlendFactor::NumBlendFactors;
    for (uint32_t y = 0; y < BlendFactor::NumBlendFactors; y++) {
        for (uint32_t x = 0; x < BlendFactor::NumBlendFactors; x++) {
            this->params.translate.x = ((d * x) + d*0.5f) * 2.0f - 1.0f;
            this->params.translate.y = ((d * y) + d*0.5f) * 2.0f - 1.0f;
            triDrawState.Pipeline = this->pipelines[y][x];
            Gfx::ApplyDrawState(triDrawState);
            Gfx::ApplyUniformBlock(this->params);
            Gfx::Draw();
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
