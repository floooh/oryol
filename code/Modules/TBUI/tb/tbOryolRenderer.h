
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

class tbOryolRenderer : public tb::TBRendererBatcher
{
public:
    /// constructor
    tbOryolRenderer();
    /// destructor
    virtual ~tbOryolRenderer();

    /// setup the renderer
    void Setup();
    /// discard the renderer
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;

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

private:
    friend class tbOryolBitmap;

    /// setup white texture used for untextured areas
    void setupWhiteTexture();
    /// setup the dynamic mesh
    void setupMesh();
    /// setup the draw state
    void setupDrawState();
    /// add texture to be defer-deleted in EndPaint()
    void deferDeleteTexture(ResourceLabel label);
    /// delete textures added with deferDeleteTexture
    void deleteTextures();
    
    static const int MaxNumVertices = 16 * 1024;
    static const int MaxNumBatches = 1024;

    bool isValid;
    ResourceLabel resLabel;
    VertexLayout vertexLayout;
    tb::TBRect tbClipRect;
    Id whiteTexture;
    Id mesh;
    Id drawState;
    Array<ResourceLabel> texturesForDeletion;
    int curBatchIndex;
    uint16 curVertexIndex;
    struct tbOryolVertex {
        float32 pos[2];
        float32 uv[2];
        uint32 color;
    } vertexData[MaxNumVertices];
    struct tbOryolBatch {
        uint16 startIndex = 0;
        uint16 numVertices = 0;
        Id texture;
        tb::TBRect clipRect;
    } batches[MaxNumBatches];
};

}   // namespace _priv
}   // namespace Oryol

