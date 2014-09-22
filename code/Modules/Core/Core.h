#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Core Core
    @brief Core module

    @class Oryol::Core
    @ingroup Core
    @brief Core module facade
*/
#include "Core/RefCounted.h"
#include <thread>

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

private:
    /// return true if main thread
    static bool isMainThread();
    
    static ORYOL_THREAD_LOCAL class RunLoop* threadPreRunLoop;
    static ORYOL_THREAD_LOCAL class RunLoop* threadPostRunLoop;
    struct _state {
        std::thread::id mainThreadId;
    };
    static _state* state;
};

} // namespace Oryol
