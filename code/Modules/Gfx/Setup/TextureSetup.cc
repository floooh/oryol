//------------------------------------------------------------------------------
//  TextureSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
TextureSetup::TextureSetup() :
TextureUsage(Usage::Immutable),
Type(TextureType::Texture2D),
Width(0),
Height(0),
RelWidth(0.0f),
RelHeight(0.0f),
NumMipMaps(1),
ColorFormat(PixelFormat::RGBA8),
DepthFormat(PixelFormat::None),
Locator(Locator::NonShared()),
setupFromFile(false),
setupFromPixelData(false),
setupEmpty(false),
setupAsRenderTarget(false),
isRelSizeRenderTarget(false),
hasSharedDepth(false),
hasMipMaps(false) {
    // empty
}

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
TextureSetup::RenderTarget(int32 w, int32 h) {
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
TextureSetup::FromPixelData(int32 w, int32 h, int32 numMipMaps, TextureType::Code type, PixelFormat::Code fmt, const TextureSetup& blueprint) {
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
TextureSetup::Empty(int32 w, int32 h, int32 numMipMaps, TextureType::Code type, PixelFormat::Code fmt, Usage::Code usage) {
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
TextureSetup
TextureSetup::RelSizeRenderTarget(float32 relWidth, float32 relHeight) {
    o_assert(relWidth > 0.0f);
    o_assert(relHeight > 0.0f);

    TextureSetup setup;
    setup.setupAsRenderTarget = true;
    setup.isRelSizeRenderTarget = true;
    setup.RelWidth = relWidth;
    setup.RelHeight = relHeight;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::SharedDepthRenderTarget(const Id& depthRenderTarget) {
    o_assert(depthRenderTarget.IsValid() && depthRenderTarget.Type == GfxResourceType::Texture);

    TextureSetup setup;
    setup.setupAsRenderTarget = true;
    setup.hasSharedDepth = true;
    setup.DepthRenderTarget = depthRenderTarget;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
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
TextureSetup::IsRelSizeRenderTarget() const {
    return this->isRelSizeRenderTarget;
}

//------------------------------------------------------------------------------
bool
TextureSetup::HasDepth() const {
    return this->hasSharedDepth || (this->DepthFormat != PixelFormat::InvalidPixelFormat);
}

//------------------------------------------------------------------------------
bool
TextureSetup::HasSharedDepth() const {
    return this->hasSharedDepth;
}

} // namespace Oryol
