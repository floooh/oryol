//------------------------------------------------------------------------------
//  tbOryolRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Memory/Memory.h"
#include "tbOryolRenderer.h"
#include "tb_bitmap_fragment.h"
#include "tb_system.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
tbOryolBitmap::tbOryolBitmap(tbOryolRenderer* rnd) :
renderer(rnd),
width(0),
height(0),
label(ResourceLabel::Invalid) {
    // empty
}

//------------------------------------------------------------------------------
tbOryolBitmap::~tbOryolBitmap() {
    if (this->texture.IsValid()) {
        this->DestroyTexture();
    }
}

//------------------------------------------------------------------------------
bool
tbOryolBitmap::Init(int w, int h, tb::uint32* data) {
    o_assert_dbg(tb::TBGetNearestPowerOfTwo(w) == w);
    o_assert_dbg(tb::TBGetNearestPowerOfTwo(h) == h);
    this->width = w;
    this->height = h;
    this->CreateTexture(data);
    return true;
}

//------------------------------------------------------------------------------
void
tbOryolBitmap::SetData(uint32* data) {
    o_assert_dbg(this->texture.IsValid());
    this->DestroyTexture();
    this->CreateTexture(data);
}

//------------------------------------------------------------------------------
void
tbOryolBitmap::CreateTexture(tb::uint32* data) {
    o_assert_dbg(!this->texture.IsValid());

    // hmm this kinda sucks, a 'view mode' stream would be nice
    auto stream = MemoryStream::Create();
    stream->Open(OpenMode::WriteOnly);
    stream->Write(data, this->width * this->height * sizeof(tb::uint32));
    stream->Close();
    
    this->label = Gfx::Resource().PushLabel();
    auto texSetup = TextureSetup::FromPixelData(this->width, this->height, 1, TextureType::Texture2D, PixelFormat::RGBA8);
    this->texture = Gfx::Resource().Create(texSetup, stream);
    Gfx::Resource().PopLabel();
}

//------------------------------------------------------------------------------
void
tbOryolBitmap::DestroyTexture() {
    o_assert_dbg(this->texture.IsValid());
    this->renderer->FlushBitmap(this);
    Gfx::Resource().Destroy(this->label);
    this->texture.Invalidate();
    this->label = ResourceLabel::Invalid;
}

//------------------------------------------------------------------------------
void
tbOryolBitmap::Bind() {
    o_assert_dbg(this->texture.IsValid());
    
    // FIXME!
}

//------------------------------------------------------------------------------
void tbOryolRenderer::BeginPaint(int render_target_w, int render_target_h)
{
    TBRendererBatcher::BeginPaint(render_target_w, render_target_h);
    
    // FIXME!
    /*
    g_current_texture = (GLuint)-1;
    g_current_batch = nullptr;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    Ortho2D(0, (GLfloat)render_target_w, (GLfloat)render_target_h, 0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, render_target_w, render_target_h);
    glScissor(0, 0, render_target_w, render_target_h);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    */
}

//------------------------------------------------------------------------------
void tbOryolRenderer::EndPaint()
{
    TBRendererBatcher::EndPaint();
}

//------------------------------------------------------------------------------
tb::TBBitmap *
tbOryolRenderer::CreateBitmap(int width, int height, uint32 *data)
{
    tbOryolBitmap *bitmap = new tbOryolBitmap(this);
    bitmap->Init(width, height, data);
    return bitmap;
}

//------------------------------------------------------------------------------
void tbOryolRenderer::RenderBatch(Batch *batch)
{
    // FIXME!
    /*
    // Bind texture and array pointers
    BindBitmap(batch->bitmap);
    if (g_current_batch != batch)
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void *) &batch->vertex[0].r);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void *) &batch->vertex[0].u);
        glVertexPointer(2, GL_FLOAT, sizeof(Vertex), (void *) &batch->vertex[0].x);
        g_current_batch = batch;
    }

    // Flush
    glDrawArrays(GL_TRIANGLES, 0, batch->vertex_count);
    */
}

//------------------------------------------------------------------------------
void tbOryolRenderer::SetClipRect(const tb::TBRect &rect)
{
    // FIXME!
//      glScissor(m_clip_rect.x, m_screen_rect.h - (m_clip_rect.y + m_clip_rect.h), m_clip_rect.w, m_clip_rect.h);
}

} // namespace _priv
} // namespace Oryol
