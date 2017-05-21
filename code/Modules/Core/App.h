#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::App
    @ingroup Core
    @brief Oryol's lowlevel application wrapper class

    This is Oryol's base application class which implements the
    application's life-cycle. 

    Oryol doesn't provide a main function, instead an Oryol app must derive
    from the Oryol::App class and override application lifecycle methods.

    Regardless of target platform, an Oryol platform's main source
    will always look like this:
 
    ```cpp
    #include "Core/Main.h"
    
    class MyAppClass : public Oryol::App {
    public:
        virtual AppState::Code OnInit() {
            ...
        }
        virtual AppState::Code OnRunning() {
            ...
        }
        virtual AppState::Code OnCleanup() {
            ...
        }
    };
    OryolMain(MyAppClass);    
    ```
*/
#include "Core/Args.h"
#include "Core/AppState.h"
#include "Core/Containers/Set.h"

namespace Oryol {
namespace _priv {
#if ORYOL_ANDROID
class androidBridge;
#elif ORYOL_IOS
class iosBridge;
#elif ORYOL_MACOS && ORYOL_METAL
class osxBridge;
#endif
}

class App {
public:
    /// constructor
    App();
    /// destructor
    virtual ~App();
    
    /// start the main loop, returns when QuitRequested is set
    void StartMainLoop();

    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();
    /// on destroy frame method
    virtual AppState::Code OnDestroy();

    /// add a blocker which prevents entering this state
    void addBlocker(AppState::Code blockedState);
    /// remove a blocker, state can be entered if needed
    void remBlocker(AppState::Code blockedState);

    /// static frame function
    static void staticOnFrame();
    /// virtual onFrame method to be overwritten by subclass
    virtual void onFrame();
    /// low-level ready for init notifier
    void readyForInit();
    /// low-level request app to quit notifier
    void requestQuit();
    /// low-level request app to suspend notifier
    void requestSuspend();

protected:    
    static App* self;
    AppState::Code curState;
    AppState::Code nextState;
    Set<AppState::Code> blockers;
    bool quitRequested;
    bool suspendRequested;
    #if ORYOL_IOS
    _priv::iosBridge* iosBridge;
    #elif ORYOL_MACOS && ORYOL_METAL
    _priv::osxBridge* osxBridge;
    #elif ORYOL_ANDROID
    _priv::androidBridge* androidBridge;
    #endif
};

} // namespace Oryol
