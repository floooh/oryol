#pragma once
//------------------------------------------------------------------------------
/**
    @class oryol::core::logger
    
    Base class for loggers. Create logger objects and add them to
    oryol::core::log.
*/
#include "core/types.h"
#include "core/macros.h"
#include "core/log.h"
#include "core/refcounted.h"

namespace oryol {
namespace core {

class logger : public refcounted {
    oryol_class_decl(logger, 1);
public:
    /// constructor
    logger();
    /// destructor
    virtual ~logger();
    /// generic vprint-style method
    virtual void vprint(log::level l, const char* msg, va_list args);
    /// print an assert message
    virtual void assert_msg(const char* cond, const char* msg, const char* file, int32 line, const char* func);
};
    
} // namespace core
} // namespace oryol
