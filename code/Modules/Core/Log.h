#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Log
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
namespace Core {

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
    static int32 GetNumLoggers();
    /// get logger at index
    static Ptr<Logger> GetLogger(int32 index);
    
    /// set current log level
    static void SetLogLevel(Level l);
    /// get current log level
    static Level GetLogLevel();
    /// print a debug message
    static void Dbg(const char* msg, ...) __attribute__((format(printf, 1, 2)));
    /// print an info message
    static void Info(const char* msg, ...) __attribute__((format(printf, 1, 2)));
    /// print a warning
    static void Warn(const char* msg, ...) __attribute__((format(printf, 1, 2)));
    /// print an error (use o_error() macro to also abort the program)
    static void Error(const char* msg, ...) __attribute__((format(printf, 1, 2)));
    /// print an assert message
    static void AssertMsg(const char* cond, const char* msg, const char* file, int32 line, const char* func);

private:
    /// generic vprint-style method
    static void vprint(Level l, const char* msg, va_list args) __attribute__((format(printf, 2, 0)));
};
} // namespace Core
} // namespace Oryol
