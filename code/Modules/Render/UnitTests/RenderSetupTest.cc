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
    Render::Setup(RenderSetup::AsWindow(400, 300, false, "Oryol Test"));
    
    CHECK(Render::GetRenderSetup().Width == 400);
    CHECK(Render::GetRenderSetup().Height == 300);
    CHECK(Render::GetRenderSetup().ColorFormat == PixelFormat::RGB8);
    CHECK(Render::GetRenderSetup().DepthFormat == PixelFormat::D24S8);
    CHECK(Render::GetRenderSetup().Title == "Oryol Test");
    CHECK(Render::GetRenderSetup().Fullscreen == false);
    
    CHECK(Render::GetDisplayAttrs().WindowWidth >= 400);
    CHECK(Render::GetDisplayAttrs().WindowHeight >= 300);
    CHECK(Render::GetDisplayAttrs().WindowPosX >= 0);
    CHECK(Render::GetDisplayAttrs().WindowPosY >= 0);
    CHECK(Render::GetDisplayAttrs().FramebufferWidth >= 400);
    CHECK(Render::GetDisplayAttrs().FramebufferHeight >= 300);
    CHECK(Render::GetDisplayAttrs().ColorPixelFormat == PixelFormat::RGB8);
    CHECK(Render::GetDisplayAttrs().DepthPixelFormat == PixelFormat::D24S8);
    CHECK(Render::GetDisplayAttrs().SwapInterval == 1);
    CHECK(Render::GetDisplayAttrs().WindowTitle == "Oryol Test");
    CHECK(Render::GetDisplayAttrs().IsFullscreen == false);
    
    Render::Discard();
    #endif
}

