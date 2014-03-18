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

    RenderFacade* f = RenderFacade::CreateSingleton();
    auto dispSetup = RenderSetup::Windowed(400, 300, "Oryol Test");
    f->Setup(dispSetup);
    CHECK(f->IsValid());
    
    CHECK(f->GetRenderSetup().GetWindowWidth() == 400);
    CHECK(f->GetRenderSetup().GetWindowHeight() == 300);
    CHECK(f->GetRenderSetup().GetWindowPosX() == 0);
    CHECK(f->GetRenderSetup().GetWindowPosY() == 0);
    CHECK(f->GetRenderSetup().GetColorPixelFormat() == PixelFormat::R8G8B8);
    CHECK(f->GetRenderSetup().GetDepthPixelFormat() == PixelFormat::D24S8);
    CHECK(f->GetRenderSetup().GetSwapInterval() == 1);
    CHECK(f->GetRenderSetup().GetWindowTitle() == "Oryol Test");
    CHECK(f->GetRenderSetup().IsFullscreen() == false);
    
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

