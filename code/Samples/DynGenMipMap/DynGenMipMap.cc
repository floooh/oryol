//------------------------------------------------------------------------------
//  DynGenMipMap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

class DynGenMipMap : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();
};
OryolMain(DynGenMipMap);

//------------------------------------------------------------------------------
AppState::Code
DynGenMipMap::OnInit() {
    Gfx::Setup(GfxSetup::Window(512, 512, "DynGenMipMap"));
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DynGenMipMap::OnRunning() {
    Gfx::BeginPass(PassState(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)));
    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DynGenMipMap::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
