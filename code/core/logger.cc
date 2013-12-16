//------------------------------------------------------------------------------
//  logger.cc
//------------------------------------------------------------------------------
#include "pre.h"
#include "core/logger.h"
#include "core/log.h"

namespace oryol {
namespace core {

//------------------------------------------------------------------------------
/**
*/
logger::logger() {
    // empty
}

//------------------------------------------------------------------------------
/**
*/
logger::~logger() {
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void logger::vprint(log::level /*l*/, const char* /*msg*/, va_list /*args*/) {
    // we can't do an o_error() here since it would recurse
}

//------------------------------------------------------------------------------
/**
 */
void logger::assert_msg(const char* /*cond*/, const char* /*msg*/, const char* /*file*/, int32 /*line*/, const char* /*func*/) {
    // we can't do an o_error() here since it would recurse
}

} // namespace core
} // namespace oryol