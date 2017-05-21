//------------------------------------------------------------------------------
//  TextureFactoryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Gfx/private/gfxFactory.h"
#include "Gfx/private/resourcePools.h"
#include "Gfx/private/displayMgr.h"
#include "Gfx/private/renderer.h"

#if ORYOL_OPENGL
#include "Gfx/private/gl/gl_impl.h"
#endif

using namespace Oryol;
using namespace _priv;

TEST(RenderTargetCreationTest) {

    #if !ORYOL_UNITTESTS_HEADLESS
    // setup a GL context
    auto gfxSetup = GfxSetup::Window(400, 300, "Oryol Test");
    displayMgr displayManager;
    class renderer renderer;
    texturePool texPool;
    meshPool meshPool;

    gfxPointers ptrs;
    ptrs.displayMgr = &displayManager;
    ptrs.renderer = &renderer;
    ptrs.texturePool = &texPool;
    ptrs.meshPool = &meshPool;

    displayManager.SetupDisplay(gfxSetup, ptrs);
    
    // setup a meshFactory object
    renderer.setup(gfxSetup, ptrs);
    gfxFactory factory;
    factory.setup(ptrs);
    
    // create a render target (no depth buffer)
    auto texSetup = TextureSetup::RenderTarget2D(320, 256);
    texSetup.ColorFormat = PixelFormat::RGBA8;
    texture tex0;
    tex0.Setup = texSetup;
    factory.initTexture(tex0, nullptr, 0);
    CHECK(tex0.glTextures[0] != 0);
    CHECK(tex0.glDepthRenderbuffer == 0);
    const TextureAttrs& attrs0 = tex0.textureAttrs;
    CHECK(attrs0.Locator == Locator::NonShared());
    CHECK(attrs0.Type == TextureType::Texture2D);
    CHECK(attrs0.ColorFormat == PixelFormat::RGBA8);
    CHECK(attrs0.DepthFormat == PixelFormat::InvalidPixelFormat);
    CHECK(attrs0.TextureUsage == Usage::Immutable);
    CHECK(attrs0.Width == 320);
    CHECK(attrs0.Height == 256);
    CHECK(attrs0.Depth == 1);
    CHECK(1 == attrs0.NumMipMaps);
    CHECK(attrs0.IsRenderTarget);
    CHECK(!attrs0.HasDepthBuffer);

    // create a render target with depth buffer
    auto rtSetup = TextureSetup::RenderTarget2D(640, 480);
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.DepthFormat = PixelFormat::DEPTHSTENCIL;
    texture tex1;
    tex1.Setup = rtSetup;
    factory.initTexture(tex1, nullptr, 0);
    CHECK(tex1.glTextures[0] != 0);
    CHECK(tex1.glDepthRenderbuffer != 0);
    const TextureAttrs& attrs1 = tex1.textureAttrs;
    CHECK(attrs1.Locator == Locator::NonShared());
    CHECK(attrs1.Type == TextureType::Texture2D);
    CHECK(attrs1.ColorFormat == PixelFormat::RGBA8);
    CHECK(attrs1.DepthFormat == PixelFormat::DEPTHSTENCIL);
    CHECK(attrs1.TextureUsage == Usage::Immutable);
    CHECK(attrs1.Width == 640);
    CHECK(attrs1.Height == 480);
    CHECK(attrs1.Depth == 1);
    CHECK(1 == attrs1.NumMipMaps);
    CHECK(attrs1.IsRenderTarget);
    CHECK(attrs1.HasDepthBuffer);

    // cleanup
    factory.destroyTexture(tex1);
    CHECK(tex1.glTextures[0] == 0);
    CHECK(tex1.glDepthRenderbuffer == 0);
    
    factory.destroyTexture(tex0);
    factory.discard();
    renderer.discard();
    displayManager.DiscardDisplay();
    #endif
}

