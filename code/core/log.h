#pragma once
//------------------------------------------------------------------------------
/**
    @file core/log.h

    Basic logging functions.
*/
#include "core/types.h"
#include "core/threading/rwlock.h"

namespace oryol {
namespace core {

class logger;

class log {
public:
    enum struct level {
        none,
        error,
        warn,
        info,
        dbg,

        num_levels,
        invalid_level
    };

    /// add a logger object
    static void add_logger(const std::shared_ptr<logger>& p);
    /// get number of loggers
    static int32 get_numloggers();
    /// get logger at index
    static std::shared_ptr<logger> get_logger(int32 index);
    
    /// set current log level
    static void set_loglevel(level l);
    /// get current log level
    static level get_loglevel();
    /// print a debug message
    static void dbg(const char* msg, ...);
    /// print an info message
    static void info(const char* msg, ...);
    /// print a warning
    static void warn(const char* msg, ...);
    /// print an error (use o_error() macro to also abort the program)
    static void error(const char* msg, ...);
    /// print an assert message
    static void assert_msg(const char* cond, const char* msg, const char* file, int32 line, const char* func);

private:
    /// generic vprint-style method
    static void vprint(level l, const char* msg, va_list args);

    static threading::rwlock lock;
    static level cur_loglevel;
    static std::vector<std::shared_ptr<logger>> loggers;
};
} // namespace core
} // namespace oryol