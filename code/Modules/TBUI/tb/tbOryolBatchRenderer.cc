//------------------------------------------------------------------------------
//  tbOryolBatchRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "tbOryolBatchRenderer.h"
#include "TBUI/tb/tbOryolBitmap.h"
#include "tb_bitmap_fragment.h"
#include "tb_system.h"
#include "TBUIShaders.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Oryol {
namespace _priv {

using namespace tb;

#define VER_COL(r, g, b, a) (((a)<<24) + ((b)<<16) + ((g)<<8) + r)
#define VER_COL_OPACITY(a) (0x00ffffff + (((uint32)a) << 24))

//------------------------------------------------------------------------------
tbOryolBatchRenderer::tbOryolBatchRenderer() :
isValid(false),
fOpacitity(1.0f),
ui8Opacity(255),
translationX(0),
translationY(0),
u0(0.0f), v0(0.0f), u1(0.0f), v1(0.0f),
batchId(0),
inFlushBatch(false),
curBatchIndex(0),
curVertexIndex(0) {
    // empty
}

//------------------------------------------------------------------------------
tbOryolBatchRenderer::~tbOryolBatchRenderer() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::Setup() {
    o_assert_dbg(!this->isValid);
    
    // create gfx resources
    this->resLabel = Gfx::PushResourceLabel();
    this->setupResources();
    this->setupWhiteTexture();
    Gfx::PopResourceLabel();
    
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::Discard() {
    o_assert_dbg(this->isValid);
    
    this->deleteTextures();
    Gfx::DestroyResources(this->resLabel);
    this->isValid = false;
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::setupWhiteTexture() {
    o_assert_dbg(!this->whiteTexture.IsValid());

    const int w = 4;
    const int h = 4;
    uint32 pixels[w * h];
    Memory::Fill(pixels, sizeof(pixels), 0xFF);
    
    auto texSetup = TextureSetup::FromPixelData(w, h, 1, TextureType::Texture2D, PixelFormat::RGBA8);
    texSetup.Sampler.WrapU = TextureWrapMode::Repeat;
    texSetup.Sampler.WrapV = TextureWrapMode::Repeat;
    texSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
    texSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
    texSetup.ImageData.Sizes[0][0] = sizeof(pixels);
    this->whiteTexture = Gfx::CreateResource(texSetup, pixels, sizeof(pixels));
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::setupResources() {
    o_assert_dbg(!this->drawState.Mesh[0].IsValid());
    o_assert_dbg(!this->drawState.Pipeline.IsValid());

    this->vertexLayout
        .Add(VertexAttr::Position, VertexFormat::Float2)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2)
        .Add(VertexAttr::Color0, VertexFormat::UByte4N);
    o_assert(sizeof(this->vertexData) == MaxNumVertices * this->vertexLayout.ByteSize());
    
    MeshSetup setup = MeshSetup::Empty(MaxNumVertices, Usage::Stream);
    setup.Layout = this->vertexLayout;
    this->drawState.Mesh[0] = Gfx::CreateResource(setup);
    o_assert(this->drawState.Mesh[0].IsValid());
    o_assert(Gfx::QueryResourceInfo(this->drawState.Mesh[0]).State == ResourceState::Valid);

    Id shd = Gfx::CreateResource(TBUIShader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(this->vertexLayout, shd);
    ps.DepthStencilState.DepthWriteEnabled = false;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    ps.BlendState.BlendEnabled = true;
    ps.BlendState.SrcFactorRGB = BlendFactor::SrcAlpha;
    ps.BlendState.DstFactorRGB = BlendFactor::OneMinusSrcAlpha;
    ps.BlendState.ColorFormat = Gfx::DisplayAttrs().ColorPixelFormat;
    ps.BlendState.DepthFormat = Gfx::DisplayAttrs().DepthPixelFormat;
    ps.BlendState.ColorWriteMask = PixelChannel::RGB;
    ps.RasterizerState.ScissorTestEnabled = true;
    ps.RasterizerState.SampleCount = Gfx::DisplayAttrs().SampleCount;
    this->drawState.Pipeline = Gfx::CreateResource(ps);
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::deferDeleteTexture(ResourceLabel label) {
    this->texturesForDeletion.Add(label);
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::deleteTextures() {
    if (!this->texturesForDeletion.Empty()) {
        for (ResourceLabel label : this->texturesForDeletion) {
            Log::Info("tbOryolBatchRenderer: texture deleted\n");
            Gfx::DestroyResources(label);
        }
        this->texturesForDeletion.Clear();
    }
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::BeginPaint(int renderTargetWidth, int renderTargetHeight)
{
    o_assert_dbg(this->isValid);

    this->curVertexIndex = 0;
    this->curBatchIndex = 0;
    this->screenRect.Set(0, 0, renderTargetWidth, renderTargetHeight);
    this->clipRect = this->screenRect;
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::EndPaint() {
    o_assert_dbg(this->isValid);
    this->flushBatch();
    this->drawBatches();
    this->deleteTextures();
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::Translate(int dx, int dy) {
    this->translationX += dx;
    this->translationY += dy;
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::SetOpacity(float opacity)
{
    this->fOpacitity = opacity;
    this->ui8Opacity = (uint8) (opacity * 255);
}

//------------------------------------------------------------------------------
float
tbOryolBatchRenderer::GetOpacity() {
    return this->fOpacitity;
}

//------------------------------------------------------------------------------
tb::TBRect
tbOryolBatchRenderer::SetClipRect(const TBRect &rect, bool addToCurrent) {

    this->flushBatch();

    TBRect oldClipRect = this->clipRect;
    this->clipRect = rect;
    this->clipRect.x += this->translationX;
    this->clipRect.y += this->translationY;
    if (addToCurrent) {
        this->clipRect = this->clipRect.Clip(oldClipRect);
    }
    oldClipRect.x -= this->translationX;
    oldClipRect.y -= this->translationY;
    return oldClipRect;
}

//------------------------------------------------------------------------------
tb::TBRect
tbOryolBatchRenderer::GetClipRect() {
    TBRect curClipRect = this->clipRect;
    curClipRect.x -= this->translationX;
    curClipRect.y -= this->translationY;
    return curClipRect;
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::DrawBitmap(const TBRect& dstRect, const TBRect& srcRect, TBBitmapFragment* bitmapFragment) {
    o_assert_dbg(bitmapFragment);

    if (TBBitmap *bitmap = bitmapFragment->GetBitmap(TB_VALIDATE_FIRST_TIME)) {
        this->addQuad(dstRect.Offset(this->translationX, this->translationY),
                      srcRect.Offset(bitmapFragment->m_rect.x, bitmapFragment->m_rect.y),
                      VER_COL_OPACITY(this->ui8Opacity), bitmap, bitmapFragment);
    }
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::DrawBitmap(const TBRect& dstRect, const TBRect& srcRect, TBBitmap* bitmap) {
    this->addQuad(dstRect.Offset(this->translationX, this->translationY), srcRect, VER_COL_OPACITY(this->ui8Opacity), bitmap, nullptr);
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::DrawBitmapColored(const TBRect& dstRect, const TBRect& srcRect, const TBColor& color, TBBitmapFragment* bitmapFragment) {
    if (TBBitmap* bitmap = bitmapFragment->GetBitmap(TB_VALIDATE_FIRST_TIME)) {
        uint32 a = (color.a * this->ui8Opacity) / 255;
        this->addQuad(dstRect.Offset(this->translationX, this->translationY),
                      srcRect.Offset(bitmapFragment->m_rect.x, bitmapFragment->m_rect.y),
                      VER_COL(color.r, color.g, color.b, a), bitmap, bitmapFragment);
    }
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::DrawBitmapColored(const TBRect& dstRect, const TBRect& srcRect, const TBColor& color, TBBitmap* bitmap) {
    uint32 a = (color.a * this->ui8Opacity) / 255;
    this->addQuad(dstRect.Offset(this->translationX, this->translationY),
                  srcRect, VER_COL(color.r, color.g, color.b, a), bitmap, nullptr);
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::DrawBitmapTile(const TBRect& dstRect, TBBitmap* bitmap) {
    this->addQuad(dstRect.Offset(this->translationX, this->translationY),
                    TBRect(0, 0, dstRect.w, dstRect.h),
                    VER_COL_OPACITY(this->ui8Opacity), bitmap, nullptr);
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::DrawRect(const TBRect& dstRect, const TBColor& color) {
    if (dstRect.IsEmpty()) {
        return;
    }
    // Top
    this->DrawRectFill(TBRect(dstRect.x, dstRect.y, dstRect.w, 1), color);
    // Bottom
    this->DrawRectFill(TBRect(dstRect.x, dstRect.y + dstRect.h - 1, dstRect.w, 1), color);
    // Left
    this->DrawRectFill(TBRect(dstRect.x, dstRect.y + 1, 1, dstRect.h - 2), color);
    // Right
    this->DrawRectFill(TBRect(dstRect.x + dstRect.w - 1, dstRect.y + 1, 1, dstRect.h - 2), color);
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::DrawRectFill(const TBRect& dstRect, const TBColor &color) {
    if (dstRect.IsEmpty()) {
        return;
    }
    uint32 a = (color.a * this->ui8Opacity) / 255;
    this->addQuad(dstRect.Offset(this->translationX, this->translationY),
                  TBRect(), VER_COL(color.r, color.g, color.b, a), nullptr, nullptr);
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::FlushBitmap(TBBitmap* bitmap) {
    // Flush the batch if it's using this bitmap (that is about to change or be deleted)
    if (this->curBatchIndex < MaxNumBatches) {
        const Batch& curBatch = this->batches[this->curBatchIndex];
        auto* oryolBitmap = (tbOryolBitmap*) bitmap;
        if (oryolBitmap && (oryolBitmap->texture == curBatch.texture)) {
            this->flushBatch();
        }
    }
}
        
//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::FlushBitmapFragment(TBBitmapFragment* fragment) {
    // Flush the batch if it is using this fragment (that is about to change or be deleted)
    // We know if it is in use in the current batch if its batch_id matches the current
    // batch_id in our (one and only) batch.
    // If we switch to a more advance batching system with multiple batches, we need to
    // solve this a bit differently.
    if (this->curBatchIndex < MaxNumBatches) {
        const Batch& curBatch = this->batches[this->curBatchIndex];
        if (curBatch.batchId == fragment->m_batch_id) {
            this->flushBatch();
        }
    }
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::addQuad(const TBRect& dstRect, const TBRect& srcRect, uint32 color, TBBitmap* bitmap, TBBitmapFragment* fragment) {

    if (this->curBatchIndex >= MaxNumBatches) {
        return;
    }
    if ((this->curVertexIndex + 6) >= MaxNumVertices) {
        return;
    }
    
    Batch* curBatch = &(this->batches[this->curBatchIndex]);
    auto* oryolBitmap = (tbOryolBitmap*) bitmap;
    if (oryolBitmap) {
        if (curBatch->texture != oryolBitmap->texture) {
            curBatch = this->flushBatch();
            if (!curBatch) {
                return;
            }
            curBatch->texture = oryolBitmap->texture;
        }
        int bitmapWidth = bitmap->Width();
        int bitmapHeight = bitmap->Height();
        this->u0 = (float) srcRect.x / bitmapWidth;
        this->v0 = (float) srcRect.y / bitmapHeight;
        this->u1 = (float) (srcRect.x + srcRect.w) / bitmapWidth;
        this->v1 = (float) (srcRect.y + srcRect.h) / bitmapHeight;
    }
    else {
        // bitmap-less, use white texture
        if (curBatch->texture != this->whiteTexture) {
            curBatch = this->flushBatch();
            if (!curBatch) {
                return;
            }
            curBatch->texture = this->whiteTexture;
        }
    }
    curBatch->fragment = fragment;
    if (fragment) {
        fragment->m_batch_id = curBatch->batchId;
    }
    
    Vertex* v = &(this->vertexData[this->curVertexIndex]);
    this->curVertexIndex += 6;
    
    v[0].x = (float) dstRect.x;
    v[0].y = (float) (dstRect.y + dstRect.h);
    v[0].u = this->u0;
    v[0].v = this->v1;
    v[0].c = color;
    
    v[1].x = (float) (dstRect.x + dstRect.w);
    v[1].y = (float) (dstRect.y + dstRect.h);
    v[1].u = this->u1;
    v[1].v = this->v1;
    v[1].c = color;
    
    v[2].x = (float) dstRect.x;
    v[2].y = (float) dstRect.y;
    v[2].u = this->u0;
    v[2].v = this->v0;
    v[2].c = color;
    
    v[3].x = (float) dstRect.x;
    v[3].y = (float) dstRect.y;
    v[3].u = this->u0;
    v[3].v = this->v0;
    v[3].c = color;
    
    v[4].x = (float) (dstRect.x + dstRect.w);
    v[4].y = (float) (dstRect.y + dstRect.h);
    v[4].u = this->u1;
    v[4].v = this->v1;
    v[4].c = color;
    
    v[5].x = (float) (dstRect.x + dstRect.w);
    v[5].y = (float) dstRect.y;
    v[5].u = this->u1;
    v[5].v = this->v0;
    v[5].c = color;
}

//------------------------------------------------------------------------------
tbOryolBatchRenderer::Batch*
tbOryolBatchRenderer::flushBatch() {
    o_assert_dbg(this->curBatchIndex < MaxNumBatches);
    Batch* curBatch = &(this->batches[this->curBatchIndex]);

    // empty batch?
    const int numVerts = this->curVertexIndex - curBatch->startIndex;
    if (0 == numVerts) {
        return curBatch;
    }
    
    // prevent re-entrancy
    // FIXME: does this actually ever happen?
    if (this->inFlushBatch) {
        return curBatch;
    }
    this->inFlushBatch = true;
    if (curBatch->fragment) {
        // Now it's time to ensure the bitmap data is up to date. A call to GetBitmap
        // with TB_VALIDATE_ALWAYS should guarantee that its data is validated.
        curBatch->fragment->GetBitmap(TB_VALIDATE_ALWAYS);
    }
    this->inFlushBatch = false;
        
    curBatch->numVertices = numVerts;
    curBatch->clipRect.x = this->clipRect.x;
    curBatch->clipRect.y = this->screenRect.h - (this->clipRect.y + this->clipRect.h);
    curBatch->clipRect.w = this->clipRect.w;
    curBatch->clipRect.h = this->clipRect.h;
    
    this->curBatchIndex++;
    this->batchId++;
    
    if (this->curBatchIndex < MaxNumBatches) {
        // initialize the new batch
        curBatch = &this->batches[this->curBatchIndex];
        curBatch->startIndex = this->curVertexIndex;
        curBatch->numVertices = 0;
        curBatch->texture.Invalidate();
        curBatch->fragment = nullptr;
        curBatch->batchId = this->batchId;
        return &(this->batches[this->curBatchIndex]);
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
void
tbOryolBatchRenderer::drawBatches() {

    // NOTE: curBatchIndex is always one-past-end
    if (this->curBatchIndex > 1) {

        TBUIShader::VSParams vsParams;
        vsParams.Ortho = glm::ortho(0.0f, float(this->screenRect.w),
            (float)this->screenRect.h, 0.0f,
            -1.0f, 1.0f);
        const int vertexDataSize = this->curVertexIndex * this->vertexLayout.ByteSize();

        this->tbClipRect = this->screenRect;
        Gfx::UpdateVertices(this->drawState.Mesh[0], this->vertexData, vertexDataSize);
        for (int batchIndex = 0; batchIndex < this->curBatchIndex; batchIndex++) {
            const Batch& batch = this->batches[batchIndex];
            Gfx::ApplyScissorRect(batch.clipRect.x, batch.clipRect.y, batch.clipRect.w, batch.clipRect.h);
            this->drawState.FSTexture[TBUITextures::Texture] = batch.texture.IsValid() ? batch.texture : this->whiteTexture;
            Gfx::ApplyDrawState(drawState);
            Gfx::ApplyUniformBlock(vsParams);
            Gfx::Draw(PrimitiveGroup(batch.startIndex, batch.numVertices));
        }
        Gfx::ApplyScissorRect(this->screenRect.x, this->screenRect.y, this->screenRect.w, this->screenRect.h);
    }
}

//------------------------------------------------------------------------------
tb::TBBitmap *
tbOryolBatchRenderer::CreateBitmap(int width, int height, uint32 *data)
{
    Log::Info("tbOryolBatchRenderer::CreateBitmap() called\n");
    
    auto *bitmap = new tbOryolBitmap(this);
    bitmap->Init(width, height, data);
    return bitmap;
}
    
} // namespace _priv
} // namespace Oryol
