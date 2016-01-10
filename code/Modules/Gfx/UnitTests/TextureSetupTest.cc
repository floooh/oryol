//------------------------------------------------------------------------------
//  TextureSetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Gfx/Setup/TextureSetup.h"

using namespace Oryol;

TEST(TextureSetupTest) {
    
    // setup as absolute-size render target, no depth buffer
    auto rt = TextureSetup::RenderTarget(320, 256);
    rt.Locator = Locator("absSize");
    rt.ColorFormat = PixelFormat::RGB8;
    CHECK(!rt.ShouldSetupFromFile());
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
    CHECK(rt.ColorFormat == PixelFormat::RGB8);
    CHECK(rt.DepthFormat == PixelFormat::InvalidPixelFormat);
    CHECK(!rt.DepthRenderTarget.IsValid());
    CHECK(rt.Sampler.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt.Sampler.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt.Sampler.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt.Sampler.MinFilter == TextureFilterMode::Nearest);
    
    // setup as absolute-size render target, with depth buffer
    auto rt0 = TextureSetup::RenderTarget(320, 256);
    rt0.ColorFormat = PixelFormat::RGBA8;
    rt0.DepthFormat = PixelFormat::D24S8;
    CHECK(!rt0.ShouldSetupFromFile());
    CHECK(!rt0.ShouldSetupFromPixelData());
    CHECK(rt0.ShouldSetupAsRenderTarget());
    CHECK(!rt0.IsRelSizeRenderTarget());
    CHECK(!rt0.HasSharedDepth());
    CHECK(rt0.HasDepth());
    CHECK(rt0.Locator == Locator::NonShared());
    CHECK(rt0.Width == 320);
    CHECK(rt0.Height == 256);
    CHECK(rt0.RelWidth == 0.0f);
    CHECK(rt0.RelHeight == 0.0f);
    CHECK(rt0.ColorFormat == PixelFormat::RGBA8);
    CHECK(rt0.DepthFormat == PixelFormat::D24S8);
    CHECK(!rt0.DepthRenderTarget.IsValid());
    CHECK(rt0.Sampler.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt0.Sampler.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt0.Sampler.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt0.Sampler.MinFilter == TextureFilterMode::Nearest);
    
    // setup as relative-size render target, no depth buffer
    auto rt1 = TextureSetup::RelSizeRenderTarget(0.5f, 0.25f);
    rt1.ColorFormat = PixelFormat::R5G6B5;
    CHECK(!rt1.ShouldSetupFromFile());
    CHECK(!rt1.ShouldSetupFromPixelData());
    CHECK(rt1.ShouldSetupAsRenderTarget());
    CHECK(rt1.IsRelSizeRenderTarget());
    CHECK(!rt1.HasSharedDepth());
    CHECK(!rt1.HasDepth());
    CHECK(rt1.Locator == Locator::NonShared());
    CHECK(rt1.Width == 0);
    CHECK(rt1.Height == 0);
    CHECK(rt1.RelWidth == 0.5f);
    CHECK(rt1.RelHeight == 0.25f);
    CHECK(rt1.ColorFormat == PixelFormat::R5G6B5);
    CHECK(rt1.DepthFormat == PixelFormat::InvalidPixelFormat);
    CHECK(!rt1.DepthRenderTarget.IsValid());
    CHECK(rt1.Sampler.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt1.Sampler.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt1.Sampler.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt1.Sampler.MinFilter == TextureFilterMode::Nearest);
    
    // setup as relative-size render target, with depth buffer
    auto rt2 = TextureSetup::RelSizeRenderTarget(0.5f, 0.25f);
    rt2.ColorFormat = PixelFormat::RGBA4;
    rt2.DepthFormat = PixelFormat::D16;
    CHECK(!rt2.ShouldSetupFromFile());
    CHECK(!rt2.ShouldSetupFromPixelData());
    CHECK(rt2.ShouldSetupAsRenderTarget());
    CHECK(rt2.IsRelSizeRenderTarget());
    CHECK(!rt2.HasSharedDepth());
    CHECK(rt2.HasDepth());
    CHECK(rt2.Locator == Locator::NonShared());
    CHECK(rt2.Width == 0);
    CHECK(rt2.Height == 0);
    CHECK(rt2.RelWidth == 0.5f);
    CHECK(rt2.RelHeight == 0.25f);
    CHECK(rt2.ColorFormat == PixelFormat::RGBA4);
    CHECK(rt2.DepthFormat == PixelFormat::D16);
    CHECK(!rt2.DepthRenderTarget.IsValid());
    CHECK(rt2.Sampler.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt2.Sampler.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt2.Sampler.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt2.Sampler.MinFilter == TextureFilterMode::Nearest);
    
    // setup as shared-depth render target
    auto rt3 = TextureSetup::SharedDepthRenderTarget(Id(1, 2, GfxResourceType::Texture));
    rt3.ColorFormat = PixelFormat::RGBA32F;
    CHECK(!rt3.ShouldSetupFromFile());
    CHECK(!rt3.ShouldSetupFromPixelData());
    CHECK(rt3.ShouldSetupAsRenderTarget());
    CHECK(!rt3.IsRelSizeRenderTarget());
    CHECK(rt3.HasSharedDepth());
    CHECK(rt3.HasDepth());
    CHECK(rt3.Width == 0);
    CHECK(rt3.Height == 0);
    CHECK(rt3.RelWidth == 0.0f);
    CHECK(rt3.RelHeight == 0.0f);
    CHECK(rt3.ColorFormat == PixelFormat::RGBA32F);
    CHECK(rt3.DepthFormat == PixelFormat::InvalidPixelFormat);
    CHECK(rt3.DepthRenderTarget.SlotIndex == 2);
    CHECK(rt3.DepthRenderTarget.Type == GfxResourceType::Texture);
    CHECK(rt3.DepthRenderTarget.UniqueStamp == 1);
    CHECK(rt3.Sampler.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt3.Sampler.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt3.Sampler.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt3.Sampler.MinFilter == TextureFilterMode::Nearest);
}

