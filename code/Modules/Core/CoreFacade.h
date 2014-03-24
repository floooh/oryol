#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::CoreFacade
    @brief facade singleton of the Core module
    
    @todo: CoreFacade description
*/
#include "Core/RefCounted.h"
#include "Core/Macros.h"
#include "Core/RunLoop.h"
#include <thread>

namespace Oryol {
namespace Core {

class CoreFacade {
    OryolGlobalSingletonDecl(CoreFacade);
public:
    /// constructor
    CoreFacade();
    /// destructor
    ~CoreFacade();
    
    /// get pointer to the per-thread runloop
    class RunLoop* RunLoop();

    /// private: called when a thread is entered
    void enterThread();
    /// private: called before a thread is left
    void leaveThread();

private:
    /// return true if main thread
    bool isMainThread() const;

    std::thread::id mainThreadId;
    static ORYOL_THREAD_LOCAL class RunLoop* threadRunLoop;
};

} // namespace Core
} // namespace Oryol
