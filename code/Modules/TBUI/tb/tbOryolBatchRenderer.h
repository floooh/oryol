#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::tbOryolBatchRenderer
    @brief properly optimized turbobadger ui renderer for oryol
*/
#include "Core/Types.h"
#include "Gfx/Gfx.h"
#include "tb_renderer.h"

namespace Oryol {
namespace _priv {

class tbOryolBatchRenderer : public tb::TBRenderer {
public:
    /// constructor
    tbOryolBatchRenderer();
    /// destructor
    virtual ~tbOryolBatchRenderer();

    /// setup the renderer
    void Setup();
    /// discard the renderer
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;
    
    /// begin rendering a frame
    virtual void BeginPaint(int renderTargetWidth, int renderTargetHeight);
    /// finish rendering a frame
    virtual void EndPaint();

    /// set current translation
    virtual void Translate(int dx, int dy);

    /// set current opacity
    virtual void SetOpacity(float opacity);
    /// get current opacity
    virtual float GetOpacity();

    /// set current cliprect
    virtual tb::TBRect SetClipRect(const tb::TBRect& rect, bool addToCurrent);
    /// get current cliprect
    virtual tb::TBRect GetClipRect();
    
    /// draw functions
    virtual void DrawBitmap(const tb::TBRect& dstRect, const tb::TBRect& srcRect, tb::TBBitmapFragment* bitmap_fragment);
    virtual void DrawBitmap(const tb::TBRect& dstRect, const tb::TBRect& srcRect, tb::TBBitmap* bitmap);
    virtual void DrawBitmapColored(const tb::TBRect& dstRect, const tb::TBRect& srcRect, const tb::TBColor& color, tb::TBBitmapFragment* bitmapFragment);
    virtual void DrawBitmapColored(const tb::TBRect& dstRect, const tb::TBRect& srcRect, const tb::TBColor& color, tb::TBBitmap* bitmap);
    virtual void DrawBitmapTile(const tb::TBRect& dstRect, tb::TBBitmap* bitmap);
    virtual void DrawRect(const tb::TBRect& dstRect, const tb::TBColor& color);
    virtual void DrawRectFill(const tb::TBRect& dstRect, const tb::TBColor& color);
    virtual void FlushBitmap(tb::TBBitmap* bitmap);
    virtual void FlushBitmapFragment(tb::TBBitmapFragment* bitmapFragment);

    /// create a new bitmap object
    virtual tb::TBBitmap *CreateBitmap(int width, int height, tb::uint32 *data);
    
    struct Batch {
        uint16_t startIndex = 0;
        uint16_t numVertices = 0;
        Id texture;
        uint32_t batchId = 0;
        tb::TBRect clipRect;
        tb::TBBitmapFragment* fragment = nullptr;
    };
    /// add a single quad
    void addQuad(const tb::TBRect& dstRect, const tb::TBRect& srcRect, uint32_t color, tb::TBBitmap* bitmap, tb::TBBitmapFragment* fragment);
    /// setup white texture used for untextured areas
    void setupWhiteTexture();
    /// setup the mesh, shader and draw state
    void setupResources();
    /// add texture to be defer-deleted in EndPaint()
    void deferDeleteTexture(ResourceLabel label);
    /// delete textures added with deferDeleteTexture
    void deleteTextures();
    /// flush current batch (adds curBatch to batches array)
    Batch* flushBatch();
    /// draw this frame's patches
    void drawBatches();

private:
    friend class tbOryolBitmap;

    bool isValid;
    
    float fOpacitity;
    uint8_t ui8Opacity;
    tb::TBRect screenRect;
    tb::TBRect clipRect;
    int translationX;
    int translationY;
    float u0, v0, u1, v1;
    uint32_t batchId;
    bool inFlushBatch;
    
    static const int MaxNumVertices = 64 * 1024;
    static const int MaxNumBatches = 1024;
    
    ResourceLabel resLabel;
    VertexLayout vertexLayout;
    tb::TBRect tbClipRect;
    Id whiteTexture;
    DrawState drawState;
    Array<ResourceLabel> texturesForDeletion;
    int curBatchIndex;
    Batch batches[MaxNumBatches];
    uint16_t curVertexIndex;
    struct Vertex {
        float x, y;
        float u, v;
        uint32_t c;
    } vertexData[MaxNumVertices];
};

} // namespace _priv
} // namespace Oryol