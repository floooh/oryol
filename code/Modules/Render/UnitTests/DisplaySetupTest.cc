//------------------------------------------------------------------------------
//  DisplaySetupTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/RenderFacade.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

//------------------------------------------------------------------------------
TEST(DisplaySetuptest) {

    RenderFacade* f = RenderFacade::CreateSingleton();
    
    auto dispSetup = DisplaySetup::Windowed(400, 300, "Oryol Test");
    f->Setup(dispSetup);
    CHECK(f->IsValid());
    CHECK(f->GetDisplaySetup().GetWindowWidth() == 400);
    CHECK(f->GetDisplaySetup().GetWindowHeight() == 300);
    CHECK(f->GetDisplaySetup().GetWindowPosX() == 0);
    CHECK(f->GetDisplaySetup().GetWindowPosY() == 0);
    CHECK(f->GetDisplaySetup().GetColorPixelFormat() == PixelFormat::R8G8B8);
    CHECK(f->GetDisplaySetup().GetDepthPixelFormat() == PixelFormat::D24S8);
    CHECK(f->GetDisplaySetup().GetSwapInterval() == 1);
    CHECK(f->GetDisplaySetup().GetWindowTitle() == "Oryol Test");
    CHECK(f->GetDisplaySetup().IsFullscreen() == false);
    CHECK(f->GetDisplayAttrs().GetWindowWidth() >= 400);
    CHECK(f->GetDisplayAttrs().GetWindowHeight() >= 300);
    CHECK(f->GetDisplayAttrs().GetWindowPosX() >= 0);
    CHECK(f->GetDisplayAttrs().GetWindowPosY() >= 0);
    CHECK(f->GetDisplayAttrs().GetFramebufferWidth() >= 400);
    CHECK(f->GetDisplayAttrs().GetFramebufferHeight() >= 300);
    CHECK(f->GetDisplayAttrs().GetColorPixelFormat() == PixelFormat::R8G8B8);
    CHECK(f->GetDisplayAttrs().GetDepthPixelFormat() == PixelFormat::D24S8);
    CHECK(f->GetDisplayAttrs().GetSwapInterval() == 1);
    CHECK(f->GetDisplayAttrs().GetWindowTitle() == "Oryol Test");
    CHECK(f->GetDisplayAttrs().IsFullscreen() == false);
    f->Discard();
    
    RenderFacade::DestroySingleton();
}

