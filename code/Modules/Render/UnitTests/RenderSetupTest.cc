//------------------------------------------------------------------------------
//  RenderSetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Render.h"

using namespace Oryol;

//------------------------------------------------------------------------------
TEST(RenderSetupTest) {

    #if !ORYOL_UNITTESTS_HEADLESS
    Render::Setup(RenderSetup::Window(400, 300, false, "Oryol Test"));
    
    CHECK(Render::RenderSetup().Width == 400);
    CHECK(Render::RenderSetup().Height == 300);
    CHECK(Render::RenderSetup().ColorFormat == PixelFormat::RGB8);
    CHECK(Render::RenderSetup().DepthFormat == PixelFormat::D24S8);
    CHECK(Render::RenderSetup().Title == "Oryol Test");
    CHECK(Render::RenderSetup().Windowed == true);
    
    CHECK(Render::DisplayAttrs().WindowWidth >= 400);
    CHECK(Render::DisplayAttrs().WindowHeight >= 300);
    CHECK(Render::DisplayAttrs().WindowPosX >= 0);
    CHECK(Render::DisplayAttrs().WindowPosY >= 0);
    CHECK(Render::DisplayAttrs().FramebufferWidth >= 400);
    CHECK(Render::DisplayAttrs().FramebufferHeight >= 300);
    CHECK(Render::DisplayAttrs().ColorPixelFormat == PixelFormat::RGB8);
    CHECK(Render::DisplayAttrs().DepthPixelFormat == PixelFormat::D24S8);
    CHECK(Render::DisplayAttrs().SwapInterval == 1);
    CHECK(Render::DisplayAttrs().WindowTitle == "Oryol Test");
    CHECK(Render::DisplayAttrs().IsFullscreen == false);
    
    Render::Discard();
    #endif
}

