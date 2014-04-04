#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Application::App
    @brief Oryol's App main class
    @todo describe App class
*/
#include "Core/AppBase.h"
#include "Application/AppState.h"

namespace Oryol {
namespace Application {

class App : public Core::AppBase {
public:
    /// constructor
    App();
    /// destructor
    ~App();

    /// on construction callback
    virtual AppState::Code OnConstruct();
    /// on enqueue preload files frame method
    virtual AppState::Code OnEnqueuePreload();
    /// on preloading frame method
    virtual AppState::Code OnPreloading();
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();
    /// on destroy frame method
    virtual AppState::Code OnDestroy();

protected:
    /// per-frame dispatch method, which calls the on-state callbacks
    virtual void onFrame();

    AppState::Code curState;
};
    
} // namespace Application
} // namespace Oryol