#pragma once
//------------------------------------------------------------------------------
/**
    @class tbOryolBitmap
    @brief Oryol wrapper for TBBitmap
*/
#include "Core/Types.h"
#include "tb_renderer.h"
#include "Resource/ResourceLabel.h"
#include "Resource/Id.h"

namespace Oryol {
namespace _priv {

class tbOryolRenderer;

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
    
    /// destroy texture
    void destroyTexture();
    /// create texture
    void createTexture(tb::uint32* data);
    
    tbOryolRenderer *renderer;
    int32 width;
    int32 height;
    
    ResourceLabel label;
    Id texture;
};

} // namespace _priv
} // namespace Oryol
