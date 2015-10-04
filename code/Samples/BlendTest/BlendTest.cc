//------------------------------------------------------------------------------
//  BlendTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
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
    Id backgroundDrawState;
    Id drawStates[BlendFactor::NumBlendFactors][BlendFactor::NumBlendFactors];
    Shaders::Triangle::Params params;
};
OryolMain(BlendTestApp);

//------------------------------------------------------------------------------
AppState::Code
BlendTestApp::OnRunning() {
    
    // draw checkboard background
    Gfx::ApplyDefaultRenderTarget();
    Gfx::ApplyDrawState(this->backgroundDrawState);
    Gfx::Draw(0);

    // draw blended triangles
    float d = 1.0f / BlendFactor::NumBlendFactors;
    for (uint32 y = 0; y < BlendFactor::NumBlendFactors; y++) {
        for (uint32 x = 0; x < BlendFactor::NumBlendFactors; x++) {
            this->params.Translate.x = ((d * x) + d*0.5f) * 2.0f - 1.0f;
            this->params.Translate.y = ((d * y) + d*0.5f) * 2.0f - 1.0f;
            Gfx::ApplyDrawState(this->drawStates[y][x]);
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
    gfxSetup.SetPoolSize(GfxResourceType::DrawState, 512);
    Gfx::Setup(gfxSetup);

    // create drawstate for a patterned background
    Id cbMesh = Gfx::CreateResource(MeshSetup::FullScreenQuad());
    Id cbShd = Gfx::CreateResource(Shaders::Background::Setup());
    this->backgroundDrawState = Gfx::CreateResource(DrawStateSetup::FromMeshAndShader(cbMesh, cbShd));

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
        .Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 0.75f, 0.75f)
        .End();
    Id mesh = Gfx::CreateResource(meshBuilder.Result());
    Id shd = Gfx::CreateResource(Shaders::Triangle::Setup());
    
    // setup one draw state for each blend factor combination
    const glm::vec4 blendColor(1.0f, 1.0f, 0.0f, 1.0f);
    for (uint32 y = 0; y < BlendFactor::NumBlendFactors; y++) {
        for (uint32 x = 0; x < BlendFactor::NumBlendFactors; x++) {
            auto dss = DrawStateSetup::FromMeshAndShader(mesh, shd);
            dss.BlendState.BlendEnabled = true;
            dss.BlendState.SrcFactorRGB = (BlendFactor::Code) x;
            dss.BlendState.DstFactorRGB = (BlendFactor::Code) y;
            dss.BlendColor = blendColor;
            this->drawStates[y][x] = Gfx::CreateResource(dss);
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
