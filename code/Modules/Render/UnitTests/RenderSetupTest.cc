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
    RenderFacade* f = RenderFacade::CreateSingle(RenderSetup::Windowed(400, 300, "Oryol Test"));
    
    CHECK(f->GetRenderSetup().WindowWidth == 400);
    CHECK(f->GetRenderSetup().WindowHeight == 300);
    CHECK(f->GetRenderSetup().WindowPosX == 0);
    CHECK(f->GetRenderSetup().WindowPosY == 0);
    CHECK(f->GetRenderSetup().ColorPixelFormat == PixelFormat::R8G8B8);
    CHECK(f->GetRenderSetup().DepthPixelFormat == PixelFormat::D24S8);
    CHECK(f->GetRenderSetup().SwapInterval == 1);
    CHECK(f->GetRenderSetup().WindowTitle == "Oryol Test");
    CHECK(f->GetRenderSetup().IsFullscreen == false);
    
    CHECK(f->GetDisplayAttrs().WindowWidth >= 400);
    CHECK(f->GetDisplayAttrs().WindowHeight >= 300);
    CHECK(f->GetDisplayAttrs().WindowPosX >= 0);
    CHECK(f->GetDisplayAttrs().WindowPosY >= 0);
    CHECK(f->GetDisplayAttrs().FramebufferWidth >= 400);
    CHECK(f->GetDisplayAttrs().FramebufferHeight >= 300);
    CHECK(f->GetDisplayAttrs().ColorPixelFormat == PixelFormat::R8G8B8);
    CHECK(f->GetDisplayAttrs().DepthPixelFormat == PixelFormat::D24S8);
    CHECK(f->GetDisplayAttrs().SwapInterval == 1);
    CHECK(f->GetDisplayAttrs().WindowTitle == "Oryol Test");
    CHECK(f->GetDisplayAttrs().IsFullscreen == false);
    
    RenderFacade::DestroySingle();
    #endif
}

