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
    class Locator Locator;
    TextureType::Code Type{TextureType::InvalidTextureType};
    PixelFormat::Code ColorFormat{PixelFormat::InvalidPixelFormat};
    PixelFormat::Code DepthFormat{PixelFormat::InvalidPixelFormat};
    Usage::Code TextureUsage{Usage::InvalidUsage};
    int32 Width{0};
    int32 Height{0};
    int32 Depth{0};
    bool HasMipmaps{false};
    bool IsRenderTarget{false};
    bool HasDepthBuffer{false};
    bool HasSharedDepthBuffer{false};
    bool IsDepthTexture{false};
};
    
} // namespace Oryol