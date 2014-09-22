//------------------------------------------------------------------------------
//  RenderSetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

//------------------------------------------------------------------------------
TEST(RenderSetupTest) {

    #if !ORYOL_UNITTESTS_HEADLESS
    Gfx::Setup(GfxSetup::Window(400, 300, "Oryol Test"));
    
    CHECK(Gfx::GfxSetup().Width == 400);
    CHECK(Gfx::GfxSetup().Height == 300);
    CHECK(Gfx::GfxSetup().ColorFormat == PixelFormat::RGB8);
    CHECK(Gfx::GfxSetup().DepthFormat == PixelFormat::D24S8);
    CHECK(Gfx::GfxSetup().Title == "Oryol Test");
    CHECK(Gfx::GfxSetup().Windowed == true);
    
    CHECK(Gfx::DisplayAttrs().WindowWidth >= 400);
    CHECK(Gfx::DisplayAttrs().WindowHeight >= 300);
    CHECK(Gfx::DisplayAttrs().WindowPosX >= 0);
    CHECK(Gfx::DisplayAttrs().WindowPosY >= 0);
    CHECK(Gfx::DisplayAttrs().FramebufferWidth >= 400);
    CHECK(Gfx::DisplayAttrs().FramebufferHeight >= 300);
    CHECK(Gfx::DisplayAttrs().ColorPixelFormat == PixelFormat::RGB8);
    CHECK(Gfx::DisplayAttrs().DepthPixelFormat == PixelFormat::D24S8);
    CHECK(Gfx::DisplayAttrs().SwapInterval == 1);
    CHECK(Gfx::DisplayAttrs().WindowTitle == "Oryol Test");
    CHECK(Gfx::DisplayAttrs().Windowed == true);
    
    Gfx::Discard();
    #endif
}

