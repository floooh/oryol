//------------------------------------------------------------------------------
//  RenderSetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Core.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

//------------------------------------------------------------------------------
TEST(RenderSetupTest) {

    #if !ORYOL_UNITTESTS_HEADLESS
    Core::Setup();
    Gfx::Setup(GfxSetup::Window(400, 300, "Oryol Test"));
    
    CHECK(Gfx::GfxSetup().Width == 400);
    CHECK(Gfx::GfxSetup().Height == 300);
    CHECK(Gfx::GfxSetup().ColorFormat == PixelFormat::RGBA8);
    CHECK(Gfx::GfxSetup().DepthFormat == PixelFormat::DEPTHSTENCIL);
    CHECK(Gfx::GfxSetup().Title == "Oryol Test");
    CHECK(Gfx::GfxSetup().Windowed == true);
    
    CHECK(Gfx::DisplayAttrs().WindowWidth >= 400);
    CHECK(Gfx::DisplayAttrs().WindowHeight >= 300);
    CHECK(Gfx::DisplayAttrs().WindowPosX >= 0);
    CHECK(Gfx::DisplayAttrs().WindowPosY >= 0);
    CHECK(Gfx::DisplayAttrs().FramebufferWidth >= 400);
    CHECK(Gfx::DisplayAttrs().FramebufferHeight >= 300);
    CHECK(Gfx::DisplayAttrs().ColorPixelFormat == PixelFormat::RGBA8);
    CHECK(Gfx::DisplayAttrs().DepthPixelFormat == PixelFormat::DEPTHSTENCIL);
    CHECK(Gfx::DisplayAttrs().SwapInterval == 1);
    CHECK(Gfx::DisplayAttrs().WindowTitle == "Oryol Test");
    CHECK(Gfx::DisplayAttrs().Windowed == true);
    
    Gfx::Discard();
    Core::Discard();
    #endif
}

