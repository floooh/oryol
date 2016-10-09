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
    o_assert(w > 0);
    o_assert(h > 0);

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
    o_assert(w > 0);
    o_assert(h > 0);
    o_assert(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    
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
    o_assert(w > 0);
    o_assert(h > 0);
    o_assert(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert(!PixelFormat::IsCompressedFormat(fmt));
    o_assert(TextureType::Texture2D == type);
    o_assert((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    
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
