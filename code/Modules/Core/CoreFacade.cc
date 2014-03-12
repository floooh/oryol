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
    threadRunLoop = RunLoop::Create();
    threadRunLoop->addRef();
}

//------------------------------------------------------------------------------
CoreFacade::~CoreFacade() {
    o_assert(this->isMainThread());
    o_assert(nullptr != threadRunLoop);
    threadRunLoop->release();
    threadRunLoop = 0;
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
CoreFacade::enterThread() {
    o_assert(!this->isMainThread());
    o_assert(nullptr == threadRunLoop);
    threadRunLoop = RunLoop::Create();
    threadRunLoop->addRef();
}

//------------------------------------------------------------------------------
void
CoreFacade::leaveThread() {
    o_assert(!this->isMainThread());
    o_assert(nullptr != threadRunLoop);
    threadRunLoop->release();
}

} // namespace Core
} // namespace Oryol