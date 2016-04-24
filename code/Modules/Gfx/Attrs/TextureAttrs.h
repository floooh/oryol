#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureAttrs
    @ingroup Gfx
    @brief holds the public attributes of a Texture object
    
    @todo: describe TextureAttrs
*/
#include "Core/Types.h"
#include "Resource/Locator.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
    
struct TextureAttrs {
    /// texture locator (usually the URL of the texture file)
    class Locator Locator;
    /// the texture type (2D, 3D, cube...)
    TextureType::Code Type{TextureType::InvalidTextureType};
    /// the RGBA pixel format of texture data
    PixelFormat::Code ColorFormat{PixelFormat::InvalidPixelFormat};
    /// optional depth format (only used for render target textures)
    PixelFormat::Code DepthFormat{PixelFormat::InvalidPixelFormat};
    /// texture usage hint
    Usage::Code TextureUsage{Usage::InvalidUsage};
    /// width of top-level mipmap in pixels
    int Width = 0;
    /// height of top-level mipmap in pixels
    int Height = 0;
    /// depth of top-level mipmap in pixels (only used for 3D textures)
    int Depth = 0;
    /// number of mipmaps (1 for 'no child mipmaps')
    int NumMipMaps = 1;
    /// true if this is a render target texture
    bool IsRenderTarget = false;
    /// true if this render target texture has an attached depth buffer
    bool HasDepthBuffer = false;
    /// true if this render target texture shared a depth buffer with another render target texture
    bool HasSharedDepthBuffer = false;
};
    
} // namespace Oryol