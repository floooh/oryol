//------------------------------------------------------------------------------
//  Core.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core.h"
#include "Core/RunLoop.h"
#include "Core/Threading/ThreadLocalPtr.h"
#include "Core/Trace.h"
#include <thread>

namespace Oryol {
    
namespace {
    ORYOL_THREADLOCAL_PTR(RunLoop) threadPreRunLoop = nullptr;
    ORYOL_THREADLOCAL_PTR(RunLoop) threadPostRunLoop = nullptr;
    struct _state {
        std::thread::id mainThreadId;
        #if ORYOL_PROFILING
        Trace trace;
        #endif
    };
    _state* state = nullptr;
}

//------------------------------------------------------------------------------
void
Core::Setup() {
    o_assert_dbg(!IsValid());
    o_assert_dbg(nullptr == threadPreRunLoop);
    o_assert_dbg(nullptr == threadPostRunLoop);
    state = Memory::New<_state>();
    state->mainThreadId = std::this_thread::get_id();
    threadPreRunLoop = Memory::New<RunLoop>();
    threadPostRunLoop = Memory::New<RunLoop>();
}

//------------------------------------------------------------------------------
void
Core::Discard() {
    o_assert(IsValid());
    o_assert(threadPreRunLoop);
    o_assert(threadPostRunLoop);
    Memory::Delete<RunLoop>(threadPreRunLoop);
    Memory::Delete<RunLoop>(threadPostRunLoop);
    Memory::Delete(state);
    threadPreRunLoop = nullptr;
    threadPostRunLoop = nullptr;
    state = nullptr;

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data!!!    
}

//------------------------------------------------------------------------------
bool
Core::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
RunLoop*
Core::PreRunLoop() {
    o_assert(threadPreRunLoop);
    return threadPreRunLoop;
}

//------------------------------------------------------------------------------
RunLoop*
Core::PostRunLoop() {
    o_assert(threadPostRunLoop);
    return threadPostRunLoop;
}

//------------------------------------------------------------------------------
bool
Core::IsMainThread() {
    #if ORYOL_HAS_THREADS
    o_assert_dbg(IsValid());
    return state->mainThreadId == std::this_thread::get_id();
    #else
    return true;
    #endif
}

//------------------------------------------------------------------------------
void
Core::EnterThread() {
    #if ORYOL_HAS_THREADS
    o_assert(nullptr == threadPreRunLoop);
    o_assert(nullptr == threadPostRunLoop);
    threadPreRunLoop = Memory::New<RunLoop>();
    threadPostRunLoop = Memory::New<RunLoop>();
    #endif
}

//------------------------------------------------------------------------------
void
Core::LeaveThread() {
    #if ORYOL_HAS_THREADS
    o_assert(threadPreRunLoop);
    o_assert(threadPostRunLoop);
    Memory::Delete<RunLoop>(threadPreRunLoop);
    Memory::Delete<RunLoop>(threadPostRunLoop);
    threadPreRunLoop = nullptr;
    threadPostRunLoop = nullptr;

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data
    #endif
}

} // namespace Oryol
