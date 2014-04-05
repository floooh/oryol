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
    auto rt = TextureSetup::AsRenderTarget("absSize", 320, 256, PixelFormat::R8G8B8);
    CHECK(!rt.ShouldSetupFromFile());
    CHECK(!rt.ShouldSetupFromData());
    CHECK(rt.ShouldSetupAsRenderTarget());
    CHECK(!rt.IsRelSizeRenderTarget());
    CHECK(!rt.HasSharedDepth());
    CHECK(!rt.HasDepth());
    CHECK(rt.GetLocator().Location() == "absSize");
    CHECK(rt.GetWidth() == 320);
    CHECK(rt.GetHeight() == 256);
    CHECK(rt.GetRelWidth() == 0.0f);
    CHECK(rt.GetRelHeight() == 0.0f);
    CHECK(rt.GetColorFormat() == PixelFormat::R8G8B8);
    CHECK(rt.GetDepthFormat() == PixelFormat::InvalidPixelFormat);
    CHECK(!rt.GetDepthRenderTarget().IsValid());
    CHECK(rt.GetWrapU() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapV() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapW() == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt.GetMagFilter() == TextureFilterMode::Nearest);
    CHECK(rt.GetMinFilter() == TextureFilterMode::Nearest);
    
    // setup as absolute-size render target, with depth buffer
    rt = TextureSetup::AsRenderTarget("absSize", 320, 256, PixelFormat::R8G8B8A8, PixelFormat::D24S8);
    CHECK(!rt.ShouldSetupFromFile());
    CHECK(!rt.ShouldSetupFromData());
    CHECK(rt.ShouldSetupAsRenderTarget());
    CHECK(!rt.IsRelSizeRenderTarget());
    CHECK(!rt.HasSharedDepth());
    CHECK(rt.HasDepth());
    CHECK(rt.GetLocator().Location() == "absSize");
    CHECK(rt.GetWidth() == 320);
    CHECK(rt.GetHeight() == 256);
    CHECK(rt.GetRelWidth() == 0.0f);
    CHECK(rt.GetRelHeight() == 0.0f);
    CHECK(rt.GetColorFormat() == PixelFormat::R8G8B8A8);
    CHECK(rt.GetDepthFormat() == PixelFormat::D24S8);
    CHECK(!rt.GetDepthRenderTarget().IsValid());
    CHECK(rt.GetWrapU() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapV() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapW() == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt.GetMagFilter() == TextureFilterMode::Nearest);
    CHECK(rt.GetMinFilter() == TextureFilterMode::Nearest);
    
    // setup as relative-size render target, no depth buffer
    rt = TextureSetup::AsRelSizeRenderTarget("relSize", 0.5f, 0.25f, PixelFormat::R5G6B5);
    CHECK(!rt.ShouldSetupFromFile());
    CHECK(!rt.ShouldSetupFromData());
    CHECK(rt.ShouldSetupAsRenderTarget());
    CHECK(rt.IsRelSizeRenderTarget());
    CHECK(!rt.HasSharedDepth());
    CHECK(!rt.HasDepth());
    CHECK(rt.GetLocator().Location() == "relSize");
    CHECK(rt.GetWidth() == 0);
    CHECK(rt.GetHeight() == 0);
    CHECK(rt.GetRelWidth() == 0.5f);
    CHECK(rt.GetRelHeight() == 0.25f);
    CHECK(rt.GetColorFormat() == PixelFormat::R5G6B5);
    CHECK(rt.GetDepthFormat() == PixelFormat::InvalidPixelFormat);
    CHECK(!rt.GetDepthRenderTarget().IsValid());
    CHECK(rt.GetWrapU() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapV() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapW() == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt.GetMagFilter() == TextureFilterMode::Nearest);
    CHECK(rt.GetMinFilter() == TextureFilterMode::Nearest);
    
    // setup as relative-size render target, with depth buffer
    rt = TextureSetup::AsRelSizeRenderTarget("relSize", 0.5f, 0.25f, PixelFormat::R4G4B4A4, PixelFormat::D16);
    CHECK(!rt.ShouldSetupFromFile());
    CHECK(!rt.ShouldSetupFromData());
    CHECK(rt.ShouldSetupAsRenderTarget());
    CHECK(rt.IsRelSizeRenderTarget());
    CHECK(!rt.HasSharedDepth());
    CHECK(rt.HasDepth());
    CHECK(rt.GetLocator().Location() == "relSize");
    CHECK(rt.GetWidth() == 0);
    CHECK(rt.GetHeight() == 0);
    CHECK(rt.GetRelWidth() == 0.5f);
    CHECK(rt.GetRelHeight() == 0.25f);
    CHECK(rt.GetColorFormat() == PixelFormat::R4G4B4A4);
    CHECK(rt.GetDepthFormat() == PixelFormat::D16);
    CHECK(!rt.GetDepthRenderTarget().IsValid());
    CHECK(rt.GetWrapU() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapV() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapW() == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt.GetMagFilter() == TextureFilterMode::Nearest);
    CHECK(rt.GetMinFilter() == TextureFilterMode::Nearest);
    
    // setup as shared-depth render target
    rt = TextureSetup::AsSharedDepthRenderTarget("sharedDepth", PixelFormat::R32F, Resource::Id(1, 2, 3));
    CHECK(!rt.ShouldSetupFromFile());
    CHECK(!rt.ShouldSetupFromData());
    CHECK(rt.ShouldSetupAsRenderTarget());
    CHECK(!rt.IsRelSizeRenderTarget());
    CHECK(rt.HasSharedDepth());
    CHECK(rt.HasDepth());
    CHECK(rt.GetLocator().Location() == "sharedDepth");
    CHECK(rt.GetWidth() == 0);
    CHECK(rt.GetHeight() == 0);
    CHECK(rt.GetRelWidth() == 0.0f);
    CHECK(rt.GetRelHeight() == 0.0f);
    CHECK(rt.GetColorFormat() == PixelFormat::R32F);
    CHECK(rt.GetDepthFormat() == PixelFormat::InvalidPixelFormat);
    CHECK(rt.GetDepthRenderTarget().SlotIndex() == 2);
    CHECK(rt.GetDepthRenderTarget().Type() == 3);
    CHECK(rt.GetDepthRenderTarget().UniqueStamp() == 1);
    CHECK(rt.GetWrapU() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapV() == TextureWrapMode::ClampToEdge);
    CHECK(rt.GetWrapW() == TextureWrapMode::InvalidTextureWrapMode);
    CHECK(rt.GetMagFilter() == TextureFilterMode::Nearest);
    CHECK(rt.GetMinFilter() == TextureFilterMode::Nearest);
}

