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
#if ORYOL_WINDOWS
#define VC_EXTRALEAN (1)
#define WIN32_LEAN_AND_MEAN (1)
#include <Windows.h>
#endif
#if ORYOL_ANDROID
#include "android_native/android_native_app_glue.h"
#endif
#if ORYOL_PNACL
#include "Core/pnacl/pnaclModule.h"
#endif
#include "Core/App.h"

#if ORYOL_WINDOWS
#define OryolMain(clazz) \
Oryol::Args OryolArgs; \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd) {\
    Oryol::WideString cmdLine = ::GetCommandLineW(); \
    OryolArgs = Oryol::Args(cmdLine); \
    clazz* app = Memory::New<clazz>(); \
    app->StartMainLoop(); \
    Memory::Delete<clazz>(app); \
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
    Memory::Delete<clazz>(app); \
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
    clazz* app = Memory::New<clazz>(); \
    app->StartMainLoop(); \
    Memory::Delete(app); \
    return 0; \
}
#endif
