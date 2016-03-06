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
private:
    Id bgPipeline;
    Id pipelines[BlendFactor::NumBlendFactors][BlendFactor::NumBlendFactors];
    Shaders::Triangle::Params params;
    MeshBlock bgQuadMesh;
    MeshBlock triangleMesh;
};
OryolMain(BlendTestApp);

//------------------------------------------------------------------------------
AppState::Code
BlendTestApp::OnRunning() {
    
    // draw checkboard background
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->bgPipeline, this->bgQuadMesh);
    Gfx::Draw(0);

    // draw blended triangles
    float d = 1.0f / BlendFactor::NumBlendFactors;
    for (uint32 y = 0; y < BlendFactor::NumBlendFactors; y++) {
        for (uint32 x = 0; x < BlendFactor::NumBlendFactors; x++) {
            this->params.Translate.x = ((d * x) + d*0.5f) * 2.0f - 1.0f;
            this->params.Translate.y = ((d * y) + d*0.5f) * 2.0f - 1.0f;
            Gfx::ApplyDrawState(this->pipelines[y][x], this->triangleMesh);
            Gfx::ApplyUniformBlock(this->params);
            Gfx::Draw(0);
        }
    }
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
BlendTestApp::OnInit() {
    // setup rendering system
    auto gfxSetup = GfxSetup::Window(1024, 768, "Oryol Blend Sample");
    gfxSetup.SetPoolSize(GfxResourceType::Pipeline, 512);
    Gfx::Setup(gfxSetup);

    // create pipeline object for a patterned background
    auto ms = MeshSetup::FullScreenQuad();
    this->bgQuadMesh[0] = Gfx::CreateResource(ms);
    Id bgShd = Gfx::CreateResource(Shaders::Background::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(ms.Layout, bgShd);
    this->bgPipeline = Gfx::CreateResource(ps);

    // setup a triangle mesh and shader
    MeshBuilder meshBuilder;
    meshBuilder.NumVertices = 3;
    meshBuilder.IndicesType = IndexType::None;
    meshBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Color0, VertexFormat::Float4);
    meshBuilder.PrimitiveGroups.Add(0, 3);
    meshBuilder.Begin()
        .Vertex(0, VertexAttr::Position, 0.0f, 0.05f, 0.5f)
        .Vertex(0, VertexAttr::Color0, 0.75f, 0.0f, 0.0f, 0.75f)
        .Vertex(1, VertexAttr::Position, 0.05f, -0.05f, 0.5f)
        .Vertex(1, VertexAttr::Color0, 0.0f, 0.75f, 0.0f, 0.75f)
        .Vertex(2, VertexAttr::Position, -0.05f, -0.05f, 0.5f)
        .Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 0.75f, 0.75f);
    this->triangleMesh[0] = Gfx::CreateResource(meshBuilder.Build());
    Id shd = Gfx::CreateResource(Shaders::Triangle::Setup());
    
    // setup one draw state for each blend factor combination
    ps = PipelineSetup::FromLayoutAndShader(meshBuilder.Layout, shd);
    ps.BlendState.BlendEnabled = true;
    ps.BlendColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    ps.BlendState.ColorWriteMask = PixelChannel::RGB;
    for (uint32 y = 0; y < BlendFactor::NumBlendFactors; y++) {
        for (uint32 x = 0; x < BlendFactor::NumBlendFactors; x++) {
            ps.BlendState.SrcFactorRGB = (BlendFactor::Code) x;
            ps.BlendState.DstFactorRGB = (BlendFactor::Code) y;
            this->pipelines[y][x] = Gfx::CreateResource(ps);
        }
    }

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
BlendTestApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
