//------------------------------------------------------------------------------
//  CoreFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "CoreFacade.h"
#include "Core/Ptr.h"

namespace Oryol {
namespace Core {
    
OryolGlobalSingletonImpl(CoreFacade);

ORYOL_THREAD_LOCAL RunLoop* CoreFacade::threadRunLoop = 0;

//------------------------------------------------------------------------------
CoreFacade::CoreFacade() {
    this->SingletonEnsureUnique();
    this->mainThreadId = std::this_thread::get_id();
    stringAtomTable::CreateSingle();
    auto ptr = RunLoop::Create();
    ptr->addRef();
    threadRunLoop = ptr.get();
}

//------------------------------------------------------------------------------
CoreFacade::~CoreFacade() {
    o_assert(this->isMainThread());
    o_assert(nullptr != threadRunLoop);
    threadRunLoop->release();
    threadRunLoop = 0;

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data!!!    
}

//------------------------------------------------------------------------------
RunLoop*
CoreFacade::RunLoop() {
    o_assert(nullptr != threadRunLoop);
    return threadRunLoop;
}

//------------------------------------------------------------------------------
bool
CoreFacade::isMainThread() const {
    #if ORYOL_HAS_THREADS
    return this->mainThreadId == std::this_thread::get_id();
    #else
    return true;
    #endif
}

//------------------------------------------------------------------------------
void
CoreFacade::EnterThread() {
    #if ORYOL_HAS_THREADS
    o_assert(nullptr == threadRunLoop);
    
    // create thread-local string atom table
    stringAtomTable::CreateSingle();
    
    // create thread-local run loop
    auto ptr = RunLoop::Create();
    ptr->addRef();
    threadRunLoop = ptr.get();
    #endif
}

//------------------------------------------------------------------------------
void
CoreFacade::LeaveThread() {
    #if ORYOL_HAS_THREADS
    o_assert(nullptr != threadRunLoop);
    threadRunLoop->release();

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data
    #endif
}

} // namespace Core
} // namespace Oryol