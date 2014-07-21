//------------------------------------------------------------------------------
//  TextureSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureSetup.h"

namespace Oryol {
namespace Render {

using namespace Resource;
    
//------------------------------------------------------------------------------
TextureSetup::TextureSetup() :
IOLane(0),
Width(0),
Height(0),
RelWidth(0.0f),
RelHeight(0.0f),
ColorFormat(PixelFormat::InvalidPixelFormat),
DepthFormat(PixelFormat::InvalidPixelFormat),
WrapU(TextureWrapMode::Repeat),
WrapV(TextureWrapMode::Repeat),
WrapW(TextureWrapMode::Repeat),
MagFilter(TextureFilterMode::Nearest),
MinFilter(TextureFilterMode::Nearest),
shouldSetupFromFile(false),
shouldSetupFromImageFileData(false),
shouldSetupFromPixelData(false),
shouldSetupAsRenderTarget(false),
isRelSizeRenderTarget(false),
hasSharedDepth(false),
hasMipMaps(false) {
    // empty
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::AsRenderTarget(const class Locator& loc, int32 w, int32 h, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert(w > 0);
    o_assert(h > 0);
    o_assert(colorFmt != PixelFormat::InvalidPixelFormat);

    TextureSetup setup;
    setup.shouldSetupAsRenderTarget = true;
    setup.Locator = loc;
    setup.Width = w;
    setup.Height = h;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
    setup.WrapU = TextureWrapMode::ClampToEdge;
    setup.WrapV = TextureWrapMode::ClampToEdge;
    setup.WrapW = TextureWrapMode::InvalidTextureWrapMode;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromFile(const class Locator& loc, TextureSetup bluePrint) {
    TextureSetup setup(bluePrint);
    setup.shouldSetupFromFile = true;
    setup.Locator = loc;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromImageFileData(const class Locator& loc, TextureSetup bluePrint) {
    TextureSetup setup(bluePrint);
    setup.shouldSetupFromImageFileData = true;
    setup.Locator = loc;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromPixelData(const class Locator& loc, int32 w, int32 h, bool hasMipMaps, PixelFormat::Code fmt) {
    o_assert(w > 0);
    o_assert(h > 0);
    o_assert(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert(!PixelFormat::IsCompressedFormat(fmt));
    
    TextureSetup setup;
    setup.shouldSetupFromPixelData = true;
    setup.hasMipMaps = hasMipMaps;
    setup.Locator = loc;
    setup.Width = w;
    setup.Height = h;
    setup.ColorFormat = fmt;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::AsRelSizeRenderTarget(const class Locator& loc, float32 relWidth, float32 relHeight, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert(relWidth > 0.0f);
    o_assert(relHeight > 0.0f);
    o_assert(colorFmt != PixelFormat::InvalidPixelFormat);

    TextureSetup setup;
    setup.shouldSetupAsRenderTarget = true;
    setup.isRelSizeRenderTarget = true;
    setup.Locator = loc;
    setup.RelWidth = relWidth;
    setup.RelHeight = relHeight;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
    setup.WrapU = TextureWrapMode::ClampToEdge;
    setup.WrapV = TextureWrapMode::ClampToEdge;
    setup.WrapW = TextureWrapMode::InvalidTextureWrapMode;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::AsSharedDepthRenderTarget(const class Locator& loc, PixelFormat::Code colorFmt, const Id& depthRenderTarget) {
    o_assert(colorFmt != PixelFormat::InvalidPixelFormat);

    TextureSetup setup;
    setup.shouldSetupAsRenderTarget = true;
    setup.Locator = loc;
    setup.hasSharedDepth = true;
    setup.ColorFormat = colorFmt;
    setup.DepthRenderTarget = depthRenderTarget;
    setup.WrapU = TextureWrapMode::ClampToEdge;
    setup.WrapV = TextureWrapMode::ClampToEdge;
    setup.WrapW = TextureWrapMode::InvalidTextureWrapMode;
    return setup;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromFile() const {
    return this->shouldSetupFromFile;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromImageFileData() const {
    return this->shouldSetupFromImageFileData;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromPixelData() const {
    return this->shouldSetupFromPixelData;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupAsRenderTarget() const {
    return this->shouldSetupAsRenderTarget;
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

//------------------------------------------------------------------------------
bool
TextureSetup::HasMipMaps() const {
    return this->hasMipMaps;
}

} // namespace Render
} // namespace Oryol