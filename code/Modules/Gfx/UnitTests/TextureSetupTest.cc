//------------------------------------------------------------------------------
//  TextureSetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Gfx/GfxTypes.h"

using namespace Oryol;

TEST(TextureSetupTest) {
    
    // setup as absolute-size render target, no depth buffer
    auto rt = TextureSetup::RenderTarget2D(320, 256);
    rt.Locator = Locator("absSize");
    rt.ColorFormat = PixelFormat::RGB8;
    CHECK(!rt.ShouldSetupFromFile());
    CHECK(!rt.ShouldSetupFromPixelData());
    CHECK(rt.IsRenderTarget);
    CHECK(!rt.HasDepth());
    CHECK(rt.Locator.Location() == "absSize");
    CHECK(rt.Width == 320);
    CHECK(rt.Height == 256);
    CHECK(rt.ColorFormat == PixelFormat::RGB8);
    CHECK(rt.DepthFormat == PixelFormat::InvalidPixelFormat);
    CHECK(rt.Sampler.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt.Sampler.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt.Sampler.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt.Sampler.MinFilter == TextureFilterMode::Nearest);
    
    // setup as absolute-size render target, with depth buffer
    auto rt0 = TextureSetup::RenderTarget2D(320, 256);
    rt0.ColorFormat = PixelFormat::RGBA8;
    rt0.DepthFormat = PixelFormat::DEPTHSTENCIL;
    CHECK(!rt0.ShouldSetupFromFile());
    CHECK(!rt0.ShouldSetupFromPixelData());
    CHECK(rt0.IsRenderTarget);
    CHECK(rt0.HasDepth());
    CHECK(rt0.Locator == Locator::NonShared());
    CHECK(rt0.Width == 320);
    CHECK(rt0.Height == 256);
    CHECK(rt0.ColorFormat == PixelFormat::RGBA8);
    CHECK(rt0.DepthFormat == PixelFormat::DEPTHSTENCIL);
    CHECK(rt0.Sampler.WrapU == TextureWrapMode::ClampToEdge);
    CHECK(rt0.Sampler.WrapV == TextureWrapMode::ClampToEdge);
    CHECK(rt0.Sampler.MagFilter == TextureFilterMode::Nearest);
    CHECK(rt0.Sampler.MinFilter == TextureFilterMode::Nearest);
}

