//------------------------------------------------------------------------------
//  TextureFactoryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Core/textureFactory.h"
#include "Render/Core/displayMgr.h"
#include "Render/Core/texturePool.h"
#include "Render/Core/stateWrapper.h"

#if ORYOL_OPENGL
#include "Render/gl/gl_impl.h"
#endif

using namespace Oryol;
using namespace Oryol::Render;

TEST(RenderTargetCreationTest) {

    #if !ORYOL_UNITTESTS_HEADLESS
    // setup a GL context
    auto renderSetup = RenderSetup::Windowed(400, 300, "Oryol Test");
    displayMgr displayManager;
    displayManager.SetupDisplay(renderSetup);
    
    // setup a meshFactory object
    texturePool texPool;
    stateWrapper stWrapper;
    textureFactory factory;
    factory.Setup(&stWrapper, &displayManager, &texPool);
    
    // create a render target (no depth buffer)
    texture tex0;
    tex0.setSetup(TextureSetup::AsRenderTarget("tex0", 320, 256, PixelFormat::R8G8B8A8));
    factory.SetupResource(tex0);
    CHECK(tex0.GetState() == Resource::State::Valid);
    CHECK(tex0.glGetTexture() != 0);
    CHECK(tex0.glGetFramebuffer() != 0);
    CHECK(tex0.glGetDepthRenderbuffer() == 0);
    CHECK(tex0.glGetDepthTexture() == 0);
    const TextureAttrs& attrs0 = tex0.GetTextureAttrs();
    CHECK(attrs0.GetLocator().Location() == "tex0");
    CHECK(attrs0.Type() == TextureType::Texture2D);
    CHECK(attrs0.ColorFormat() == PixelFormat::R8G8B8A8);
    CHECK(attrs0.DepthFormat() == PixelFormat::InvalidPixelFormat);
    CHECK(attrs0.GetUsage() == Usage::Immutable);
    CHECK(attrs0.Width() == 320);
    CHECK(attrs0.Height() == 256);
    CHECK(attrs0.Depth() == 0);
    CHECK(!attrs0.HasMipmaps());
    CHECK(attrs0.IsRenderTarget());
    CHECK(!attrs0.HasDepthBuffer());
    CHECK(!attrs0.HasSharedDepthBuffer());
    CHECK(!attrs0.IsDepthTexture());
    
    // create a render target with depth buffer
    texture tex1;
    tex1.setSetup(TextureSetup::AsRenderTarget("tex1", 640, 480, PixelFormat::R8G8B8A8, PixelFormat::D24S8));
    factory.SetupResource(tex1);
    CHECK(tex1.GetState() == Resource::State::Valid);
    CHECK(tex1.glGetTexture() != 0);
    CHECK(tex1.glGetFramebuffer() != 0);
    CHECK(tex1.glGetDepthRenderbuffer() != 0);
    CHECK(tex1.glGetDepthTexture() == 0);
    const TextureAttrs& attrs1 = tex1.GetTextureAttrs();
    CHECK(attrs1.GetLocator().Location() == "tex1");
    CHECK(attrs1.Type() == TextureType::Texture2D);
    CHECK(attrs1.ColorFormat() == PixelFormat::R8G8B8A8);
    CHECK(attrs1.DepthFormat() == PixelFormat::D24S8);
    CHECK(attrs1.GetUsage() == Usage::Immutable);
    CHECK(attrs1.Width() == 640);
    CHECK(attrs1.Height() == 480);
    CHECK(attrs1.Depth() == 0);
    CHECK(!attrs1.HasMipmaps());
    CHECK(attrs1.IsRenderTarget());
    CHECK(attrs1.HasDepthBuffer());
    CHECK(!attrs1.HasSharedDepthBuffer());
    CHECK(!attrs1.IsDepthTexture());
    
    // create relative-size render target with depth buffer
    texture tex2;
    tex2.setSetup(TextureSetup::AsRelSizeRenderTarget("tex2", 1.0f, 1.0f, PixelFormat::R5G6B5, PixelFormat::D16));
    factory.SetupResource(tex2);
    CHECK(tex2.GetState() == Resource::State::Valid);
    CHECK(tex2.glGetTexture() != 0);
    CHECK(tex2.glGetFramebuffer() != 0);
    CHECK(tex2.glGetDepthRenderbuffer() != 0);
    CHECK(tex2.glGetDepthTexture() == 0);
    const TextureAttrs& attrs2 = tex2.GetTextureAttrs();
    CHECK(attrs2.GetLocator().Location() == "tex2");
    CHECK(attrs2.Type() == TextureType::Texture2D);
    CHECK(attrs2.ColorFormat() == PixelFormat::R5G6B5);
    CHECK(attrs2.DepthFormat() == PixelFormat::D16);
    CHECK(attrs2.GetUsage() == Usage::Immutable);
    CHECK(attrs2.Width() == 400);
    CHECK(attrs2.Height() == 300);
    CHECK(attrs2.Depth() == 0);
    CHECK(!attrs2.HasMipmaps());
    CHECK(attrs2.IsRenderTarget());
    CHECK(attrs2.HasDepthBuffer());
    CHECK(!attrs2.HasSharedDepthBuffer());
    CHECK(!attrs2.IsDepthTexture());
    
    // cleanup
    factory.DestroyResource(tex1);
    CHECK(tex1.GetState() == Resource::State::Setup);
    CHECK(tex1.glGetTexture() == 0);
    CHECK(tex1.glGetFramebuffer() == 0);
    CHECK(tex1.glGetDepthRenderbuffer() == 0);
    CHECK(tex1.glGetDepthTexture() == 0);
    
    factory.DestroyResource(tex0);
    CHECK(tex0.GetState() == Resource::State::Setup);
    factory.DestroyResource(tex2);
    CHECK(tex2.GetState() == Resource::State::Setup);
    factory.Discard();
    displayManager.DiscardDisplay();
    #endif
}

