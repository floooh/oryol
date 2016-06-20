#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Log
    @ingroup Core
    @brief logging functions

    The Log class provides a central place for logging. By default,
    output is logged to stdout and stderr, but custom Logger objects
    can be attached to handle log output differently.

    @see Logger
*/
#include <cstdarg>
#include "Core/Types.h"
#include "Core/Config.h"

namespace Oryol {

class Logger;
template<class TYPE> class Ptr;

class Log {
public:
    /// log levels
    enum class Level {
        None,
        Error,
        Warn,
        Info,
        Dbg,

        NumLevels,
        InvalidLevel
    };

    /// add a logger object
    static void AddLogger(const Ptr<Logger>& p);
    /// get number of loggers
    static int GetNumLoggers();
    /// get logger at index
    static Ptr<Logger> GetLogger(int index);
    
    /// set current log level
    static void SetLogLevel(Level l);
    /// get current log level
    static Level GetLogLevel();
    /// print a debug message
    static void Dbg(const char* msg, ...) __attribute__((format(printf, 1, 2)));
    /// print a debug message (with va_list)
    static void VDbg(const char* msg, va_list args);
    /// print an info message
    static void Info(const char* msg, ...) __attribute__((format(printf, 1, 2)));
    /// print an info message (with va_list)
    static void VInfo(const char* msg, va_list args);
    /// print a warning
    static void Warn(const char* msg, ...) __attribute__((format(printf, 1, 2)));
    /// print a warning (with va_list)
    static void VWarn(const char* msg, va_list args);
    /// print an error (use o_error() macro to also abort the program)
    static void Error(const char* msg, ...) __attribute__((format(printf, 1, 2)));
    /// print an error (with va_list)
    static void VError(const char* msg, va_list args);
    /// print an assert message
    static void AssertMsg(const char* cond, const char* msg, const char* file, int line, const char* func);

private:
    /// generic vprint-style method
    static void vprint(Level l, const char* msg, va_list args) __attribute__((format(printf, 2, 0)));
};

/// shortcut for Log::Dbg()
#define o_dbg(...) do { Oryol::Log::Dbg(__VA_ARGS__); } while(0)
/// shortcut for Log::Info()
#define o_info(...) do { Oryol::Log::Info(__VA_ARGS__); } while(0)
/// shortcut for Log::Warn()
#define o_warn(...) do { Oryol::Log::Warn(__VA_ARGS__); } while(0)
/// shortcut for Log::Error()
#define o_error(...) do { Oryol::Log::Error(__VA_ARGS__); ORYOL_TRAP(); } while(0)


} // namespace Oryol
