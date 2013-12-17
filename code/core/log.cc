//------------------------------------------------------------------------------
//  log.cc
//------------------------------------------------------------------------------
#include "pre.h"
#include "core/log.h"
#include "core/macros.h"
#include "core/logger.h"

namespace oryol {
namespace core {

using namespace std;

log::level log::cur_loglevel = log::level::dbg;
threading::rwlock log::lock;
std::vector<std::shared_ptr<logger>> log::loggers;

//------------------------------------------------------------------------------
/**
*/
void log::add_logger(const shared_ptr<logger>& l) {
    if (l) {
        lock.lock_write();
        loggers.push_back(l);
        lock.unlock_write();
    }
}

//------------------------------------------------------------------------------
/**
 */
int32 log::get_numloggers() {
    return loggers.size();
}

//------------------------------------------------------------------------------
/**
 */
std::shared_ptr<logger> log::get_logger(int32 index) {
    return loggers[index];
}

//------------------------------------------------------------------------------
/**
*/
void log::set_loglevel(level l) {
    cur_loglevel = l;
}

//------------------------------------------------------------------------------
/**
*/
log::level log::get_loglevel() {
    return cur_loglevel;
}

//------------------------------------------------------------------------------
/**
*/
void log::dbg(const char* msg, ...) {
    if (cur_loglevel >= level::dbg) {
        va_list args;
        va_start(args, msg);
        log::vprint(level::dbg, msg, args);
        va_end(args);
    }
}

//------------------------------------------------------------------------------
/**
*/
void log::info(const char* msg, ...) {
    if (cur_loglevel >= level::info) {
        va_list args;
        va_start(args, msg);
        log::vprint(level::info, msg, args);
        va_end(args);
    }
}

//------------------------------------------------------------------------------
/**
*/
void log::warn(const char* msg, ...) {
    if (cur_loglevel >= level::warn) {
        va_list args;
        va_start(args, msg);
        log::vprint(level::warn, msg, args);
        va_end(args);
    }
}

//------------------------------------------------------------------------------
/**
*/
void log::error(const char* msg, ...) {
    if (cur_loglevel >= level::error) {
        va_list args;
        va_start(args, msg);
        log::vprint(level::error, msg, args);
        va_end(args);
    }
}

//------------------------------------------------------------------------------
/**
*/
void log::vprint(level lvl, const char* msg, va_list args) {
    lock.lock_read();
    if (loggers.empty()) {
        vprintf(msg, args);
    }
    else {
        for (auto l : loggers) {
            l->vprint(lvl, msg, args);
        }
    }
    lock.unlock_read();
}

//------------------------------------------------------------------------------
/**
*/
void log::assert_msg(const char* cond, const char* msg, const char* file, int32 line, const char* func) {
    lock.lock_read();
    if (loggers.empty()) {
        ::printf("oryol assert: cond='%s' msg='%s', file='%s', line='%d', func='%s'\n",
                 cond, msg ? "none" : msg, file, line, func);
    }
    else
    {
        for (auto l : loggers) {
            l->assert_msg(cond, msg, file, line, func);
        }
    }
    lock.unlock_read();
} 

} // namespace core
} // namespace oryol