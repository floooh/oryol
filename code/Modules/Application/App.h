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

typedef std::function<AppState::Code(void)> AppCallback;
    
class App : public Core::AppBase {
public:
    /// constructor
    App();
    /// destructor
    ~App();
    /// add a callback
    void AddCallback(AppState::Code state, AppCallback callback);

protected:
    /// per-frame dispatch method, which calls the on-state callbacks
    void dispatch();

    AppState::Code curState;
    AppCallback callbacks[AppState::NumAppStates];    
};
    
} // namespace Application
} // namespace Oryol