#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureSetup
    @ingroup Gfx
    @brief setup object for textures and render targets
*/
#include "Core/Containers/StaticArray.h"
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/SamplerState.h"
#include "Gfx/Core/ClearState.h"

namespace Oryol {
    
class TextureSetup {
public:
    /// asynchronously load from file
    static TextureSetup FromFile(const Locator& loc, Id placeholder=Id::InvalidId());
    /// asynchronously load from file
    static TextureSetup FromFile(const Locator& loc, const TextureSetup& blueprint=TextureSetup(), Id placeholder=Id::InvalidId());
    /// setup a texture from a image file data in memory
    static TextureSetup FromImageFileData(const TextureSetup& blueprint=TextureSetup());
    /// setup texture from raw pixel data
    static TextureSetup FromPixelData(int32 w, int32 h, int32 numMipMaps, TextureType::Code type, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
    /// setup as absolute-size render target
    static TextureSetup RenderTarget(int32 w, int32 h);
    /// setup as render target with size relative to current display size
    static TextureSetup RelSizeRenderTarget(float32 relWidth, float32 relHeight);
    /// create render target with shared depth buffer
    static TextureSetup SharedDepthRenderTarget(const Id& depthRenderTarget);

    /// default constructor
    TextureSetup();
    /// return true if texture should be setup from a file
    bool ShouldSetupFromFile() const;
    /// return true if texture should be setup from image file data in memory
    bool ShouldSetupFromImageFileData() const;
    /// return true if texture should be setup from raw pixel data
    bool ShouldSetupFromPixelData() const;
    /// return true if texture should be setup as render target
    bool ShouldSetupAsRenderTarget() const;
    /// return true if rel-size render target
    bool IsRelSizeRenderTarget() const;
    /// return true if render target has depth (shared or non-shared)
    bool HasDepth() const;
    /// return true if render target with shared depth buffer
    bool HasSharedDepth() const;
    
    /// texture type (default is Texture2D)
    TextureType::Code Type;
    /// the width in pixels (only if absolute-size render target)
    int32 Width;
    /// the height in pixels (only if absolute-size render target)
    int32 Height;
    /// display-relative width (only if screen render target)
    float32 RelWidth;
    /// display-relative height (only if screen render target)
    float32 RelHeight;
    /// number of mipmaps (default is 1, only for FromPixelData)
    int32 NumMipMaps;
    /// the color pixel format (only if render target)
    PixelFormat::Code ColorFormat;
    /// the depth pixel format (only if render target, InvalidPixelFormat if render target should not have depth buffer)
    PixelFormat::Code DepthFormat;
    /// resource id of render target which owns the depth buffer (only if render target with shared depth buffer)
    Id DepthRenderTarget;

    /// sampler state
    SamplerState Sampler;
    /// optional clear hint for render targets, this is used as hint by D3D12 to optimize clears
    ClearState ClearHint;

    /// resource locator
    class Locator Locator;
    /// resource placeholder
    Id Placeholder;
    
    /// pixel data mipmap image offsets
    StaticArray<StaticArray<int32, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> ImageOffsets;
    /// pixel data mipmap image sizes
    StaticArray<StaticArray<int32, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> ImageSizes;
    
private:
    bool setupFromFile : 1;
    bool setupFromImageFileData : 1;
    bool setupFromPixelData : 1;
    bool setupAsRenderTarget : 1;
    bool isRelSizeRenderTarget : 1;
    bool hasSharedDepth : 1;
    bool hasMipMaps : 1;
};
    
} // namespace Oryol
