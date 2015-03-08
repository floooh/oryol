//------------------------------------------------------------------------------
//  TextureSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
TextureSetup::TextureSetup() :
Type(TextureType::Texture2D),
Width(0),
Height(0),
RelWidth(0.0f),
RelHeight(0.0f),
NumMipMaps(1),
ColorFormat(PixelFormat::RGBA8),
DepthFormat(PixelFormat::None),
WrapU(TextureWrapMode::Repeat),
WrapV(TextureWrapMode::Repeat),
WrapW(TextureWrapMode::Repeat),
MagFilter(TextureFilterMode::Nearest),
MinFilter(TextureFilterMode::Nearest),
Locator(Locator::NonShared()),
setupFromFile(false),
setupFromImageFileData(false),
setupFromPixelData(false),
setupAsRenderTarget(false),
isRelSizeRenderTarget(false),
hasSharedDepth(false),
hasMipMaps(false) {
    for (auto& offsets : this->ImageOffsets) {
        offsets.Fill(0);
    }
    for (auto& sizes : this->ImageSizes) {
        sizes.Fill(0);
    }
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
TextureSetup::FromFile(const class Locator& loc, TextureSetup blueprint, Id placeholder) {
    TextureSetup setup(blueprint);
    setup.setupFromFile = true;
    setup.setupFromImageFileData = false;
    setup.setupFromPixelData = false;
    setup.setupAsRenderTarget = false;
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
    setup.WrapU = TextureWrapMode::ClampToEdge;
    setup.WrapV = TextureWrapMode::ClampToEdge;
    setup.WrapW = TextureWrapMode::InvalidTextureWrapMode;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromImageFileData(TextureSetup bluePrint) {
    TextureSetup setup(bluePrint);
    setup.setupFromFile = false;
    setup.setupFromImageFileData = true;
    setup.setupFromPixelData = false;
    setup.setupAsRenderTarget = false;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromPixelData(int32 w, int32 h, int32 numMipMaps, TextureType::Code type, PixelFormat::Code fmt, TextureSetup blueprint) {
    o_assert(w > 0);
    o_assert(h > 0);
    o_assert(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert((numMipMaps > 0) && (numMipMaps < MaxNumMipMaps));
    
    TextureSetup setup(blueprint);
    setup.setupFromFile = false;
    setup.setupFromImageFileData = false;
    setup.setupFromPixelData = true;
    setup.setupAsRenderTarget = false;
    
    setup.setupFromPixelData = true;
    setup.Type = type;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
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
    setup.WrapU = TextureWrapMode::ClampToEdge;
    setup.WrapV = TextureWrapMode::ClampToEdge;
    setup.WrapW = TextureWrapMode::InvalidTextureWrapMode;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::SharedDepthRenderTarget(const Id& depthRenderTarget) {
    o_assert(depthRenderTarget.IsValid() && depthRenderTarget.Type() == GfxResourceType::Texture);

    TextureSetup setup;
    setup.setupAsRenderTarget = true;
    setup.hasSharedDepth = true;
    setup.DepthRenderTarget = depthRenderTarget;
    setup.WrapU = TextureWrapMode::ClampToEdge;
    setup.WrapV = TextureWrapMode::ClampToEdge;
    setup.WrapW = TextureWrapMode::InvalidTextureWrapMode;
    return setup;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromFile() const {
    return this->setupFromFile;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromImageFileData() const {
    return this->setupFromImageFileData;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromPixelData() const {
    return this->setupFromPixelData;
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
