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
    this->setupWhiteTexture();
    this->setupMesh();
    this->setupDrawState();
    Gfx::Resource().PopLabel();
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
tbOryolRenderer::Discard() {
    o_assert_dbg(this->isValid);
    this->deleteTextures();
    this->isValid = false;
    Gfx::Resource().Destroy(this->resLabel);
}

//------------------------------------------------------------------------------
void
tbOryolRenderer::setupWhiteTexture() {
    o_assert_dbg(!this->whiteTexture.IsValid());
    
    const int w = 4;
    const int h = 4;
    uint32 pixels[w * h];
    Memory::Fill(pixels, sizeof(pixels), 0xFF);
    auto stream = MemoryStream::Create();
    stream->Open(OpenMode::WriteOnly);
    stream->Write(pixels, sizeof(pixels));
    stream->Close();
    
    auto texSetup = TextureSetup::FromPixelData(w, h, 1, TextureType::Texture2D, PixelFormat::RGBA8);
    texSetup.WrapU = TextureWrapMode::Repeat;
    texSetup.WrapV = TextureWrapMode::Repeat;
    texSetup.MinFilter = TextureFilterMode::Nearest;
    texSetup.MagFilter = TextureFilterMode::Nearest;
    texSetup.ImageSizes[0][0] = sizeof(pixels);
    this->whiteTexture = Gfx::Resource().Create(texSetup, stream);
}

//------------------------------------------------------------------------------
void
tbOryolRenderer::setupMesh() {
    o_assert_dbg(!this->mesh.IsValid());
    
    this->vertexLayout
        .Add(VertexAttr::Position, VertexFormat::Float2)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2)
        .Add(VertexAttr::Color0, VertexFormat::UByte4N);
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
tbOryolRenderer::deferDeleteTexture(ResourceLabel label) {
    this->texturesForDeletion.Add(label);
}

//------------------------------------------------------------------------------
void
tbOryolRenderer::deleteTextures() {
    if (!this->texturesForDeletion.Empty()) {
        for (ResourceLabel label : this->texturesForDeletion) {
            Gfx::Resource().Destroy(label);
        }
        this->texturesForDeletion.Clear();
    }
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
        
        this->tbClipRect = this->m_screen_rect;
        Gfx::ApplyScissorRect(this->m_screen_rect.x, this->m_screen_rect.y, this->m_screen_rect.w, this->m_screen_rect.h);
        Gfx::UpdateVertices(this->mesh, vertexDataSize, this->vertexData);
        Gfx::ApplyDrawState(this->drawState);
        Gfx::ApplyVariable(Shaders::TBUIShader::Ortho, ortho);
        for (int batchIndex = 0; batchIndex < this->curBatchIndex; batchIndex++) {
            const tbOryolBatch& batch = this->batches[batchIndex];
            Gfx::ApplyScissorRect(batch.clipRect.x, batch.clipRect.y, batch.clipRect.w, batch.clipRect.h);
            if (batch.texture.IsValid()) {
                Gfx::ApplyVariable(Shaders::TBUIShader::Texture, batch.texture);
            }
            else {
                Gfx::ApplyVariable(Shaders::TBUIShader::Texture, this->whiteTexture);
            }
            Gfx::Draw(PrimitiveGroup(PrimitiveType::Triangles, batch.startIndex, batch.numVertices));
        }
    }
    
    // defer-delete this frame's textures
    this->deleteTextures();

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
    if (tbBatch->bitmap) {
        batch.texture = ((tbOryolBitmap*)tbBatch->bitmap)->texture;
    }
    else {
        batch.texture.Invalidate();
    }
    batch.clipRect = this->tbClipRect;

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
    this->tbClipRect.x = this->m_clip_rect.x;
    this->tbClipRect.y = this->m_screen_rect.h - (this->m_clip_rect.y + this->m_clip_rect.h);
    this->tbClipRect.w = this->m_clip_rect.w;
    this->tbClipRect.h = this->m_clip_rect.h;
}

} // namespace _priv
} // namespace Oryol
