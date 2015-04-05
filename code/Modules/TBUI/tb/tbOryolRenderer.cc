//------------------------------------------------------------------------------
//  tbOryolRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Memory/Memory.h"
#include "tbOryolRenderer.h"
#include "TBUI/tb/tbOryolBitmap.h"
#include "tb_bitmap_fragment.h"
#include "tb_system.h"
#include "TBUI/tb/TBUIShaders.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
tbOryolRenderer::tbOryolRenderer() :
isValid(false),
curBatchIndex(0),
curVertexIndex(0) {
    // empty
}

//------------------------------------------------------------------------------
tbOryolRenderer::~tbOryolRenderer() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
tbOryolRenderer::Setup() {
    o_assert_dbg(!this->isValid);
    this->resLabel = Gfx::Resource().PushLabel();
    this->setupMesh();
    this->setupDrawState();
    Gfx::Resource().PopLabel();
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
tbOryolRenderer::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    Gfx::Resource().Destroy(this->resLabel);
}

//------------------------------------------------------------------------------
void
tbOryolRenderer::setupMesh() {
    o_assert_dbg(!this->mesh.IsValid());
    
    this->vertexLayout
        .Add(VertexAttr::Position, VertexFormat::Float2)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2)
        .Add(VertexAttr::Color0, VertexFormat::UByte4);
    o_assert(sizeof(this->vertexData) == MaxNumVertices * this->vertexLayout.ByteSize());
    
    MeshSetup setup = MeshSetup::Empty(MaxNumVertices, Usage::Stream);
    setup.Layout = this->vertexLayout;
    this->mesh = Gfx::Resource().Create(setup);
    o_assert(this->mesh.IsValid());
    o_assert(Gfx::Resource().QueryResourceInfo(this->mesh).State == ResourceState::Valid);
}

//------------------------------------------------------------------------------
void
tbOryolRenderer::setupDrawState() {
    o_assert_dbg(this->mesh.IsValid());
    o_assert_dbg(!this->drawState.IsValid());
    
    Id prog = Gfx::Resource().Create(Shaders::TBUIShader::CreateSetup());
    
    auto dss = DrawStateSetup::FromMeshAndProg(this->mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    dss.BlendState.BlendEnabled = true;
    dss.BlendState.SrcFactorRGB = BlendFactor::SrcAlpha;
    dss.BlendState.DstFactorRGB = BlendFactor::OneMinusSrcAlpha;
    dss.RasterizerState.ScissorTestEnabled = true;
    this->drawState = Gfx::Resource().Create(dss);
}

//------------------------------------------------------------------------------
void
tbOryolRenderer::BeginPaint(int render_target_w, int render_target_h)
{
    o_assert_dbg(this->isValid);

    TBRendererBatcher::BeginPaint(render_target_w, render_target_h);
    this->curVertexIndex = 0;
    this->curBatchIndex = 0;
}

//------------------------------------------------------------------------------
void tbOryolRenderer::EndPaint()
{
    o_assert_dbg(this->isValid);

    if (this->curVertexIndex > 0) {
    
        const glm::mat4 ortho = glm::ortho(0.0f, float(this->m_screen_rect.w),
            (float)this->m_screen_rect.h, 0.0f,
            -1.0f, 1.0f);
        const int vertexDataSize = this->curVertexIndex * this->vertexLayout.ByteSize();
        
        Gfx::ApplyScissorRect(this->m_screen_rect.x, this->m_screen_rect.y, this->m_screen_rect.w, this->m_screen_rect.h);
        Gfx::UpdateVertices(this->mesh, vertexDataSize, this->vertexData);
        Gfx::ApplyDrawState(this->drawState);
        Gfx::ApplyVariable(Shaders::TBUIShader::Ortho, ortho);
        for (int batchIndex = 0; batchIndex < this->curBatchIndex; batchIndex++) {
            const tbOryolBatch& batch = this->batches[batchIndex];
            Gfx::ApplyVariable(Shaders::TBUIShader::Texture, batch.texture);
            Gfx::Draw(PrimitiveGroup(PrimitiveType::Triangles, batch.startIndex, batch.numVertices));
        }
    }

    TBRendererBatcher::EndPaint();
}

//------------------------------------------------------------------------------
void tbOryolRenderer::RenderBatch(Batch *tbBatch)
{
    o_assert_dbg(this->isValid);
    o_assert((this->curVertexIndex + tbBatch->vertex_count) < MaxNumVertices);
    o_assert(this->curBatchIndex < MaxNumBatches);

    const int size = this->vertexLayout.ByteSize() * tbBatch->vertex_count;
    Memory::Copy(&tbBatch->vertex[0], &this->vertexData[this->curVertexIndex], size);

    tbOryolBatch& batch = this->batches[this->curBatchIndex++];
    batch.startIndex = this->curVertexIndex;
    batch.numVertices = tbBatch->vertex_count;
    batch.texture = ((tbOryolBitmap*)tbBatch->bitmap)->GetTexture();

    this->curVertexIndex += tbBatch->vertex_count;
}

//------------------------------------------------------------------------------
tb::TBBitmap *
tbOryolRenderer::CreateBitmap(int width, int height, uint32 *data)
{
    Log::Info("tbOryolRenderer::CreateBitmap() called\n");

    tbOryolBitmap *bitmap = new tbOryolBitmap(this);
    bitmap->Init(width, height, data);
    return bitmap;
}

//------------------------------------------------------------------------------
void tbOryolRenderer::SetClipRect(const tb::TBRect &rect)
{
    // NOTE: this is an internal method called by parent class!
    int32 x = this->m_clip_rect.x;
    int32 y = this->m_screen_rect.h - (this->m_clip_rect.y + this->m_clip_rect.h);
    int32 w = this->m_clip_rect.w;
    int32 h = this->m_clip_rect.h;
    Gfx::ApplyScissorRect(x, y, w, h);
}

} // namespace _priv
} // namespace Oryol
