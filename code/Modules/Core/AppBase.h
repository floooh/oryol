#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::AppBase
    @brief Oryol's lowlevel application wrapper class
    
    The AppBase header provides the lowest-level application wrapper
    class, which basically only provides the StartMainLoop() method,
    and a macro OryolMain() which wraps the application entry 
    procedure on different target platforms (not all platforms have a
    main, for instance).

    Regardless of target platform, an Oryol platform's main source
    will always look like this:
 
    ```cpp
    #include "Core/AppBase.h"
    
    OryolApp("MyAppName", "1.0");
 
    OryolMain(const Args& args) {
        AppBase app;
        app.SetArgs(args);
        app.StartMainLoop();
    }
    
    ```
    ...where AppBase is usually replaced with a user-provided 
    derived application class.
*/
#include "Core/Types.h"
#include "Core/Macros.h"
#include "Core/Args.h"
#include "Core/String/WideString.h"
#if ORYOL_WINDOWS
#define VC_EXTRALEAN (1)
#define WIN32_LEAN_AND_MEAN (1)
#include <Windows.h>
#endif

namespace Oryol {
namespace Core {

class CoreFacade;

class AppBase {
public:
    /// constructor
    AppBase();
    /// destructor
    ~AppBase();
    
    /// start the main loop, returns when QuitRequested is set
    void StartMainLoop();

protected:
    /// static frame function
    static void staticOnFrame();
    /// virtual onFrame method to be overwritten by subclass
    virtual void onFrame();
    /// set quit-requested flag
    void setQuitRequested();
    /// return true if quit requested flag is set
    bool isQuitRequested() const;
    
    static AppBase* self;
    CoreFacade* coreFacade;
    bool quitRequested;
};

#if ORYOL_WINDOWS
#define OryolApp(name, ver) \
void OryolMain(); \
const char* const OryolAppName = name; \
const char* const OryolAppVersion = ver; \
Oryol::Core::Args OryolArgs; \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd) {\
    Oryol::Core::WideString cmdLine = ::GetCommandLineW(); \
    OryolArgs = Oryol::Core::Args(cmdLine); \
    OryolMain(); \
    return 0; \
}
#else
#define OryolApp(name, ver) \
void OryolMain(); \
const char* const OryolAppName = name; \
const char* const OryolAppVersion = ver; \
Oryol::Core::Args OryolArgs; \
int main(int argc, const char** argv) { \
    OryolArgs = Oryol::Core::Args(argc, argv); \
    OryolMain(); \
    return 0; \
}
#endif

} // namespace Core
} // namespace Oryol
