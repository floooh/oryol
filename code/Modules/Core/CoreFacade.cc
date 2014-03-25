//------------------------------------------------------------------------------
//  CoreFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "CoreFacade.h"

namespace Oryol {
namespace Core {
    
OryolGlobalSingletonImpl(CoreFacade);

ORYOL_THREAD_LOCAL RunLoop* CoreFacade::threadRunLoop = 0;

//------------------------------------------------------------------------------
CoreFacade::CoreFacade() {
    this->SingletonEnsureUnique();
    this->mainThreadId = std::this_thread::get_id();
    stringAtomTable::CreateSingleton();    
    threadRunLoop = RunLoop::Create();
    threadRunLoop->addRef();
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
    return this->mainThreadId == std::this_thread::get_id();
}

//------------------------------------------------------------------------------
void
CoreFacade::EnterThread() {
    o_assert(nullptr == threadRunLoop);
    
    // create thread-local string atom table
    stringAtomTable::CreateSingleton();
    
    // create thread-local run loop
    threadRunLoop = RunLoop::Create();
    threadRunLoop->addRef();
}

//------------------------------------------------------------------------------
void
CoreFacade::LeaveThread() {
    o_assert(nullptr != threadRunLoop);
    threadRunLoop->release();

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data    
}

} // namespace Core
} // namespace Oryol