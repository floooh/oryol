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
TextureSetup::FromPixelData2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup(blueprint);
    setup.setupFromPixelData = true;
    setup.Type = TextureType::Texture2D;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.ImageData.NumFaces = 1;
    setup.ImageData.NumMipMaps = numMipMaps;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromPixelDataCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup(blueprint);
    setup.setupFromPixelData = true;
    setup.Type = TextureType::TextureCube;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.ImageData.NumFaces = 6;
    setup.ImageData.NumMipMaps = numMipMaps;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromPixelData3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0) && (d > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup(blueprint);
    setup.setupFromPixelData = true;
    setup.Type = TextureType::Texture3D;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = d;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.ImageData.NumFaces = 1;
    setup.ImageData.NumMipMaps = numMipMaps;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::FromPixelDataArray(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0) && (d > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup(blueprint);
    setup.setupFromPixelData = true;
    setup.Type = TextureType::TextureArray;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = d;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.ImageData.NumFaces = 1;
    setup.ImageData.NumMipMaps = numMipMaps;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::Empty2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup(blueprint);
    setup.setupEmpty = true;
    setup.Type = TextureType::Texture2D;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::EmptyCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup(blueprint);
    setup.setupEmpty = true;
    setup.Type = TextureType::TextureCube;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::Empty3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0) && (d > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup(blueprint);
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
TextureSetup::EmptyArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0) && (layers > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));

    TextureSetup setup(blueprint);
    setup.setupEmpty = true;
    setup.Type = TextureType::TextureArray;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = layers;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::RenderTarget2D(int w, int h, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert_dbg((w > 0) && (h > 0));

    TextureSetup setup;
    setup.Type = TextureType::Texture2D;
    setup.RenderTarget = true;
    setup.Width = w;
    setup.Height = h;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::RenderTargetCube(int w, int h, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert_dbg((w > 0) && (h > 0));

    TextureSetup setup;
    setup.Type = TextureType::TextureCube;
    setup.RenderTarget = true;
    setup.Width = w;
    setup.Height = h;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::RenderTarget3D(int w, int h, int d, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert_dbg((w > 0) && (h > 0));

    TextureSetup setup;
    setup.Type = TextureType::Texture3D;
    setup.RenderTarget = true;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = d;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup
TextureSetup::RenderTargetArray(int w, int h, int layers, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert_dbg((w > 0) && (h > 0));

    TextureSetup setup;
    setup.Type = TextureType::TextureArray;
    setup.RenderTarget = true;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = layers;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
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
TextureSetup::HasDepth() const {
    return this->DepthFormat != PixelFormat::InvalidPixelFormat;
}

} // namespace Oryol
