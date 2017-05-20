#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Core Core
    @brief Oryol core functionality

    @class Oryol::Core
    @ingroup Core
    @brief Core module facade
*/
#include "Core/Types.h"
#include "Core/RunLoop.h"

namespace Oryol {

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
};

} // namespace Oryol
