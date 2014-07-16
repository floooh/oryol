#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::App
    @brief Oryol's lowlevel application wrapper class

    This is Oryol's base application class which implements the
    application's life-cycle. 

    Oryol doesn't provide a main function, instead an Oryol app must derive
    from the Oryol::Core::App class and override application lifecycle methods.

    Regardless of target platform, an Oryol platform's main source
    will always look like this:
 
    ```cpp
    #include "Core/App.h"
    
    class MyAppClass : public Oryol::Core::App {
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
#include "Core/Types.h"
#include "Core/Args.h"
#include "Core/AppState.h"
#include "Core/String/WideString.h"
#include "Core/Containers/Set.h"
#if ORYOL_WINDOWS
#define VC_EXTRALEAN (1)
#define WIN32_LEAN_AND_MEAN (1)
#include <Windows.h>
#endif
#if ORYOL_ANDROID
#include "android_native/android_native_app_glue.h"
#include "Core/android/androidBridge.h"
#endif
#if ORYOL_IOS
#include "Core/ios/iosBridge.h"
#endif
#if ORYOL_PNACL
#include "Core/pnacl/pnaclModule.h"
#endif

namespace Oryol {
namespace Core {

class CoreFacade;

class App {
public:
    /// constructor
    App();
    /// destructor
    virtual ~App();
    
    /// start the main loop, returns when QuitRequested is set
    void StartMainLoop();

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
    CoreFacade* coreFacade;
    AppState::Code curState;
    AppState::Code nextState;
    Set<AppState::Code> blockers;
    bool quitRequested;
    bool suspendRequested;
    #if ORYOL_IOS
    class iosBridge* iosBridge;
    #endif
    #if ORYOL_ANDROID
    class androidBridge androidBridge;
    #endif
};

#if ORYOL_WINDOWS
#define OryolMain(clazz) \
Oryol::Core::Args OryolArgs; \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd) {\
    Oryol::Core::WideString cmdLine = ::GetCommandLineW(); \
    OryolArgs = Oryol::Core::Args(cmdLine); \
    clazz app; \
    app.StartMainLoop(); \
    return 0; \
}
#elif ORYOL_ANDROID
#define OryolMain(clazz) \
android_app* OryolAndroidAppState = nullptr; \
Oryol::Core::Args OryolArgs; \
void android_main(struct android_app* app_) { \
    app_dummy(); \
    OryolAndroidAppState = app_; \
    clazz* app = new clazz(); \
    app->StartMainLoop(); \
    delete(app); \
}
#elif ORYOL_PNACL
#define OryolMain(clazz) \
namespace pp \
{ \
    Module* CreateModule() \
    { \
        return new Oryol::Core::pnaclModule(); \
    }; \
} \
void PNaclAppCreator() {\
    static clazz* app = new clazz(); \
    app->StartMainLoop(); \
}
#else
#define OryolMain(clazz) \
Oryol::Core::Args OryolArgs; \
int main(int argc, const char** argv) { \
    OryolArgs = Oryol::Core::Args(argc, argv); \
    clazz* app = new clazz(); \
    app->StartMainLoop(); \
    delete(app); \
    return 0; \
}
#endif

} // namespace Core
} // namespace Oryol
