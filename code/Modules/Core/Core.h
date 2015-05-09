#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Core Core
    @brief Oryol core functionality

    @class Oryol::Core
    @ingroup Core
    @brief Core module facade
*/
#include "Core/RefCounted.h"
#include "Core/Threading/ThreadLocalPtr.h"
#include <thread>
#include "Core/Trace.h"

namespace Oryol {

class RunLoop;

class Core {
public:
    /// setup the Core module
    static void Setup();
    /// discard the Core module
    static void Discard();
    /// check if Core module has been setup
    static bool IsValid();
    
    /// get pointer to the per-thread 'before-frame' runloop
    static class RunLoop* PreRunLoop();
    /// get pointer to the per-thread 'after-frame' runloop
    static class RunLoop* PostRunLoop();

    /// called when a thread is entered
    static void EnterThread();
    /// called before a thread is left
    static void LeaveThread();
    /// test if we are on the main thread
    static bool IsMainThread();

private:
    static ORYOL_THREADLOCAL_PTR(RunLoop) threadPreRunLoop;
    static ORYOL_THREADLOCAL_PTR(RunLoop) threadPostRunLoop;
    struct _state {
        std::thread::id mainThreadId;
        #if ORYOL_PROFILING
        Trace trace;
        #endif
    };
    static _state* state;
};

} // namespace Oryol
