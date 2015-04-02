//-----------------------------------------------------------------------------
//  tbMgr.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "tbMgr.h"
#include "Core/Core.h"
#include "animation/tb_widget_animation.h"

using namespace tb;

namespace Oryol {
namespace _priv {

//-----------------------------------------------------------------------------
tbMgr::tbMgr() {
    TBWidgetsAnimationManager::Init();
    this->runLoopId = Core::PreRunLoop()->Add([this] {
        this->update();
    });
}

//-----------------------------------------------------------------------------
tbMgr::~tbMgr() {
    Core::PreRunLoop()->Remove(this->runLoopId);
    TBWidgetsAnimationManager::Shutdown();
}

//-----------------------------------------------------------------------------
void
tbMgr::update() {
    TBAnimationManager::Update();
    this->rootWidget.InvokeProcessStates();
    this->rootWidget.InvokeProcess();
}

} // namespace _priv
} // namespace Oryol


