
#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::tbOryolRenderer
    @brief TurboBadger UI renderer for Oryol
    
    NOTE: currently this is simply a copy of the standard turbobadger 
    GL renderer, should be ported to using Oryol's Gfx module!
*/
#include "Core/Types.h"
#include "Gfx/Gfx.h"
#include "renderers/tb_renderer_batcher.h"

namespace Oryol {
namespace _priv {

class tbOryolRenderer;

//------------------------------------------------------------------------------
class tbOryolBitmap : public tb::TBBitmap
{
public:
    /// bitmap wrapper constructor
    tbOryolBitmap(tbOryolRenderer *renderer);
    /// bitmap wrapper destructor
    ~tbOryolBitmap();
    /// initialize bitmap with data
    bool Init(int width, int height, tb::uint32 *data);
    /// return width
    virtual int Width() { return this->width; }
    /// return height
    virtual int Height() { return this->height; }
    /// initialize with new data
    virtual void SetData(tb::uint32 *data);
    
    /// bind the bitmap (set shader param)
    void Bind();
    
private:
    /// destroy texture
    void DestroyTexture();
    /// create texture
    void CreateTexture(tb::uint32* data);
    
    tbOryolRenderer *renderer;
    int32 width;
    int32 height;
    
    ResourceLabel label;
    Id texture;
};

//------------------------------------------------------------------------------
class tbOryolRenderer : public tb::TBRendererBatcher
{
public:
    /// begin painting
    virtual void BeginPaint(int render_target_w, int render_target_h);
    /// end painting
    virtual void EndPaint();
    /// create a new bitmap object
    virtual tb::TBBitmap *CreateBitmap(int width, int height, tb::uint32 *data);

    /// render a batch
    virtual void RenderBatch(Batch *batch);
    /// set clip rect
    virtual void SetClipRect(const tb::TBRect &rect);
};

}   // namespace _priv
}   // namespace Oryol

