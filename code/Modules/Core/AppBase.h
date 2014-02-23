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

namespace Oryol {
namespace Core {

class AppBase {
    OryolLocalSingletonDecl(AppBase);
public:
    /// constructor
    AppBase();
    /// destructor
    ~AppBase();
    
    /// start the main loop, returns when QuitRequested is set
    void StartMainLoop();
    /// set the quit-requested flag
    void SetQuitRequested();
    /// test the quit-requested flag
    bool IsQuitRequested() const;
    
protected:
    /// the on-frame-callback
    virtual void OnFrame();
    
private:
    /// internal static on-frame helper method
    static void staticOnFrame();
};

#if ORYOL_WINDOWS
#define OryolApp(name, ver) \
void OryolMain(); \
const char* const OryolAppName = name; \
const char* const OryolAppVersion = ver; \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd) {\
    Core::WideString cmdLine = ::GetCommandLineW(); \
    Core::Args args(cmdLine); \
    OryolMain(args); \
    return 0; \
}
#else
#define OryolApp(name, ver) \
void OryolMain(); \
const char* const OryolAppName = name; \
const char* const OryolAppVersion = ver; \
int main(int argc, const char** argv) { \
    Core::Args args(argc, argv); \
    OryolMain(args); \
    return 0; \
}
#endif

} // namespace Core
} // namespace Oryol