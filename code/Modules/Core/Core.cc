//------------------------------------------------------------------------------
//  Core.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core.h"
#include "Core/RunLoop.h"
#include "Core/Ptr.h"

namespace Oryol {
    
Core::_state* Core::state = nullptr;
ORYOL_THREAD_LOCAL RunLoop* Core::threadRunLoop = nullptr;

//------------------------------------------------------------------------------
void
Core::Setup() {
    o_assert(!IsValid());
    state = new _state();
    state->mainThreadId = std::this_thread::get_id();
    auto ptr = RunLoop::Create();
    ptr->addRef();
    threadRunLoop = ptr.get();
}

//------------------------------------------------------------------------------
void
Core::Discard() {
    o_assert(IsValid());
    o_assert(nullptr != threadRunLoop);
    threadRunLoop->release();
    threadRunLoop = 0;
    delete state;
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
Core::RunLoop() {
    o_assert(nullptr != threadRunLoop);
    return threadRunLoop;
}

//------------------------------------------------------------------------------
bool
Core::isMainThread() {
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
    o_assert(nullptr == threadRunLoop);
    
    // create thread-local run loop
    auto ptr = RunLoop::Create();
    ptr->addRef();
    threadRunLoop = ptr.get();
    #endif
}

//------------------------------------------------------------------------------
void
Core::LeaveThread() {
    #if ORYOL_HAS_THREADS
    o_assert(nullptr != threadRunLoop);
    threadRunLoop->release();

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data
    #endif
}

} // namespace Oryol