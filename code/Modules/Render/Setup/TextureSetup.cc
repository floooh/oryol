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
shouldSetupFromFile(false),
shouldSetupFromData(false),
shouldSetupAsRenderTarget(false),
isRelSizeRenderTarget(false),
hasSharedDepth(false),
ioLane(0),
width(0),
height(0),
relWidth(0.0f),
relHeight(0.0f),
colorFormat(PixelFormat::InvalidPixelFormat),
depthFormat(PixelFormat::InvalidPixelFormat),
wrapU(TextureWrapMode::Repeat),
wrapV(TextureWrapMode::Repeat),
wrapW(TextureWrapMode::Repeat),
magFilter(TextureFilterMode::Nearest),
minFilter(TextureFilterMode::Nearest) {
    // empty
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::AsRenderTarget(const Locator& loc, int32 w, int32 h, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert(w > 0);
    o_assert(h > 0);
    o_assert(colorFmt != PixelFormat::InvalidPixelFormat);

    TextureSetup setup;
    setup.shouldSetupAsRenderTarget = true;
    setup.locator = loc;
    setup.width = w;
    setup.height = h;
    setup.colorFormat = colorFmt;
    setup.depthFormat = depthFmt;
    setup.wrapU = TextureWrapMode::ClampToEdge;
    setup.wrapV = TextureWrapMode::ClampToEdge;
    setup.wrapW = TextureWrapMode::InvalidTextureWrapMode;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromFile(const Locator& loc) {
    TextureSetup setup;
    setup.shouldSetupFromFile = true;
    setup.locator = loc;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::AsRelSizeRenderTarget(const Locator& loc, float32 relWidth, float32 relHeight, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert(relWidth > 0.0f);
    o_assert(relHeight > 0.0f);
    o_assert(colorFmt != PixelFormat::InvalidPixelFormat);

    TextureSetup setup;
    setup.shouldSetupAsRenderTarget = true;
    setup.isRelSizeRenderTarget = true;
    setup.locator = loc;
    setup.relWidth = relWidth;
    setup.relHeight = relHeight;
    setup.colorFormat = colorFmt;
    setup.depthFormat = depthFmt;
    setup.wrapU = TextureWrapMode::ClampToEdge;
    setup.wrapV = TextureWrapMode::ClampToEdge;
    setup.wrapW = TextureWrapMode::InvalidTextureWrapMode;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::AsSharedDepthRenderTarget(const Locator& loc, PixelFormat::Code colorFmt, const Id& depthRenderTarget) {
    o_assert(colorFmt != PixelFormat::InvalidPixelFormat);

    TextureSetup setup;
    setup.shouldSetupAsRenderTarget = true;
    setup.locator = loc;
    setup.hasSharedDepth = true;
    setup.colorFormat = colorFmt;
    setup.depthRenderTarget = depthRenderTarget;
    setup.wrapU = TextureWrapMode::ClampToEdge;
    setup.wrapV = TextureWrapMode::ClampToEdge;
    setup.wrapW = TextureWrapMode::InvalidTextureWrapMode;
    return setup;
}

//------------------------------------------------------------------------------
int32
TextureSetup::GetIOLane() const {
    return this->ioLane;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromFile() const {
    return this->shouldSetupFromFile;
}

//------------------------------------------------------------------------------
bool
TextureSetup::ShouldSetupFromData() const {
    return this->shouldSetupFromData;
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
    return this->hasSharedDepth || (this->depthFormat != PixelFormat::InvalidPixelFormat);
}

//------------------------------------------------------------------------------
bool
TextureSetup::HasSharedDepth() const {
    return this->hasSharedDepth;
}

//------------------------------------------------------------------------------
const Resource::Locator&
TextureSetup::GetLocator() const {
    return this->locator;
}

//------------------------------------------------------------------------------
int32
TextureSetup::GetWidth() const {
    return this->width;
}

//------------------------------------------------------------------------------
int32
TextureSetup::GetHeight() const {
    return this->height;
}

//------------------------------------------------------------------------------
float32
TextureSetup::GetRelWidth() const {
    return this->relWidth;
}

//------------------------------------------------------------------------------
float32
TextureSetup::GetRelHeight() const {
    return this->relHeight;
}

//------------------------------------------------------------------------------
PixelFormat::Code
TextureSetup::GetColorFormat() const {
    return this->colorFormat;
}

//------------------------------------------------------------------------------
PixelFormat::Code
TextureSetup::GetDepthFormat() const {
    return this->depthFormat;
}

//------------------------------------------------------------------------------
const Id&
TextureSetup::GetDepthRenderTarget() const {
    return this->depthRenderTarget;
}

//------------------------------------------------------------------------------
void
TextureSetup::SetWrapU(TextureWrapMode::Code m) {
    this->wrapU = m;
}

//------------------------------------------------------------------------------
TextureWrapMode::Code
TextureSetup::GetWrapU() const {
    return this->wrapU;
}

//------------------------------------------------------------------------------
void
TextureSetup::SetWrapV(TextureWrapMode::Code m) {
    this->wrapV = m;
}

//------------------------------------------------------------------------------
TextureWrapMode::Code
TextureSetup::GetWrapV() const {
    return this->wrapV;
}

//------------------------------------------------------------------------------
void
TextureSetup::SetWrapW(TextureWrapMode::Code m) {
    this->wrapW = m;
}

//------------------------------------------------------------------------------
TextureWrapMode::Code
TextureSetup::GetWrapW() const {
    return this->wrapW;
}

//------------------------------------------------------------------------------
void
TextureSetup::SetMagFilter(TextureFilterMode::Code f) {
    o_assert((TextureFilterMode::Nearest == f) ||
             (TextureFilterMode::Linear == f));
    this->magFilter = f;
}

//------------------------------------------------------------------------------
TextureFilterMode::Code
TextureSetup::GetMagFilter() const {
    return this->magFilter;
}

//------------------------------------------------------------------------------
void
TextureSetup::SetMinFilter(TextureFilterMode::Code f) {
    this->minFilter = f;
}

//------------------------------------------------------------------------------
TextureFilterMode::Code
TextureSetup::GetMinFilter() const {
    return this->minFilter;
}

} // namespace Render
} // namespace Oryol