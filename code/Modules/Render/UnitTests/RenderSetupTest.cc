//------------------------------------------------------------------------------
//  RenderSetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/RenderFacade.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

//------------------------------------------------------------------------------
TEST(RenderSetupTest) {

    #if !ORYOL_UNITTESTS_HEADLESS
    RenderFacade* f = RenderFacade::CreateSingle(RenderSetup::AsWindow(400, 300, false, "Oryol Test"));
    
    CHECK(f->GetRenderSetup().Width == 400);
    CHECK(f->GetRenderSetup().Height == 300);
    CHECK(f->GetRenderSetup().ColorFormat == PixelFormat::RGB8);
    CHECK(f->GetRenderSetup().DepthFormat == PixelFormat::D24S8);
    CHECK(f->GetRenderSetup().Title == "Oryol Test");
    CHECK(f->GetRenderSetup().Fullscreen == false);
    
    CHECK(f->GetDisplayAttrs().WindowWidth >= 400);
    CHECK(f->GetDisplayAttrs().WindowHeight >= 300);
    CHECK(f->GetDisplayAttrs().WindowPosX >= 0);
    CHECK(f->GetDisplayAttrs().WindowPosY >= 0);
    CHECK(f->GetDisplayAttrs().FramebufferWidth >= 400);
    CHECK(f->GetDisplayAttrs().FramebufferHeight >= 300);
    CHECK(f->GetDisplayAttrs().ColorPixelFormat == PixelFormat::RGB8);
    CHECK(f->GetDisplayAttrs().DepthPixelFormat == PixelFormat::D24S8);
    CHECK(f->GetDisplayAttrs().SwapInterval == 1);
    CHECK(f->GetDisplayAttrs().WindowTitle == "Oryol Test");
    CHECK(f->GetDisplayAttrs().IsFullscreen == false);
    
    RenderFacade::DestroySingle();
    #endif
}

