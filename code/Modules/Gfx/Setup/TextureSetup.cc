//------------------------------------------------------------------------------
//  TextureSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromFile(const class Locator& loc, Id placeholder) {
    TextureSetup setup;
    setup.setupFromFile = true;
    setup.Locator = loc;
    setup.Placeholder = placeholder;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromFile(const class Locator& loc, const TextureSetup& blueprint, Id placeholder) {
    TextureSetup setup(blueprint);
    setup.setupFromFile = true;
    setup.Locator = loc;
    setup.Placeholder = placeholder;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::RenderTarget(int w, int h) {
    o_assert_dbg((w > 0) && (h > 0));

    TextureSetup setup;
    setup.setupAsRenderTarget = true;
    setup.Width = w;
    setup.Height = h;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromPixelData(int w, int h, int numMipMaps, TextureType::Code type, PixelFormat::Code fmt, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    
    TextureSetup setup(blueprint);
    setup.setupFromPixelData = true;
    setup.Type = type;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.ImageData.NumFaces = (type == TextureType::Texture2D) ? 1 : 6;
    setup.ImageData.NumMipMaps = numMipMaps;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::Empty(int w, int h, int numMipMaps, TextureType::Code type, PixelFormat::Code fmt, Usage::Code usage) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((TextureType::Texture2D == type) || (TextureType::TextureCube == type));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    
    TextureSetup setup;
    setup.setupEmpty = true;
    setup.Type = type;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::Empty3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage) {
    o_assert_dbg((w > 0) && (h > 0) && (d > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup;
    setup.setupEmpty = true;
    setup.Type = TextureType::Texture3D;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = d;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::EmptyArray(int w, int h, int slices, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage) {
    o_assert_dbg((w > 0) && (h > 0) && (slices > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup;
    setup.setupEmpty = true;
    setup.Type = TextureType::TextureArray;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = slices;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromFile() const {
    return this->setupFromFile;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromPixelData() const {
    return this->setupFromPixelData;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupEmpty() const {
    return this->setupEmpty;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupAsRenderTarget() const {
    return this->setupAsRenderTarget;
}

//------------------------------------------------------------------------------
bool
TextureSetup::HasDepth() const {
    return this->DepthFormat != PixelFormat::InvalidPixelFormat;
}

} // namespace Oryol
