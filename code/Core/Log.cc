//------------------------------------------------------------------------------
//  Log.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Log.h"
#include "Core/Macros.h"
#include "Core/Logger.h"
#include "Core/Threading/RWLock.h"
#include "Core/Containers/Array.h"

namespace Oryol {
namespace Core {

using namespace std;

Log::Level curLogLevel = Log::Level::Dbg;
RWLock lock;
Array<Ptr<Logger>> loggers;

//------------------------------------------------------------------------------
void
Log::AddLogger(const Ptr<Logger>& l) {
    if (l) {
        lock.LockWrite();
        loggers.AddBack(l);
        lock.UnlockWrite();
    }
}

//------------------------------------------------------------------------------
int32
Log::GetNumLoggers() {
    return loggers.Size();
}

//------------------------------------------------------------------------------
Ptr<Logger>
Log::GetLogger(int32 index) {
    return loggers[index];
}

//------------------------------------------------------------------------------
void
Log::SetLogLevel(Level l) {
    curLogLevel = l;
}

//------------------------------------------------------------------------------
Log::Level
Log::GetLogLevel() {
    return curLogLevel;
}

//------------------------------------------------------------------------------
void
Log::Dbg(const char* msg, ...) {
    if (curLogLevel >= Level::Dbg) {
        va_list args;
        va_start(args, msg);
        Log::vprint(Level::Dbg, msg, args);
        va_end(args);
    }
}

//------------------------------------------------------------------------------
void
Log::Info(const char* msg, ...) {
    if (curLogLevel >= Level::Info) {
        va_list args;
        va_start(args, msg);
        Log::vprint(Level::Info, msg, args);
        va_end(args);
    }
}

//------------------------------------------------------------------------------
void
Log::Warn(const char* msg, ...) {
    if (curLogLevel >= Level::Warn) {
        va_list args;
        va_start(args, msg);
        Log::vprint(Level::Warn, msg, args);
        va_end(args);
    }
}

//------------------------------------------------------------------------------
void
Log::Error(const char* msg, ...) {
    if (curLogLevel >= Level::Error) {
        va_list args;
        va_start(args, msg);
        Log::vprint(Level::Error, msg, args);
        va_end(args);
    }
}

//------------------------------------------------------------------------------
void
Log::vprint(Level lvl, const char* msg, va_list args) {
    lock.LockRead();
    if (loggers.Empty()) {
        ::vprintf(msg, args);
    }
    else {
        for (auto l : loggers) {
            l->VPrint(lvl, msg, args);
        }
    }
    lock.UnlockRead();
}

//------------------------------------------------------------------------------
void
Log::AssertMsg(const char* cond, const char* msg, const char* file, int32 line, const char* func) {
    lock.LockRead();
    if (loggers.Empty()) {
        ::printf("oryol assert: cond='%s' msg='%s', file='%s', line='%d', func='%s'\n",
                 cond, msg ? msg : "none", file, line, func);
    }
    else
    {
        for (const auto& l : loggers) {
            l->AssertMsg(cond, msg, file, line, func);
        }
    }
    lock.UnlockRead();
} 

} // namespace Core
} // namespace Oryol
