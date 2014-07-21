//------------------------------------------------------------------------------
//  TextureSetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Setup/TextureSetup.h"

using namespace Oryol;
using namespace Oryol::Render;

TEST(TextureSetupTest) {
    
    // setup as absolute-size render target, no depth buffer
    const auto rt = TextureSetup::AsRenderTarget("absSize", 320, 256, PixelFormat::R8G8B8);
    CHECK(!rt.ShouldSetupFromFile());
    CHECK(!rt.ShouldSetupFromImageFileData());
    CHECK(!rt.ShouldSetupFromPixelData());
    CHECK(rt.ShouldSetupAsRenderTarget());
    CHECK(!rt.IsRelSizeRenderTarget());
    CHECK(!rt.HasSharedDepth());
    CHECK(!rt.HasDepth());
    CHECK(rt.Locator.Location() == "absSize");
    CHECK(rt.Width == 320);
    CHECK(rt.Height == 256);
    CHECK(rt.RelWidth == 0.0f);
    CHECK(rt.RelHeight == 0.0f);
    CHECK(rt.ColorFormat == PixelFormat::R8G8B8);
    CHECK(rt.DepthFormat == PixelFormat::InvalidPixelFormat);
    CHECK(!rt.DepthRenderTarget.IsValid());
    CHECK(rt.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt.WrapW == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt.MinFilter == TextureFilterMode::Nearest);
    
    // setup as absolute-size render target, with depth buffer
    const auto rt0 = TextureSetup::AsRenderTarget("absSize", 320, 256, PixelFormat::R8G8B8A8, PixelFormat::D24S8);
    CHECK(!rt0.ShouldSetupFromFile());
    CHECK(!rt0.ShouldSetupFromImageFileData());
    CHECK(!rt0.ShouldSetupFromPixelData());
    CHECK(rt0.ShouldSetupAsRenderTarget());
    CHECK(!rt0.IsRelSizeRenderTarget());
    CHECK(!rt0.HasSharedDepth());
    CHECK(rt0.HasDepth());
    CHECK(rt0.Locator.Location() == "absSize");
    CHECK(rt0.Width == 320);
    CHECK(rt0.Height == 256);
    CHECK(rt0.RelWidth == 0.0f);
    CHECK(rt0.RelHeight == 0.0f);
    CHECK(rt0.ColorFormat == PixelFormat::R8G8B8A8);
    CHECK(rt0.DepthFormat == PixelFormat::D24S8);
    CHECK(!rt0.DepthRenderTarget.IsValid());
    CHECK(rt0.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt0.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt0.WrapW == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt0.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt0.MinFilter == TextureFilterMode::Nearest);
    
    // setup as relative-size render target, no depth buffer
    const auto rt1 = TextureSetup::AsRelSizeRenderTarget("relSize", 0.5f, 0.25f, PixelFormat::R5G6B5);
    CHECK(!rt1.ShouldSetupFromFile());
    CHECK(!rt1.ShouldSetupFromImageFileData());
    CHECK(!rt1.ShouldSetupFromPixelData());
    CHECK(rt1.ShouldSetupAsRenderTarget());
    CHECK(rt1.IsRelSizeRenderTarget());
    CHECK(!rt1.HasSharedDepth());
    CHECK(!rt1.HasDepth());
    CHECK(rt1.Locator.Location() == "relSize");
    CHECK(rt1.Width == 0);
    CHECK(rt1.Height == 0);
    CHECK(rt1.RelWidth == 0.5f);
    CHECK(rt1.RelHeight == 0.25f);
    CHECK(rt1.ColorFormat == PixelFormat::R5G6B5);
    CHECK(rt1.DepthFormat == PixelFormat::InvalidPixelFormat);
    CHECK(!rt1.DepthRenderTarget.IsValid());
    CHECK(rt1.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt1.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt1.WrapW == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt1.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt1.MinFilter == TextureFilterMode::Nearest);
    
    // setup as relative-size render target, with depth buffer
    const auto rt2 = TextureSetup::AsRelSizeRenderTarget("relSize", 0.5f, 0.25f, PixelFormat::R4G4B4A4, PixelFormat::D16);
    CHECK(!rt2.ShouldSetupFromFile());
    CHECK(!rt2.ShouldSetupFromImageFileData());
    CHECK(!rt2.ShouldSetupFromPixelData());
    CHECK(rt2.ShouldSetupAsRenderTarget());
    CHECK(rt2.IsRelSizeRenderTarget());
    CHECK(!rt2.HasSharedDepth());
    CHECK(rt2.HasDepth());
    CHECK(rt2.Locator.Location() == "relSize");
    CHECK(rt2.Width == 0);
    CHECK(rt2.Height == 0);
    CHECK(rt2.RelWidth == 0.5f);
    CHECK(rt2.RelHeight == 0.25f);
    CHECK(rt2.ColorFormat == PixelFormat::R4G4B4A4);
    CHECK(rt2.DepthFormat == PixelFormat::D16);
    CHECK(!rt2.DepthRenderTarget.IsValid());
    CHECK(rt2.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt2.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt2.WrapW == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt2.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt2.MinFilter == TextureFilterMode::Nearest);
    
    // setup as shared-depth render target
    const auto rt3 = TextureSetup::AsSharedDepthRenderTarget("sharedDepth", PixelFormat::R32F, Resource::Id(1, 2, 3));
    CHECK(!rt3.ShouldSetupFromFile());
    CHECK(!rt3.ShouldSetupFromImageFileData());
    CHECK(!rt3.ShouldSetupFromPixelData());
    CHECK(rt3.ShouldSetupAsRenderTarget());
    CHECK(!rt3.IsRelSizeRenderTarget());
    CHECK(rt3.HasSharedDepth());
    CHECK(rt3.HasDepth());
    CHECK(rt3.Locator.Location() == "sharedDepth");
    CHECK(rt3.Width == 0);
    CHECK(rt3.Height == 0);
    CHECK(rt3.RelWidth == 0.0f);
    CHECK(rt3.RelHeight == 0.0f);
    CHECK(rt3.ColorFormat == PixelFormat::R32F);
    CHECK(rt3.DepthFormat == PixelFormat::InvalidPixelFormat);
    CHECK(rt3.DepthRenderTarget.SlotIndex() == 2);
    CHECK(rt3.DepthRenderTarget.Type() == 3);
    CHECK(rt3.DepthRenderTarget.UniqueStamp() == 1);
    CHECK(rt3.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt3.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt3.WrapW == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt3.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt3.MinFilter == TextureFilterMode::Nearest);
}

