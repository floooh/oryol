//------------------------------------------------------------------------------
//  Logger.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Logger.h"

namespace Oryol {

//------------------------------------------------------------------------------
Logger::Logger() {
    // empty
}

//------------------------------------------------------------------------------
Logger::~Logger() {
    // empty
}

//------------------------------------------------------------------------------
void
Logger::VPrint(Log::Level /*l*/, const char* /*msg*/, va_list /*args*/) {
    // we can't do an o_error() here since it would recurse
}

//------------------------------------------------------------------------------
/**
 */
void
Logger::AssertMsg(const char* /*cond*/, const char* /*msg*/, const char* /*file*/, int /*line*/, const char* /*func*/) {
    // we can't do an o_error() here since it would recurse
}

} // namespace Oryol