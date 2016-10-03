#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureSetup
    @ingroup Gfx
    @brief setup object for textures and render targets
*/
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/SamplerState.h"
#include "Gfx/Core/ClearState.h"
#include "Gfx/Attrs/ImageDataAttrs.h"

namespace Oryol {
    
class TextureSetup {
public:
    /// default constructor (workaround for gcc)
    TextureSetup() { };
    /// asynchronously load from file
    static TextureSetup FromFile(const Locator& loc, Id placeholder=Id::InvalidId());
    /// asynchronously load from file
    static TextureSetup FromFile(const Locator& loc, const TextureSetup& blueprint=TextureSetup(), Id placeholder=Id::InvalidId());
    /// setup texture from raw pixel data
    static TextureSetup FromPixelData(int w, int h, int numMipMaps, TextureType::Code type, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
    /// setup empty texture (usually for dynamic streaming of CPU generated texture data)
    static TextureSetup Empty(int w, int h, int numMipMaps, TextureType::Code type, PixelFormat::Code fmt, Usage::Code Usage);
    /// setup as absolute-size render target
    static TextureSetup RenderTarget(int w, int h);
    /// create render target with shared depth buffer
    static TextureSetup SharedDepthRenderTarget(const Id& depthRenderTarget);

    /// return true if texture should be setup from a file
    bool ShouldSetupFromFile() const;
    /// return true if texture should be setup from raw pixel data
    bool ShouldSetupFromPixelData() const;
    /// return true if texture should be created empty
    bool ShouldSetupEmpty() const;
    /// return true if texture should be setup as render target
    bool ShouldSetupAsRenderTarget() const;
    /// return true if render target has depth (shared or non-shared)
    bool HasDepth() const;
    /// return true if render target with shared depth buffer
    bool HasSharedDepth() const;

    /// intended usage
    Usage::Code TextureUsage = Usage::Immutable;
    /// texture type
    TextureType::Code Type = TextureType::Texture2D;
    /// the width in pixels
    int Width = 0;
    /// the height in pixels
    int Height = 0;
    /// number of mipmaps (default is 1, only for FromPixelData)
    int NumMipMaps = 1;
    /// the color pixel format
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    /// the depth pixel format (only if render target, PixelFormat::None if render target should not have depth buffer)
    PixelFormat::Code DepthFormat = PixelFormat::None;
    /// MSAA samples (2, 4, 8... no MSAA: 1), check MSAARenderTargets feature availability!
    int SampleCount = 1;
    /// resource id of render target which owns the depth buffer (only if render target with shared depth buffer)
    Id DepthRenderTarget;

    /// sampler state
    SamplerState Sampler;
    /// optional clear hint for render targets, this is used as hint by D3D12 to optimize clears
    ClearState ClearHint;

    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// resource placeholder
    Id Placeholder;

    /// optional image surface offsets and sizes
    ImageDataAttrs ImageData;

private:
    bool setupFromFile = false;
    bool setupFromPixelData = false;
    bool setupEmpty = false;
    bool setupAsRenderTarget = false;
    bool hasSharedDepth = false;
    bool hasMipMaps = false;
};
    
} // namespace Oryol
