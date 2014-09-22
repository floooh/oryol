//------------------------------------------------------------------------------
//  Core.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core.h"
#include "Core/RunLoop.h"
#include "Core/Ptr.h"

namespace Oryol {
    
Core::_state* Core::state = nullptr;
ORYOL_THREAD_LOCAL RunLoop* Core::threadPreRunLoop = nullptr;
ORYOL_THREAD_LOCAL RunLoop* Core::threadPostRunLoop = nullptr;

//------------------------------------------------------------------------------
void
Core::Setup() {
    o_assert(!IsValid());
    state = new _state();
    state->mainThreadId = std::this_thread::get_id();
    
    // setup the before-frame runloop
    auto ptr = RunLoop::Create();
    ptr->addRef();
    threadPreRunLoop = ptr.get();

    // setup the after-frame runloop
    ptr = RunLoop::Create();
    ptr->addRef();
    threadPostRunLoop = ptr.get();
}

//------------------------------------------------------------------------------
void
Core::Discard() {
    o_assert(IsValid());
    o_assert(nullptr != threadPreRunLoop);
    o_assert(nullptr != threadPostRunLoop);
    
    threadPreRunLoop->release();
    threadPreRunLoop = 0;
    threadPostRunLoop->release();
    threadPostRunLoop = 0;
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
Core::PreRunLoop() {
    o_assert(nullptr != threadPreRunLoop);
    return threadPreRunLoop;
}

//------------------------------------------------------------------------------
RunLoop*
Core::PostRunLoop() {
    o_assert(nullptr != threadPostRunLoop);
    return threadPostRunLoop;
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
    o_assert(nullptr == threadPreRunLoop);
    o_assert(nullptr == threadPostRunLoop);
    
    // create thread-local run loops
    auto ptr = RunLoop::Create();
    ptr->addRef();
    threadPreRunLoop = ptr.get();
    ptr = RunLoop::Create();
    ptr->addRef();
    threadPostRunLoop = ptr.get();
    #endif
}

//------------------------------------------------------------------------------
void
Core::LeaveThread() {
    #if ORYOL_HAS_THREADS
    o_assert(nullptr != threadPreRunLoop);
    o_assert(nullptr != threadPostRunLoop);
    threadPreRunLoop->release();
    threadPreRunLoop = nullptr;
    threadPostRunLoop->release();
    threadPostRunLoop = nullptr;

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data
    #endif
}

} // namespace Oryol