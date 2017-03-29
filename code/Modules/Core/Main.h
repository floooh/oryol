#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Main.h
    @brief Oryol application entry wrapper

    This file defines the macro OryolMain() which expects the name
    of the Oryol App subclass as argument. Include this file in your
    Main.cc (and only there). This header includes big platform-specific headers
    (like Windows.h), so it is not recommended to include the header anywhere 
    else to not pollute the whole code base with platform-specific headers.
*/
#if ORYOL_WINDOWS && !ORYOL_UWP
#define VC_EXTRALEAN (1)
#define WIN32_LEAN_AND_MEAN (1)
#define NOUSER (1)
#include <Windows.h>
#elif ORYOL_UWP
#include "Core/uwp/uwpBridge.h"
#elif ORYOL_ANDROID
#include "Core/android/android_native_app_glue.h"
#elif ORYOL_PNACL
#include "Core/pnacl/pnaclModule.h"
#endif
#include "Core/App.h"
#include "Core/String/WideString.h"

#if ORYOL_UWP
// on UWP, the UI event loop is running in it's own thread,
// need to move creation of the Oryol app object to the
// UI thread, thus the lambda
#define OryolMain(clazz) \
Oryol::Args OryolArgs; \
[Platform::MTAThread] \
int main(Platform::Array<Platform::String^>^) { \
    Oryol::_priv::uwpBridge app; \
    app.start([]() -> App* { \
        return Memory::New<clazz>(); \
    }); \
    return 0; \
}
#elif ORYOL_WINDOWS
#define OryolMain(clazz) \
Oryol::Args OryolArgs; \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd) {\
    Oryol::WideString cmdLine = ::GetCommandLineW(); \
    OryolArgs = Oryol::Args(cmdLine); \
    clazz* app = Memory::New<clazz>(); \
    app->StartMainLoop(); \
    Oryol::Memory::Delete<clazz>(app); \
    return 0; \
}
#elif ORYOL_ANDROID
#define OryolMain(clazz) \
android_app* OryolAndroidAppState = nullptr; \
Oryol::Args OryolArgs; \
void android_main(struct android_app* app_) { \
    app_dummy(); \
    OryolAndroidAppState = app_; \
    clazz* app = Memory::New<clazz>(); \
    app->StartMainLoop(); \
    Oryol::Memory::Delete<clazz>(app); \
}
#elif ORYOL_PNACL
#define OryolMain(clazz) \
namespace pp \
{ \
    Module* CreateModule() \
    { \
        return Memory::New<Oryol::_priv::pnaclModule>(); \
    }; \
} \
void PNaclAppCreator() {\
    static clazz* app = Memory::New<clazz>(); \
    app->StartMainLoop(); \
}
#else
#define OryolMain(clazz) \
Oryol::Args OryolArgs; \
int main(int argc, const char** argv) { \
    OryolArgs = Oryol::Args(argc, argv); \
    clazz* app = Oryol::Memory::New<clazz>(); \
    app->StartMainLoop(); \
    Oryol::Memory::Delete(app); \
    return 0; \
}
#endif
