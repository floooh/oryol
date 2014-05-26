//------------------------------------------------------------------------------
//  Log.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cstdio>
#include "Core/Log.h"
#include "Core/Assert.h"
#include "Core/Logger.h"
#include "Core/Threading/RWLock.h"
#include "Core/Containers/Array.h"
#if ORYOL_WINDOWS
#include "Windows.h"
#endif
#if ORYOL_ANDROID
#include <android/log.h>
#endif
#if ORYOL_PNACL
#include "Core/pnacl/pnaclInstance.h"
#endif

#if ORYOL_WINDOWS || ORYOL_PNACL
const Oryol::int32 LogBufSize = 8096;
#endif

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
        #if ORYOL_ANDROID
            android_LogPriority pri = ANDROID_LOG_DEFAULT;
            switch (lvl) {
                case Level::Error: pri = ANDROID_LOG_ERROR; break;
                case Level::Warn:  pri = ANDROID_LOG_WARN; break;
                case Level::Info:  pri = ANDROID_LOG_INFO; break;
                case Level::Dbg:   pri = ANDROID_LOG_DEBUG; break;
                default:           pri = ANDROID_LOG_DEFAULT; break;
            }
            __android_log_vprint(pri, "oryol", msg, args);
        #else
            #if ORYOL_WINDOWS || ORYOL_PNACL
            va_list argsCopy;
            va_copy(argsCopy, args);
            #endif

            // do the vprintf, this will destroy the original
            // va_list, so we made a copy before if necessary
            std::vprintf(msg, args);

            #if ORYOL_WINDOWS || ORYOL_PNACL
                char buf[LogBufSize];
                std::vsnprintf(buf, sizeof(buf), msg, argsCopy);
                buf[LogBufSize - 1] = 0;
                #if ORYOL_WINDOWS
                    OutputDebugString(buf);
                #elif ORYOL_PNACL
                    if (pnaclInstance::HasInstance()) {
                        pnaclInstance::Instance()->putMsg(buf);
                    }
                #endif
            #endif
        #endif
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
        #if ORYOL_ANDROID
            __android_log_print(ANDROID_LOG_FATAL, "oryol", "oryol assert: cond='%s'\nmsg='%s'\nfile='%s'\nline='%d'\nfunc='%s'\n", 
                cond, msg ? msg : "none", file, line, func);
        #else
            std::printf("oryol assert: cond='%s'\nmsg='%s'\nfile='%s'\nline='%d'\nfunc='%s'\n",
                        cond, msg ? msg : "none", file, line, func);
            #if ORYOL_WINDOWS
                char buf[LogBufSize];
                _snprintf_s(buf, sizeof(buf), _TRUNCATE, "oryol assert: cond='%s'\nmsg='%s'\nfile='%s'\nline='%d'\nfunc='%s'\n",
                            cond, msg ? msg : "none", file, line, func);
                buf[LogBufSize - 1] = 0;
                OutputDebugString(buf);
            #elif ORYOL_PNACL
                if (pnaclInstance::HasInstance()) {
                    char buf[LogBufSize];
                    std::snprintf(buf, sizeof(buf), "oryol assert: cond='%s'\nmsg='%s'\nfile='%s'\nline='%d'\nfunc='%s'\n",
                                  cond, msg ? msg : "none", file, line, func);
                    buf[LogBufSize - 1] = 0;                
                    pnaclInstance::Instance()->putMsg(buf);
                }
            #endif
        #endif
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
